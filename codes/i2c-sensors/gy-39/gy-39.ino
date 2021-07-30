//its a combo sensor
#include <Adafruit_BME280.h>
#include <MAX44009.h> ///https://github.com/ml2068/GY39

#define SEALEVELPRESSURE_HPA (1013.25)//< Average sea level pressure is 1013.25 hPa

Adafruit_BME280 bme; //  I2C Define BME280
//MAX44009 sensor;    //   I2C Define MAX44009
#include "Max44009.h"

Max44009 sensor(0x4A);


unsigned long delayTime=3000;

void setup() {
  Serial.begin(115200);
  Serial.println(F("BME280 test"));
  bool status;
  status = bme.begin(0x76);
  if (!status) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
  }


  Serial.begin(115200);
  Serial.print("\nStart max44009_interrupt : ");
  Serial.println(MAX44009_LIB_VERSION);

  Wire.begin();
  sensor.setContinuousMode();

  sensor.setHighThreshold(30);
  Serial.print("HighThreshold:\t");
  Serial.println(sensor.getHighThreshold());

  sensor.setLowThreshold(10);
  Serial.print("LowThreshold:\t");
  Serial.println(sensor.getLowThreshold());

  sensor.setThresholdTimer(3);

  sensor.enableInterrupt();

}

void loop() {
 // Output data to serial monitor
  Serial.print("Ambient Light luminance :");
  Serial.print(sensor.getLux());//
  Serial.println(" lux");

  Serial.print("Temperature = ");
  Serial.print(bme.readTemperature());
  Serial.println(" *C");

  // Convert temperature to Fahrenheit
  /*Serial.print("Temperature = ");
  Serial.print(1.8 * bme.readTemperature() + 32);
  Serial.println(" *F");*/

  Serial.print("Pressure = ");
  Serial.print(bme.readPressure() / 100.0F);
  Serial.println(" hPa");

  Serial.print("Approx. Altitude = ");
  Serial.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
  Serial.println(" m");

  Serial.print("Humidity = ");
  Serial.print(bme.readHumidity());
  Serial.println(" %");
  Serial.println();


  float lux = sensor.getLux();
  int err = sensor.getError();
  int st = sensor.getInterruptStatus();
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

  delay(delayTime);
}
