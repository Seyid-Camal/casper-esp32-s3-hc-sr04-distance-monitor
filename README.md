# CASPER Distance MCP — ESP32 Firmware

ESP32-S3 + HC-SR04 ultrasonic distance sensor. The ESP32 measures distance and sends JSON readings by HTTP POST to a local server. The backend and MCP integration are not included in this initial firmware-only package; add them only after a separate review.

## Hardware

- ESP32-S3 development board
- HC-SR04 ultrasonic sensor
- 5 V supply for the sensor, common GND
- TRIG → GPIO 10, ECHO → GPIO 11 **through a resistor voltage divider**

**Important:** A typical HC-SR04 ECHO output is 5 V. ESP32-S3 GPIO inputs are 3.3 V devices. Do not connect ECHO directly. One divider option: 2 kΩ between ECHO and GPIO 11, and 3.3 kΩ between GPIO 11 and GND (about 3.1 V).

## Configuration

1. Open `firmware/casper_distance_esp32/casper_distance_esp32.ino` in Arduino IDE.
2. In that same directory, copy `secrets.example.h` to `secrets.h`.
3. Edit **only your local** `secrets.h` and add the Wi-Fi name, your **newly changed** Wi-Fi password, and the local HTTP endpoint.
4. Select your ESP32-S3 board and COM port, then upload the sketch.
5. Open Serial Monitor at **115200 baud**.

Example outgoing payload (not a live result):

```json
{"device":"ESP32-S3","distance_cm":25.4}
```

## Security

- `secrets.h` is ignored by Git and **must never be uploaded**.
- Change any Wi-Fi password that was pasted into a chat or committed to a repository.
- The included HTTP endpoint is plaintext and unauthenticated. Use only on a trusted local network, do not expose TCP port 8769 to the internet, and add authentication and TLS before using untrusted networks.
- Review files **and Git history** for secrets before making a repository public.

## Status

- ESP32 firmware: prepared for testing.
- Backend / MCP / Kali Linux integration: to be verified and added separately.
