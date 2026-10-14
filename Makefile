ARDUINO_CLI ?= arduino-cli
FQBN := esp32:esp32:XIAO_ESP32C6
EXAMPLE ?= MQTTLed
PORT ?=

.PHONY: help compile upload

help:
	@printf '%s\n' \
		'make compile EXAMPLE=Blink                 Compile an example.' \
		'make upload EXAMPLE=WiFiScanner            Compile and upload to one detected ESP32.' \
		'make upload EXAMPLE=MQTTLed PORT=<port>     Compile and upload to a specified port.' \
		'EXAMPLE defaults to MQTTLed; use any sketch directory in this repository.'

compile:
	@test -f "$(EXAMPLE)/$(EXAMPLE).ino" || { \
		echo 'Unknown example: $(EXAMPLE). Use a sketch directory from this repository.' >&2; \
		exit 1; \
	}
	@if [ "$(EXAMPLE)" = 'MQTTLed' ] && [ ! -f "$(EXAMPLE)/Config.h" ]; then \
		echo 'Missing MQTTLed/Config.h. Copy Config.h.example and fill in local settings.' >&2; \
		exit 1; \
	fi
	$(ARDUINO_CLI) compile --fqbn $(FQBN) "$(EXAMPLE)"

upload: compile
	@port="$(PORT)"; \
	if [ -z "$$port" ]; then \
		boards="$$( $(ARDUINO_CLI) board list )" || exit $$?; \
		port="$$(printf '%s\n' "$$boards" | awk '$$1 ~ /^(\/dev\/|COM[0-9]+$$)/ && /ESP32/ { print $$1 }')"; \
	fi; \
	set -- $$port; \
	if [ "$$#" -ne 1 ]; then \
		echo 'Specify one port: run arduino-cli board list, then make upload EXAMPLE=$(EXAMPLE) PORT=<port>.' >&2; \
		exit 1; \
	fi; \
	echo "Uploading to $$1..."; \
	$(ARDUINO_CLI) upload -p "$$1" --fqbn $(FQBN) "$(EXAMPLE)"
