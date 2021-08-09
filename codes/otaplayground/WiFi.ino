/*
 * Deals with WiFi connections. Can connect to 3 seperate connections defined in flash memory.
 */
#include <WiFi.h>
#include <WiFiClientSecure.h>

WiFiClient wifiClient;

bool wifiConnect (char* ssid, char* pass) { // attempt to connect to a wifi network
  int counter = 0;
  Serial.print("Attempting connection to ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    if (counter > 20) { // only attempt to connect for 10s.
      Serial.println("\nCould not connect.");
      return false;
    } else counter++;
  }
  Serial.println("\nConnected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  return true;
}

bool wifiGetConnection () { // just a nice function to wrap checking and getting connection to any of the 3 wifi networks
  if (WiFi.status() == WL_CONNECTED) return true; // if connected return true
  boolean connected = false;
  if (deviceInfo.ssidA && deviceInfo.passA) connected = wifiConnect(deviceInfo.ssidA, deviceInfo.passA);
  if (!connected && deviceInfo.ssidB && deviceInfo.passB) connected = wifiConnect(deviceInfo.ssidB, deviceInfo.passB);
  if (!connected && deviceInfo.ssidC && deviceInfo.passC) connected = wifiConnect(deviceInfo.ssidC, deviceInfo.passC);
  return connected; // will be true if it was able to connect
}

String ipString(){
  IPAddress ip = WiFi.localIP();
  String s="";
  for (int i=0; i<4; i++)
    s += i  ? "." + String(ip[i]) : String(ip[i]);
  return s;
}
