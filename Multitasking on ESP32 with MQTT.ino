#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>

#define DHT_SENSOR_PIN 21
#define DHT_SENSOR_TYPE DHT11

// Task handles
TaskHandle_t mqttTaskHandle;
TaskHandle_t mqttTaskHandle2;
TaskHandle_t sensorTaskHandle;

const char *ssid = "Legion-5";
const char *password = "LEGION-5";

// MQTT Broker IP
const char *mqtt_server = "192.168.137.1";

// Define Sensor Pins
const int ACSpin = 34;
const int trigPin = 5;
const int echoPin = 18;

// define sound speed in cm/uS
#define SOUND_SPEED 0.034

WiFiClient espClient;
PubSubClient client(espClient);
DHT dht_sensor(DHT_SENSOR_PIN, DHT_SENSOR_TYPE);

long lastMsg = 0;
char msg[50];
int value = 0;
long duration = 0;

float distanceCm = 0;
float distanceInch = 0;

float humi = 0;
float tempC = 0;

int mVperAmp = 185; // Calibration Factor: 5A - 185mV/Amp, 30A - 66mV/Amp
int Watt = 0;
double Voltage = 0;
double Amps = 0;
uint32_t delayMS;

void setup()
{
  Serial.begin(115200);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  dht_sensor.begin();

  setup_wifi();

  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  // Create tasks
  xTaskCreatePinnedToCore(
    mqttTask,        // Task function
    "MQTTTask",      // Task name
    10000,           // Stack size
    NULL,            // Task parameters
    1,               // Priority
    &mqttTaskHandle, // Task handle
    0);              // Core 0

  xTaskCreatePinnedToCore(
    mqttComTask,        // Task function
    "MQTTComTask",      // Task name
    10000,           // Stack size
    NULL,            // Task parameters
    2,               // Priority
    &mqttTaskHandle2, // Task handle
    0);              // Core 0

  xTaskCreatePinnedToCore(
    sensorTask,        // Task function
    "SensorTask",      // Task name
    10000,             // Stack size
    NULL,              // Task parameters
    1,                 // Priority
    &sensorTaskHandle, // Task handle
    1);                // Core 1
}

void mqttTask(void *pvParameters)
{
  while (1)
  {
    Serial.println("MQTT Task Running");

    if (!client.connected())
    {
      reconnect();
    }
    client.loop();

    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}
void mqttComTask(void *pvParameters)
{
  while (1)
  {
    Serial.println("MQTT Communication Task Running");

    char tempString[8], tempString2[8], tempString3[8], tempString4[8], tempString5[8];
    dtostrf(Amps, 1, 2, tempString);
    dtostrf(Watt, 1, 2, tempString2);
    dtostrf(distanceCm, 1, 2, tempString3);
    dtostrf(tempC, 1, 2, tempString4);
    dtostrf(humi, 1, 2, tempString5);

    // send the message to the MQTT broker
    client.publish("esp32/amp", tempString);
    client.publish("esp32/watt", tempString2);
    client.publish("esp32/us", tempString3);
    client.publish("esp32/temp", tempString4);
    client.publish("esp32/humi", tempString5);

    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

void sensorTask(void *pvParameters)
{
  while (1)
  {
    Serial.println("sensorTask Running");
    long now = millis();
    if (now - lastMsg > 2000)
    {
      lastMsg = now;

      humi = dht_sensor.readHumidity();
      tempC = dht_sensor.readTemperature();

      // check whether the reading is successful or not
      if (isnan(tempC) || isnan(humi))
      {
        Serial.println("Failed to read from DHT sensor!");
      }

      // Clears the trigPin
      digitalWrite(trigPin, LOW);
      delayMicroseconds(2);
      // Sets the trigPin on HIGH state for 10 micro seconds
      digitalWrite(trigPin, HIGH);
      delayMicroseconds(10);
      digitalWrite(trigPin, LOW);

      // Reads the echoPin, returns the sound wave travel time in microseconds
      duration = pulseIn(echoPin, HIGH);

      distanceCm = duration * SOUND_SPEED / 2;

      if (distanceCm > 1000) {
        distanceCm = 0;
      }
      else {
        distanceCm = distanceCm;
      }

      // Calculating Current from Sensor Reading.
      Voltage = getVPP();
      Amps = ((Voltage * 1000) / mVperAmp) - 0; // -1.87 is the error correction factor.
      Watt = (Amps * 12 );
      delay(100); // delay in between reads for clear read from serial
    }

    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

void loop()
{
}

// Connect to WiFi network
void setup_wifi()
{
  delay(100);

  Serial.println();
  Serial.print("Connecting to Wifi");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char *topic, byte *message, unsigned int length)
{
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;

  for (int i = 0; i < length; i++)
  {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();
}

void reconnect()
{
  while (!client.connected())
  { // Loop until connected to the MQTT server
    Serial.print("Trying MQTT connection...");
    if (client.connect("ESP8266Client"))
    { // Try to connect
      Serial.println("connected");
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000); // Wait 5 seconds before retrying
    }
  }
}

// Function to get Voltage Levels as Current Sensor Input.
float getVPP()
{
  float result;
  int readValue;       // value read from the sensor
  int maxValue = 0;    // max value
  int minValue = 4096; // min value = ESP32 ADC resolution

  uint32_t start_time = millis();
  while ((millis() - start_time) < 1000) // sample for 1 Sec
  {
    readValue = analogRead(ACSpin);

    // Check for a new maxValue
    if (readValue > maxValue)
    {
      /*record the maximum sensor value*/
      maxValue = readValue;
    }
    if (readValue < minValue)
    {
      /*record the minimum sensor value*/
      minValue = readValue;
    }
  }

  result = ((maxValue - minValue) * 3.3) / 4096.0; // ESP32 ADC resolution 4096
  return result;
}
