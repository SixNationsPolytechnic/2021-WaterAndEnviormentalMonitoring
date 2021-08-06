/*
  This file sends random data to the server every 5 minutes.
*/

#include <WiFi.h>
#include <PubSubClient.h>
#include <WiFiUdp.h>
#include <NTPClient.h>

char* DEVICEID = "jw00T";

// Replace the next variables with your SSID/Password combination
const char* ssid = "Mom use this to print";
const char* password = "SpaceRoses13";

int msgcounter = 0;

const char* mqtt_server = "52.117.240.201";

WiFiClient espClient;
WiFiUDP ntpUDP;
PubSubClient client(espClient);
NTPClient timeClient(ntpUDP);

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  timeClient.begin();
}

void send(char* topic, String mqttmsg) {
  char toSend[40] = "sensor/";
  strcat(toSend, DEVICEID);
  strcat(toSend, topic);
  Serial.print("Sending MQTT Message to ");
  Serial.println(String(toSend));
  Serial.println(mqttmsg.c_str());
  client.publish(toSend, mqttmsg.c_str(), true);
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

  while(!timeClient.update()) {
    timeClient.forceUpdate();
  }

  String msg = "{";
  msg += "\"deviceid\":\"" + String(DEVICEID) + "\"";
  msg += ",\"mac\":\"" + WiFi.macAddress() + "\"";
  msg += ",\"ip\":\"" + ipToString(WiFi.localIP()) + "\"";
  msg += ",\"msg\":" + String(msgcounter);
  msg += ",\"tsutc\":" + String(timeClient.getEpochTime());
  msg += ",\"mqi\":300000";
  msg += ",\"voltage\":" + String((float(rand())/float((RAND_MAX)) * 3.3));
  msg += ",\"ampere\":" + String((float(rand())/float((RAND_MAX)) * 5.0));
  msg += ",\"batlevel\":" + String((float(rand())/float((RAND_MAX)) * 100.0));
  msg += ",\"location\":\"Narnia\"";
  msg += ",\"light\":" + String((float(rand())/float((RAND_MAX)) * 100.0));
  msg += ",\"temperature\":" + String((float(rand())/float((RAND_MAX)) * 40.0));
  msg += ",\"humidity\":" + String((float(rand())/float((RAND_MAX)) * 100.0));
  //msg += ",\"microphone\":" + String((float(rand())/float((RAND_MAX)) * 90.0));
  //msg += ",\"airquality\":" + String((float(rand())/float((RAND_MAX)) * 100.0));
  msg += "}";

  send("/data", msg);
  delay(300000);
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
      char topic[50];
      strcpy(topic, String("sensor/" + String(DEVICEID) + "/command").c_str());
      client.subscribe(topic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
