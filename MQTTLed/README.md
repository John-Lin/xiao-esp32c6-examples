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
not necessarily `en1`. Put the resulting IPv4 address in `mqttHost` below. The ESP32 must reach that address from a 2.4 GHz Wi-Fi
network; guest networks/client isolation may prevent this. The Mac may use
Ethernet or a different Wi-Fi band as long as the devices can reach each other.

Allow incoming connections to Mosquitto if macOS prompts you. If the Mac's DHCP
address changes, update `mqttHost` in `MQTTLed/Config.h` and rebuild/upload (or reserve the address in
your router). The Mac must remain awake for the broker to be reachable.

## 3. Configure and upload the sketch

From the repository root, copy the public template without overwriting existing
local settings:

```sh
test -e MQTTLed/Config.h || cp MQTTLed/Config.h.example MQTTLed/Config.h
chmod 600 MQTTLed/Config.h
```

Edit `MQTTLed/Config.h` with your local settings:

```cpp
constexpr char wifiSsid[] = "YOUR_WIFI_SSID";
constexpr char wifiPassword[] = "YOUR_WIFI_PASSWORD";
constexpr char mqttHost[] = "192.168.1.100";
```

`mqttHost` is the LAN IP of the computer running the broker. Do not use
`localhost` or `127.0.0.1`: on the ESP32 those mean the ESP32 itself.
Set `wifiPassword` to `""` for an open Wi-Fi network. These are C++ string
literals: escape a quote as `\"` and a backslash as `\\` inside the strings.

The sketch includes `Config.h` directly. No environment variables, generated
headers, Node.js, or direnv are required.

Compile, find the port, and upload from the repository root:

```sh
arduino-cli lib install "PubSubClient@2.8"
arduino-cli compile --fqbn esp32:esp32:XIAO_ESP32C6 MQTTLed
arduino-cli board list
arduino-cli upload -p <port> --fqbn esp32:esp32:XIAO_ESP32C6 MQTTLed
arduino-cli monitor -p <port> --config baudrate=115200
```

Replace `<port>` with the value reported by `arduino-cli board list`. Wait for
`MQTT connected. Subscribed to xiao/led/set` before publishing.

`MQTTLed/Config.h` is ignored by Git. Commit `Config.h.example`, not your local
header. Credentials are still embedded in the compiled firmware and Arduino
build cache: Git ignore does not encrypt them. Do not share build artifacts.
Changing `Config.h` requires rebuilding and uploading.

## 4. Publish from another Mac terminal

Turn the LED on:

```sh
mosquitto_pub -h <broker-ip> -p 1883 -t xiao/led/set -m ON
```

Turn it off:

```sh
mosquitto_pub -h <broker-ip> -p 1883 -t xiao/led/set -m OFF
```

Replace `<broker-ip>` with the broker IP you put in `Config.h`. `127.0.0.1`
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

With a connected board, check:

1. Boot: the user LED is off, then Wi-Fi and MQTT connect.
2. Publish `ON`, then `OFF`: the LED and serial output match each command.
3. Publish `on`, `INVALID`, or an empty message (`mosquitto_pub -h <broker-ip>
   -t xiao/led/set -n`): the LED stays unchanged and serial reports it was ignored.
4. Stop/restart the broker: the board reconnects and accepts new commands.
5. Disable/restore the access point: Wi-Fi and MQTT recover, then new commands work.
6. Reboot the board: it starts off and subscribes again.

If Wi-Fi connects but MQTT does not, check `mqttHost`, the broker listener,
macOS firewall, and network isolation. If Wi-Fi never connects, check the SSID,
password, and 2.4 GHz network availability.

## References

- [PubSubClient API](https://pubsubclient.knolleary.net/api)
- [Mosquitto configuration](https://mosquitto.org/man/mosquitto-conf-5.html)
- [XIAO ESP32C6 hardware](https://wiki.seeedstudio.com/xiao_esp32c6_getting_started/)
