//
//    FILE: max44009_interrupt.ino
//  AUTHOR: Rob Tillaart
// VERSION: 0.1.0
// PURPOSE: demo of max44009 library
//    DATE: 2020-01-30
//
// Released to the public domain
//
//  breakout MAX44009 / GY-49
//
//      +--------+
//  VCC |o       |
//  GND |o       |
//  SCL |o      o| ADDRESS
//  SDA |o      o| -INT
//      +--------+
//
// ADDRESS:
// 0 = 0x4A
// 1 = 0x4B
//
// INT:
// Connect the INT pin to an pull up resistor
// 0 = interrupt
// 1 = no interrupt
// connect to an LED or an interrupt pin of an Arduino
//

#include "Wire.h"
#include "Max44009.h"

Max44009 myLux(0x4A);

uint32_t lastDisplay = 0;


void setup()
{
  Serial.begin(115200);
  Serial.print("\nStart max44009_interrupt : ");
  Serial.println(MAX44009_LIB_VERSION);

  Wire.begin();
  myLux.setContinuousMode();

  myLux.setHighThreshold(30);
  Serial.print("HighThreshold:\t");
  Serial.println(myLux.getHighThreshold());

  myLux.setLowThreshold(10);
  Serial.print("LowThreshold:\t");
  Serial.println(myLux.getLowThreshold());

  myLux.setThresholdTimer(3);

  myLux.enableInterrupt();
}


void loop()
{
  uint32_t interval = 1000;
  if (millis() - lastDisplay >= interval)
  {
    lastDisplay += interval;
    float lux = myLux.getLux();
    int err = myLux.getError();
    int st = myLux.getInterruptStatus();
    if (err != 0)
    {
      Serial.print("Error:\t");
      Serial.println(err);
    }
    else
    {
      Serial.print("lux:\t");
      Serial.print(lux);
      if (st == 1) Serial.println("\tIRQ occurred");
      else Serial.println();
    }
  }
}


// END OF FILE
