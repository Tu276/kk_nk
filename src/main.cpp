#if defined(ESP32)
#include <WiFiMulti.h>
WiFiMulti wifiMulti;
#define DEVICE "ESP32"
#elif defined(ESP8266)
#include <ESP8266WiFiMulti.h>
ESP8266WiFiMulti wifiMulti;
#define DEVICE "ESP8266"
#endif

#include <InfluxDbClient.h>
#include <InfluxDbCloud.h>

// WiFi AP SSID
#define WIFI_SSID "YOUR_WIFI_SSID"
// WiFi password
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"

#define INFLUXDB_URL "https://eastus-1.azure.cloud2.influxdata.com"
#define INFLUXDB_TOKEN "CRzxyq-yuAjoE88OAug6zsKLDe-3_6pXNX_Eq1gNGCo0aO2EhSB3CKHDhJPNo-VvxmMKfctMPNglnnjzGnDaIA=="
#define INFLUXDB_ORG "0144ab1e8021e8f0"
#define INFLUXDB_BUCKET "YOUR_BUCKET"

// Time zone info
#define TZ_INFO "UTC3"

// Declare InfluxDB client instance with preconfigured InfluxCloud certificate
InfluxDBClient client(INFLUXDB_URL, INFLUXDB_ORG, INFLUXDB_BUCKET, INFLUXDB_TOKEN, InfluxDbCloud2CACert);

// Declare Data point
Point sensor("wifi_status");

void setup()
{
  Serial.begin(115200);

  // Setup wifi
  WiFi.mode(WIFI_STA);
  wifiMulti.addAP(WIFI_SSID, WIFI_PASSWORD);

  Serial.print("Connecting to wifi");
  while (wifiMulti.run() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(100);
  }
  Serial.println();

  // Accurate time is necessary for certificate validation and writing in batches
  // We use the NTP servers in your area as provided by: https://www.pool.ntp.org/zone/
  // Syncing progress and the time will be printed to Serial.
  timeSync(TZ_INFO, "pool.ntp.org", "time.nis.gov");

  // Check server connection
  if (client.validateConnection())
  {
    Serial.print("Connected to InfluxDB: ");
    Serial.println(client.getServerUrl());
  }
  else
  {
    Serial.print("InfluxDB connection failed: ");
    Serial.println(client.getLastErrorMessage());
  }
}
void loop() {}

// // DHT Temperature & Humidity Sensor
// // Unified Sensor Library Examples

// #include <Arduino.h>
// #include <Adafruit_Sensor.h>
// #include <DHT.h>
// #include <DHT_U.h>
// #include <OneWire.h>
// #include <DallasTemperature.h>

// #define DHTPIN 25 // Digital pin connected to the DHT sensor
// // Feather HUZZAH ESP8266 note: use pins 3, 4, 5, 12, 13 or 14 --
// // Pin 15 can work but DHT must be disconnected during program upload.

// // Uncomment the type of sensor in use:
// //#define DHTTYPE    DHT11     // DHT 11
// #define DHTTYPE DHT22 // DHT 22 (AM2302)
// //#define DHTTYPE    DHT21     // DHT 21 (AM2301)

// // See guide for details on sensor wiring and usage:
// //   https://learn.adafruit.com/dht/overview

// DHT_Unified dht(DHTPIN, DHTTYPE);

// uint32_t delayMS;

// // Data wire is plugged into digital pin 3 on the Arduino
// #define ONE_WIRE_BUS 26

// // Setup a oneWire instance to communicate with any OneWire device
// OneWire oneWire(ONE_WIRE_BUS);

// // Pass oneWire reference to DallasTemperature library
// DallasTemperature sensors(&oneWire);

// void setup()
// {
//   Serial.begin(115200);
//   // Initialize device.
//   dht.begin();
//   Serial.println(F("DHTxx Unified Sensor Example"));
//   // Print temperature sensor details.
//   sensor_t sensor;
//   dht.temperature().getSensor(&sensor);
//   Serial.println(F("------------------------------------"));
//   Serial.println(F("Temperature Sensor"));
//   Serial.print(F("Sensor Type: "));
//   Serial.println(sensor.name);
//   Serial.print(F("Driver Ver:  "));
//   Serial.println(sensor.version);
//   Serial.print(F("Unique ID:   "));
//   Serial.println(sensor.sensor_id);
//   Serial.print(F("Max Value:   "));
//   Serial.print(sensor.max_value);
//   Serial.println(F("°C"));
//   Serial.print(F("Min Value:   "));
//   Serial.print(sensor.min_value);
//   Serial.println(F("°C"));
//   Serial.print(F("Resolution:  "));
//   Serial.print(sensor.resolution);
//   Serial.println(F("°C"));
//   Serial.println(F("------------------------------------"));
//   // Print humidity sensor details.
//   dht.humidity().getSensor(&sensor);
//   Serial.println(F("Humidity Sensor"));
//   Serial.print(F("Sensor Type: "));
//   Serial.println(sensor.name);
//   Serial.print(F("Driver Ver:  "));
//   Serial.println(sensor.version);
//   Serial.print(F("Unique ID:   "));
//   Serial.println(sensor.sensor_id);
//   Serial.print(F("Max Value:   "));
//   Serial.print(sensor.max_value);
//   Serial.println(F("%"));
//   Serial.print(F("Min Value:   "));
//   Serial.print(sensor.min_value);
//   Serial.println(F("%"));
//   Serial.print(F("Resolution:  "));
//   Serial.print(sensor.resolution);
//   Serial.println(F("%"));
//   Serial.println(F("------------------------------------"));
//   // Set delay between sensor readings based on sensor details.
//   delayMS = sensor.min_delay / 1000;
// }

// void loop()
// {
//   // Delay between measurements.
//   delay(delayMS);
//   // Get temperature event and print its value.
//   sensors_event_t event;
//   dht.temperature().getEvent(&event);
//   if (isnan(event.temperature))
//   {
//     Serial.println(F("Error reading temperature!"));
//   }
//   else
//   {
//     Serial.print(F("Ambient Temperature: "));
//     Serial.print(event.temperature);
//     Serial.println(F("°C"));
//   }
//   // Get humidity event and print its value.
//   dht.humidity().getEvent(&event);
//   if (isnan(event.relative_humidity))
//   {
//     Serial.println(F("Error reading humidity!"));
//   }
//   else
//   {
//     Serial.print(F("Humidity: "));
//     Serial.print(event.relative_humidity);
//     Serial.println(F("%"));
//   }
//   // Send the command to get temperatures
//   sensors.requestTemperatures();

//   // print the temperature in Celsius
//   Serial.print(" Bearing Temperature: ");
//   Serial.print(sensors.getTempCByIndex(0));
//   Serial.print((char)176); // shows degrees character
//   Serial.print("C  |  ");

//   // print the temperature in Fahrenheit
//   Serial.print((sensors.getTempCByIndex(0) * 9.0) / 5.0 + 32.0);
//   Serial.print((char)176); // shows degrees character
//   Serial.println("F");

//   delay(1000);
// }