//#include <Adafruit_BME280.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include "esp_adc_cal.h"
#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Adafruit_Sensor.h>
#include "DHT.h";
#include <Adafruit_SSD1306.h>
#include <TinyGPS++.h>
#include <HardwareSerial.h>

TinyGPSPlus gps;
HardwareSerial GPSSerial(1);
Adafruit_SSD1306 display(128, 32, &Wire);
OneWire oneWire(13);
DallasTemperature sensors(&oneWire);
DHT dht(14, DHT11);


char* DEVICEID = "mvktt01";

// Replace the next variables with your SSID/Password combination
const char* ssid = "Aw geez Rick theres a password";
const char* password = "SpaceRoses13";

int msgcounter = 0;

// Add your MQTT Broker IP address, example:
const char* mqtt_server = "52.117.240.201";

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

void displayClear() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);
}

void setup() {
  display.begin();
  displayClear();
  display.println("Starting");
  display.println("Node: " + String(DEVICEID));
  display.display();

  Serial.begin(115200);
  GPSSerial.begin(9600, SERIAL_8N1, 12, 15);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  sensors.begin();
  dht.begin();
}

static void smartDelay(unsigned long ms) // https://github.com/LilyGO/TTGO-T-Beam/blob/master/GPS/GPS.ino
{
  unsigned long start = millis();
  do
  {
    while (GPSSerial.available())
      gps.encode(GPSSerial.read());
  } while (millis() - start < ms);
}

void send(char* topic, String mqttmsg) {
  char toSend[40];
  strcpy(toSend, DEVICEID);
  Serial.println("Sending MQTT Message");
  Serial.println(mqttmsg.c_str());
  client.publish(strcat(toSend, topic), mqttmsg.c_str(), true);
}

String macToStr(const uint8_t* mac)
{
  String result;
  for (int i = 0; i < 6; ++i) {
    result += String(mac[i], 16);
    if (i < 5)
      result += ':';
  }
  return result;
}

void loop() {
  msgcounter++;

  if (!client.connected()) reconnect();
  client.loop();
  displayClear();

  sensors.requestTemperatures();
  if (sensors.getTempCByIndex(0) != -127) {
    display.println("DALtemp: " + String(sensors.getTempCByIndex(0)) + "ºC");
    Serial.println("DALtemp: " + String(sensors.getTempCByIndex(0)) + "ºC");
  } else {
    Serial.println("Invalid reading from DAL");
    display.println("DAL reading invalid");
  }

  float t = dht.readTemperature();
  float h = dht.readHumidity();

  display.println("DHTtemp: " + String(t) + "ºC");
  display.println("DHT hum: " + String(h) + "%");

  Serial.println("DHTtemp: " + String(t) + "ºC");
  Serial.println("DHT hum: " + String(h) + "%");

  smartDelay(1000);
  if (millis() > 5000 && gps.charsProcessed() < 10) {
    display.println("No GPS");
    Serial.println("GPS Error");
  } else {
    Serial.println("Lat: " + String(gps.location.lat()) + ", Lng: " + String(gps.location.lng()));
  }

  display.display();

  String msg = "{\"deviceid\":\"" + String(DEVICEID) + "\",\"description\":\"Testing device\",\"msg\":" + String(msgcounter) + ", \"mac\":\"" + WiFi.macAddress() + "\",\"ip\":\"" + ipToString(WiFi.localIP()) + "\"";//,\"voltage\":" + String((myvol * 3.9) / 4095);
  if (sensors.getTempCByIndex(0) != -127) msg += ",\"dalTemp\":" + String(sensors.getTempCByIndex(0));
  //if (BMEstatus && !bme.error) msg += ",\"bmeTemp\":" + String(bme.temperature) + ",\"bmePres\":" + String(bme.pressure) + ",\"bmeHum\":" + String(bme.humidity);
  if (true) msg += ",\"dhtTemp\":" + String(t) + ",\"dhtHum\":" + String(h);
  msg += "}";

  send("/data", msg);
  delay(15000);
}

void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");

  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
  }
  Serial.println();
}

String ipToString(IPAddress ip){
  String s="";
  for (int i=0; i<4; i++)
    s += i  ? "." + String(ip[i]) : String(ip[i]);
  return s;
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
      // Subscribe
       client.subscribe("sensor/jw00T/command");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
