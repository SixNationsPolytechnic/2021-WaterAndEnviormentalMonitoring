#include <Arduino.h>
#include <Wire.h>
int Led = 13;//LED pin
int sensor = 14; //sensor pin
int val; //numeric variable

void setup()
{
	pinMode(Led, OUTPUT); //set LED pin as output
	pinMode(sensor, INPUT_PULLDOWN); //set sensor pin as input
  Serial.begin(115200);
      Serial.println("Magnet Testng!");
      delay(10000);
}
int i=0;

void loop()
{

	val = digitalRead(sensor); //Read the sensor
    Serial.println(val);
	if(val == LOW) //when magnetic field is detected, turn led on
	{
		digitalWrite(Led, HIGH);
        Serial.println("LED high");
	}
	else
	{
		digitalWrite(Led, LOW);
      Serial.println("LED low");
	}
      delay(1000);
      i++;
        Serial.println(i);

}
