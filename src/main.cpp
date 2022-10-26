// including required libraries
// DHT Temperature & Humidity Sensor
// Unified Sensor Library Examples

#include <Arduino.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <InfluxDbClient.h>
#include <InfluxDbCloud.h>

// select board type
#if defined(ESP32)
#include <WiFiMulti.h>
WiFiMulti wifiMulti;
#define DEVICE "ESP32"
#elif defined(ESP8266)
#include <ESP8266WiFiMulti.h>
ESP8266WiFiMulti wifiMulti;
#define DEVICE "ESP8266"
#endif

/// network credentials as variables for board internet access

// WiFi AP SSID
#define WIFI_SSID "James"
// WiFi password
#define WIFI_PASSWORD "kirimijk"

#define INFLUXDB_URL "https://eastus-1.azure.cloud2.influxdata.com"
#define INFLUXDB_TOKEN "kj9FAI4Ng5XrAynQys1BKfsf1lQ4hzd5mI4rdYRt3g3vLY2VDNL90HHkN4vGM6p32WXxSyNaKD2N0NVLdn_huQ=="
#define INFLUXDB_ORG "0144ab1e8021e8f0"
#define INFLUXDB_BUCKET "kk_nk"

// Time zone info
#define TZ_INFO "UTC3"

// Declare InfluxDB client instance with preconfigured InfluxCloud certificate
InfluxDBClient client(INFLUXDB_URL, INFLUXDB_ORG, INFLUXDB_BUCKET, INFLUXDB_TOKEN, InfluxDbCloud2CACert);

#define DHTPIN 25 // Digital pin connected to the DHT sensor

//#define DHTTYPE    DHT11     // DHT 11
#define DHTTYPE DHT22 // DHT 22 (AM2302)

DHT_Unified dht(DHTPIN, DHTTYPE);

uint32_t delayMS;

// Data wire is plugged into digital pin 26 on the esp
#define ONE_WIRE_BUS 26

// Setup a oneWire instance to communicate with any OneWire device
OneWire oneWire(ONE_WIRE_BUS);

// Pass oneWire reference to DallasTemperature library
DallasTemperature sensors(&oneWire);

// Declare Data point
// Point sensor("wifi_status"); //////////////##########

Point sensorReadings("measurements");

float temperature_Celcius;
float humidity;

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

  //  Initialize device.
  dht.begin();
  Serial.println(F("DHTxx Unified Sensor Example"));
  // Add tags to the data point adding the device name,
  // the name of the sensor, and the location of the sensor.
  sensorReadings.addTag("device", DEVICE);
  sensorReadings.addTag("location", "bearing");
  sensorReadings.addTag("sensor", "dht22");

  // Print temperature sensor details.

  sensor_t sensor;
  dht.temperature().getSensor(&sensor);
  Serial.println(F("------------------------------------"));
  Serial.println(F("Temperature Sensor"));
  Serial.print(F("Sensor Type: "));
  Serial.println(sensor.name);
  Serial.print(F("Driver Ver:  "));
  Serial.println(sensor.version);
  Serial.print(F("Unique ID:   "));
  Serial.println(sensor.sensor_id);
  Serial.print(F("Max Value:   "));
  Serial.print(sensor.max_value);
  Serial.println(F("°C"));
  Serial.print(F("Min Value:   "));
  Serial.print(sensor.min_value);
  Serial.println(F("°C"));
  Serial.print(F("Resolution:  "));
  Serial.print(sensor.resolution);
  Serial.println(F("°C"));
  Serial.println(F("------------------------------------"));
  // Print humidity sensor details.
  dht.humidity().getSensor(&sensor);
  Serial.println(F("Humidity Sensor"));
  Serial.print(F("Sensor Type: "));
  Serial.println(sensor.name);
  Serial.print(F("Driver Ver:  "));
  Serial.println(sensor.version);
  Serial.print(F("Unique ID:   "));
  Serial.println(sensor.sensor_id);
  Serial.print(F("Max Value:   "));
  Serial.print(sensor.max_value);
  Serial.println(F("%"));
  Serial.print(F("Min Value:   "));
  Serial.print(sensor.min_value);
  Serial.println(F("%"));
  Serial.print(F("Resolution:  "));
  Serial.print(sensor.resolution);
  Serial.println(F("%"));
  Serial.println(F("------------------------------------"));
  // Set delay between sensor readings based on sensor details.
  delayMS = sensor.min_delay / 1000;

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

void loop()
{
  // Clear fields for reusing the point. Tags will remain the same as set above.
  sensorReadings.clearFields();

  // Delay between measurements.
  delay(delayMS);
  // Get temperature event and print its value.
  sensors_event_t event;
  dht.temperature().getEvent(&event);
  if (isnan(event.temperature))
  {
    Serial.println(F("Error reading temperature!"));
  }
  else
  {
    Serial.print(F("Ambient Temperature: "));
    Serial.print(event.temperature);
    Serial.println(F("°C"));
  }
  float temperature = event.temperature;
  // Get humidity event and print its value.
  dht.humidity().getEvent(&event);
  if (isnan(event.relative_humidity))
  {
    Serial.println(F("Error reading humidity!"));
  }
  else
  {
    Serial.print(F("Humidity: "));
    Serial.print(event.relative_humidity);
    Serial.println(F("%"));
  }
  float humidity = event.relative_humidity;
  // Send the command to get temperatures
  sensors.requestTemperatures();

  // print the temperature in Celsius
  Serial.print(" Bearing Temperature: ");
  Serial.print(sensors.getTempCByIndex(0));
  Serial.print((char)176); // shows degrees character
  Serial.print("C  |  ");

  // print the temperature in Fahrenheit
  Serial.print((sensors.getTempCByIndex(0) * 9.0) / 5.0 + 32.0);
  Serial.print((char)176); // shows degrees character
  Serial.println("F");

  humidity = dht.readHumidity();

  delay(1000);

  // Store measured value into point
  // Report RSSI of currently connected network
  sensorReadings.addField("rssi", WiFi.RSSI());

  // Print what are we exactly writing
  Serial.print("Writing: ");
  Serial.println(sensor.toLineProtocol());

  // Check WiFi connection and reconnect if needed
  if (wifiMulti.run() != WL_CONNECTED)
  {
    Serial.println("Wifi connection lost");
  }

  // Write point
  if (!client.writePoint(sensor))
  {
    Serial.print("InfluxDB write failed: ");
    Serial.println(client.getLastErrorMessage());
  }

  Serial.println("Waiting 1 second");
  delay(1000);

  // // ... flux query
  // // n this query, we are looking for data points within the last 1 minute with a measurement of "wifi_status".

  // // Query will find the RSSI values for last minute for each connected WiFi network with this device
  // String query = "from(bucket: \"kk_nk\")\n\
  // |> range(start: -1m)\n\
  // |> filter(fn: (r) => r._measurement == \"wifi_status\" and r._field == \"rssi\")";

  // // Print composed query
  // Serial.println("Querying for RSSI values written to the \"kk_nk\" bucket in the last 1 min... ");
  // Serial.println(query);

  // // Send query to the server and get result
  // FluxQueryResult result = client.query(query);

  // Serial.println("Results : ");
  // // Iterate over rows.
  // while (result.next())
  // {
  //   // Get converted value for flux result column 'SSID'
  //   String ssid = result.getValueByName("SSID").getString();
  //   Serial.print("SSID '");
  //   Serial.print(ssid);

  //   Serial.print("' with RSSI ");
  //   // Get value of column named '_value'
  //   long value = result.getValueByName("_value").getLong();
  //   Serial.print(value);

  //   // Get value for the _time column
  //   FluxDateTime time = result.getValueByName("_time").getDateTime();

  //   String timeStr = time.format("%F %T");

  //   Serial.print(" at ");
  //   Serial.print(timeStr);

  //   Serial.println();
  // }

  // // Report any error
  // if (result.getError() != "")
  // {
  //   Serial.print("Query result error: ");
  //   Serial.println(result.getError());
  // }

  // // Close the result
  // result.close();

  // Serial.println("==========");

  // delay(5000);

  // /// In this example, we use the mean() function to calculate the average value of data points in the last 1 minute.
  // //
  // //

  // // ... code from Write Data step

  // // Query will find the min RSSI value for last minute for each connected WiFi network with this device
  // String aggregate_query = "from(bucket: \"kk_nk\")\n\
  // |> range(start: -1m)\n\
  // |> filter(fn: (r) => r._measurement == \"wifi_status\")\n\
  // |> min()";

  // // Print composed query
  // Serial.println("Querying for the mean RSSI value written to the \"kk_nk\" bucket in the last 1 min... ");
  // Serial.println(aggregate_query);

  // // Send query to the server and get result
  // FluxQueryResult aggregate_result = client.query(aggregate_query);

  // Serial.println("Result : ");
  // // Iterate over rows.
  // while (aggregate_result.next())
  // {
  //   // Get converted value for flux result column 'SSID'
  //   String ssid = aggregate_result.getValueByName("SSID").getString();
  //   Serial.print("SSID '");
  //   Serial.print(ssid);

  //   Serial.print("' with RSSI ");
  //   // Get value of column named '_value'
  //   long value = aggregate_result.getValueByName("_value").getLong();
  //   Serial.print(value);

  //   // Get value for the _time column
  //   FluxDateTime time = aggregate_result.getValueByName("_time").getDateTime();

  //   String timeStr = time.format("%F %T");

  //   Serial.print(" at ");
  //   Serial.print(timeStr);

  //   Serial.println();
  // }

  // // Report any error
  // if (aggregate_result.getError() != "")
  // {
  //   Serial.print("Query result error: ");
  //   Serial.println(aggregate_result.getError());
  // }

  // // Close the result
  // aggregate_result.close();

  // Serial.println("==========");

  // delay(5000);
}
