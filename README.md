# ESP32 CO2 Sensor

This software allows a SparkFun ESP32 "Thing Plus" to read atmospheric data from its environment, and provide that data to consumers over WiFi.

Requires a Sensiron SCD30 CO2/Temperature/Humidity sensor connected to the Qwiic port of the ESP32 Plus.

This software is only tested for the SparkFun ESP32 "Thing" Plus (ESP32 WROOM), and should be used at your own risk regardless.

## Usage

After powering on, the ESP32 will immediately attempt to connect to WiFi. The SSID and PSK it uses to connect are to be configured before uploading the program to the device, and cannot be altered after compile-time.

Once connected to WiFi, the ESP32 will begin collecting sensor data and awaiting HTTP connections. The ESP32 will respond to any GET request with a small JSON object representing the last-known sensor state.

For example:
```
{
	"co2": 468,
	"temp": 16.2,
	"humidity": 18.8
}
```

The status light will blink continuously while the device is attempting to connect to WiFi. Once connected, it will announce its local IP address over serial, if possible. Without access to a serial connection, it may be necessary to locate the device using another method.

If the SCD30 air sensor cannot be initialized, the status light will blink intermittently. In this case, all connections between the ESP32 and the air sensor should be verified.

In the event that the connection between the ESP32 and the SCD30 is interrupted during the device's operation, it will continue responding to requests with the most recent data.


