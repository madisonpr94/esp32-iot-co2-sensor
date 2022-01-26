/*
Author: Madison Pratt
Date created: January 23rd, 2022

This software allows a SparkFun ESP32 "Thing" to read atmospheric data from
its environment and provide that data to web services via WiFi.

Requires a Sensiron SCD30 CO2/Temp/Humidity sensor connected to the ESP32's 
Qwiic port.
*/

#include <SparkFun_SCD30_Arduino_Library.h>
#include <WiFi.h> // Include for ESP32 on-board WiFi

// WiFi Network Definitions
#define THIS_SSID "{SSID}" // Must use 2.4GHz WiFi
#define THIS_PSK "{PSK}"

// HTTP Strings
#define HTTP_OK_HEADERS "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n\r\n"

// Other defines
#define ONBOARD_STATUS_LED_PIN 13

// Sensor Data
SCD30 scd30 = SCD30();
uint16_t CO2 = 0;
float Temp = 0.0f;
float Humidity = 0.0f;

// Web services
WiFiServer server(80);

void setStatusLed(bool value)
{
  if (value) {
    digitalWrite(ONBOARD_STATUS_LED_PIN, HIGH);
  }
  else {
    digitalWrite(ONBOARD_STATUS_LED_PIN, LOW);
  }
}

void setup()
{
  // setup serial communication at 115200 baud
  Serial.begin(115200);

  // enable i2c communication on qwiic port
  Wire.begin();

  // begin connecting to the wireless network
  WiFi.begin(THIS_SSID, THIS_PSK);

  // indicate status
  bool statusLightOn = true;
  pinMode(ONBOARD_STATUS_LED_PIN, OUTPUT);
  setStatusLed(statusLightOn);
  
  // blink status LED while connecting to wifi
  while (WiFi.status() != WL_CONNECTED) {
    delay(250);

    statusLightOn = !statusLightOn;
    setStatusLed(statusLightOn);
  }
  digitalWrite(ONBOARD_STATUS_LED_PIN, LOW);

  // log ip
  Serial.println("WiFi connected.");
  Serial.print("Local IP: ");
  Serial.print(WiFi.localIP());
  Serial.println("");

  // open server on port 80
  server.begin();
  server.setTimeout(15); // set timeout at 15 seconds to avoid polling sensors too frequently

  // initialize SCD30 air sensor
  while (!scd30.begin())
  {
    Serial.println(F("SCD30 Air Sensor not detected. Please check connections."));
      
    // indicate an error
    digitalWrite(ONBOARD_STATUS_LED_PIN, HIGH);
    delay(100);
    digitalWrite(ONBOARD_STATUS_LED_PIN, LOW);

    delay(100);

    digitalWrite(ONBOARD_STATUS_LED_PIN, HIGH);
    delay(100);
    digitalWrite(ONBOARD_STATUS_LED_PIN, LOW);

    delay(1000);
  }

  // signal device is ready
  digitalWrite(ONBOARD_STATUS_LED_PIN, HIGH);
}

void loop()
{
  // get sensor data via i2c
  checkSCD30();

  // wait on server for connections
  // this also ensures we don't poll our sensor too quickly
  serverListen();
}

// SCD30 Air Sensor - CO2 concentration, Humidity, Temperature
void checkSCD30()
{
  if (scd30.dataAvailable())
  {
    // update latest data if available
    CO2 = scd30.getCO2();
    Temp = scd30.getTemperature();
    Humidity = scd30.getHumidity();
  }
}

String getSensorDataJson()
{
  String result = "";

  result += "{";

  result += "\"co2\":";
  result += CO2;
  result += ",";
  
  result += "\"temp\":";
  result += Temp;
  result += ",";
  
  result += "\"humidity\":";
  result += Humidity;
  
  result += "}";  

  return result;
}

void serverListen()
{
  WiFiClient client = server.available();

  if (client) {
    String currentLine = "";
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c); // log request

        if (c == '\n') {
          if (currentLine.length() == 0) {
            // Two newline characters in a row - end of HTTP request
            client.println(HTTP_OK_HEADERS);
            client.println(getSensorDataJson());
            client.println();
            break;
          }
          else {
            currentLine = "";
          }
        }
        else if (c != '\r') {
          currentLine += c;
        }
      }
    }
    client.stop();
  }
}
