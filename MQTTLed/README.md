# MQTT LED

Controls the XIAO ESP32C6 onboard user LED from MQTT messages sent by your Mac.

```text
Mac publisher -> Mosquitto on the Mac -> Wi-Fi -> XIAO ESP32C6 LED
```

Only one topic is needed: `xiao/led/set`. Send exactly `ON` or `OFF` (uppercase,
without whitespace). Other messages are ignored and leave the LED unchanged.
The user LED is active-low (`LOW` = on, `HIGH` = off), and starts off at boot.
No external LED is needed. This does not control the power/charging indicator.

## 1. Start Mosquitto on macOS

From the repository root, after `brew install mosquitto`:

```sh
"$(brew --prefix)/sbin/mosquitto" -c MQTTLed/mosquitto.conf -v
```

Leave this terminal open. Stop the broker with Ctrl-C when finished. This uses
only the repository's configuration; it does not change Homebrew's configuration.

Mosquitto's default no-listener configuration only accepts local connections.
The supplied configuration listens on TCP port 1883 on all IPv4 interfaces and
allows anonymous connections so the ESP32 can connect from your LAN.

**Trusted-LAN testing only:** there is no password or TLS. Any client that can
reach the broker can control the LED. Do not forward port 1883 to the internet
or run this configuration on an untrusted network. Authentication/TLS are not
implemented by this example.

If port 1883 is already in use, inspect the listener:

```sh
lsof -nP -iTCP:1883 -sTCP:LISTEN
```

If it is a Homebrew Mosquitto service you started, stop it before running this
foreground broker:

```sh
brew services stop mosquitto
```

Do not run two brokers on the same port.

## 2. Find the broker computer's LAN IP

```sh
networksetup -listallhardwareports
```

Find the `Device` under the active Wi-Fi or Ethernet hardware port. For example,
if Wi-Fi uses `en1`:

```sh
ipconfig getifaddr en1
```

Run these commands on the Mac running the broker. Use its actual interface,
not necessarily `en1`. Put the resulting IPv4 address in `MQTT_HOST` below. The ESP32 must reach that address from a 2.4 GHz Wi-Fi
network; guest networks/client isolation may prevent this. The Mac may use
Ethernet or a different Wi-Fi band as long as the devices can reach each other.

Allow incoming connections to Mosquitto if macOS prompts you. If the Mac's DHCP
address changes, update `MQTT_HOST` in `.envrc` and rebuild/upload (or reserve the address in
your router). The Mac must remain awake for the broker to be reachable.

## 3. Configure and upload the sketch

Requires Arduino CLI, Node.js, and direnv. On macOS, install Node.js and direnv
with `brew install node direnv` if needed.

From the repository root, create the local configuration without overwriting
an existing `.envrc`:

```sh
test -e .envrc || cp .envrc.example .envrc
```

Edit `.envrc` with your local settings:

```sh
export WIFI_SSID='YOUR_WIFI_SSID'
export WIFI_PASSWORD='YOUR_WIFI_PASSWORD'
export MQTT_HOST='192.168.1.100'
```

`MQTT_HOST` is the LAN IP of the computer running the broker. Do not use
`localhost` or `127.0.0.1`: on the ESP32 those mean the ESP32 itself.
Set `WIFI_PASSWORD=''` explicitly for an open network. Values cannot contain
control characters. `.envrc` is shell code; quote values carefully, especially
passwords containing `$`, quotes, or backslashes.

Approve your local file after creating or editing it, then compile:

```sh
direnv allow .
arduino-cli lib install "PubSubClient@2.8"
direnv exec . scripts/build-mqtt-led.sh
```

The build script regenerates `MQTTLed/Config.h` from the environment before each
compile. Missing/invalid settings stop the build instead of using an old header.
Use this script rather than compiling the MQTT sketch directly. `direnv exec`
loads `.envrc` explicitly, so no shell hook is required.

Find the port, then compile and upload in one command:

```sh
arduino-cli board list
direnv exec . scripts/build-mqtt-led.sh --upload <port>
arduino-cli monitor -p <port> --config baudrate=115200
```

Replace `<port>` with the value reported by `arduino-cli board list`. Wait for
`MQTT connected. Subscribed to xiao/led/set` before publishing.

`.envrc`, `.direnv/`, and the generated `MQTTLed/Config.h` are ignored by Git.
Commit `.envrc.example`, not real credentials. The generated header uses private
file permissions. Credentials are still embedded in the compiled firmware and
Arduino build cache: Git ignore does not encrypt them. Do not share build
artifacts, use shell tracing (`set -x`) with secrets, or print the environment.
Changing `.envrc` requires rebuilding and uploading; it is not read at runtime.

## 4. Publish from another Mac terminal

Turn the LED on:

```sh
direnv exec . sh -c 'mosquitto_pub -h "$MQTT_HOST" -p 1883 -t xiao/led/set -m ON'
```

Turn it off:

```sh
direnv exec . sh -c 'mosquitto_pub -h "$MQTT_HOST" -p 1883 -t xiao/led/set -m OFF'
```

Run these commands from the repository root. They use the broker IP from
`.envrc`, including when the broker runs on a different computer. `127.0.0.1`
is only appropriate for publishing from the computer running the broker.

The serial monitor prints `LED ON` or `LED OFF` after handling the message.
There is no state-report topic: a successful publish alone does not prove the
board received it. Check the LED and serial output.

Commands are not retained by these publish commands. Commands sent while the
board is offline are not replayed; publish again after it subscribes. The LED
keeps its last state during disconnection and resets to off after reboot.
The sketch retries MQTT connections every five seconds after failures and
resubscribes after reconnection. Wi-Fi recovery waits until the network returns.

## Verification

Run hardware-independent tests from the repository root:

```sh
scripts/test-mqtt-led.sh
```

These test the actual payload decoder and configuration scripts. Configuration
tests compile generated headers with a real C++ compiler to check escaping and
UTF-8, and verify missing settings, private permissions, and error reporting
without credentials in logs. They do not verify networking or physical GPIO output.

With a connected board, check:

1. Boot: the user LED is off, then Wi-Fi and MQTT connect.
2. Publish `ON`, then `OFF`: the LED and serial output match each command.
3. Publish `on`, `INVALID`, or an empty message (`direnv exec . sh -c
   'mosquitto_pub -h "$MQTT_HOST" -t xiao/led/set -n'`): the LED stays unchanged
   and serial reports it was ignored.
4. Stop/restart the broker: the board reconnects and accepts new commands.
5. Disable/restore the access point: Wi-Fi and MQTT recover, then new commands work.
6. Reboot the board: it starts off and subscribes again.

If Wi-Fi connects but MQTT does not, check `MQTT_HOST`, the broker listener,
macOS firewall, and network isolation. If Wi-Fi never connects, check the SSID,
password, and 2.4 GHz network availability.

## References

- [direnv](https://direnv.net/)
- [PubSubClient API](https://pubsubclient.knolleary.net/api)
- [Mosquitto configuration](https://mosquitto.org/man/mosquitto-conf-5.html)
- [XIAO ESP32C6 hardware](https://wiki.seeedstudio.com/xiao_esp32c6_getting_started/)
