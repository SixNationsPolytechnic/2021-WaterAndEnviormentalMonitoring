#include "SSD1306Wire.h"
#include <Wire.h>
//#include <Adafruit_BME280.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include "esp_adc_cal.h"

char* DEVICEID   =    "sensor/jw00T/data";

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

// GPIO where the DS18B20 is connected to
const int oneWireBus = 2;     
OneWire oneWire(oneWireBus);
DallasTemperature sensors(&oneWire);

SSD1306Wire display(0x3c, SDA, SCL);
//Adafruit_BME280 bme;

void initOLED() {
  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
}

void displayOLED() {
  display.clear();

  
  display.setFont(ArialMT_Plain_10);
  display.drawString(0, 0, "Hola");
  display.setFont(ArialMT_Plain_16);
  display.drawString(0, 10, "Ni Hao Ma");
  display.setFont(ArialMT_Plain_24);
  display.drawString(0, 26, "Namaste");

  display.setFont(ArialMT_Plain_10);
  display.drawString(0, 50, String(millis()));

  display.display();
}

void setup() {
   adc1_config_width(ADC_WIDTH_12Bit);
  adc1_config_channel_atten(ADC1_CHANNEL_0, ADC_ATTEN_0db); //set reference voltage to internal
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  
  initOLED();
//  bool status;
//  status = bme.begin(uint8_t(0x76), forBME);
  sensors.begin();
  if (false) {
    Serial.println("No valid BME280 sensor.");
    display.clear();
    display.setFont(ArialMT_Plain_24);
    display.drawString(0, 0, "NO BME280!");
    display.display();
    delay(2000);
    while (true);
  }
}

 float temperature;
 //float pressure;
 //float humidity;

void send(String mqttmsg) {
  Serial.println("Sending MQTT Message");
  Serial.println(mqttmsg.c_str()); 
  client.publish(DEVICEID, mqttmsg.c_str(), true);
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
  
  sensors.requestTemperatures(); 
  temperature = sensors.getTempCByIndex(0);
  //temperature = bme.readTemperature();
  //pressure = bme.readPressure();
  //humidity = bme.readHumidity();

  display.clear();
  display.drawString(0, 0, "Temperature: " + String(temperature) + "ºC");
  //display.drawString(10, 0, "  Pressure: " + String(pressure));
  //display.drawString(20, 0, "  Humidity: " + String(humidity));
  display.display();
  Serial.println("Temperature: " + String(temperature) + "ºC");
  float myvol = adc1_get_raw(ADC1_CHANNEL_0);
  Serial.println("Voltage: " + String(myvol));

  String msg = "{\"msgnumber\" :  " + String(msgcounter) + ", \"mac\" :  \"" + WiFi.macAddress() + "\",\"ip\" : \"" + WiFi.localIP() + "\",\"voltage\" :  " + String(myvol) + ", \"temperature\" : "+String(temperature)+"}";;
  send(msg);
  delay(5000);  
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
