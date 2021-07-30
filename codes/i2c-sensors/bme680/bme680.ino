#include "bsec.h"
//https://github.com/BoschSensortec/BSEC-Arduino-library
//https://github.com/arendst/Tasmota/issues/5024
//https://github.com/trvrnrth/esphome-bsec-bme680
//
//MVK - note that it takes a while to calibrate Vo2 Co02 and IAQ (Indoor Airo Quaility)
// Helper functions declarations
void checkIaqSensorStatus(void);
void errLeds(void);

// Create an object of the class Bsec
Bsec iaqSensor;
#include <sys/time.h>

#define LOG(fmt, ...) (Serial.printf("%09llu: " fmt "\n", GetTimestamp(), ##__VA_ARGS__))

String output;

// Entry point for the example
void setup(void)
{
  Serial.begin(115200);
  Wire.begin();
  Serial.println(BME680_I2C_ADDR_PRIMARY);
  iaqSensor.begin(0x77, Wire); // 0x77 is the address for me bme680 ... see i2cscanner

  output = "\nBSEC library version " + String(iaqSensor.version.major) + "." + String(iaqSensor.version.minor) + "." + String(iaqSensor.version.major_bugfix) + "." + String(iaqSensor.version.minor_bugfix);
  Serial.println(output);
  checkIaqSensorStatus();

  bsec_virtual_sensor_t sensorList[10] = {
    BSEC_OUTPUT_RAW_TEMPERATURE,
    BSEC_OUTPUT_RAW_PRESSURE,
    BSEC_OUTPUT_RAW_HUMIDITY,
    BSEC_OUTPUT_RAW_GAS,
    BSEC_OUTPUT_IAQ,
    BSEC_OUTPUT_STATIC_IAQ,
    BSEC_OUTPUT_CO2_EQUIVALENT,
    BSEC_OUTPUT_BREATH_VOC_EQUIVALENT,
    BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_TEMPERATURE,
    BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_HUMIDITY,
  };

  iaqSensor.updateSubscription(sensorList, 10, BSEC_SAMPLE_RATE_LP);
  checkIaqSensorStatus();

  // Print the header
  output = "Timestamp [ms], raw temperature [°C], pressure [hPa], raw relative humidity [%], gas [Ohm], IAQ, IAQ accuracy, temperature [°C], relative humidity [%], Static IAQ, CO2 equivalent, breath VOC equivalent";
  Serial.println(output);
}
/*Explanation on static IAQ, breath VOC and CO2 equivalent
Below are some more details about the different outputs you listed, hopefully that will help you pick the best solution for your application:

Static IAQ:
The main difference between IAQ and static IAQ (sIAQ) relies in the scaling factor calculated based on the recent sensor history. The sIAQ output has been optimized for stationary applications (e.g. fixed indoor devices) whereas the IAQ output is ideal for mobile application (e.g. carry-on devices).
bVOCeq estimate:
The breath VOC equivalent output (bVOCeq) estimates the total VOC concentration [ppm] in the environment. It is calculated based on the sIAQ output and derived from lab tests.
CO2eq estimate:
Estimates a CO2-equivalent (CO2eq) concentration [ppm] in the environment. It is also calculated based on the sIAQ output and derived from VOC measurements and correlation from field studies.
Since bVOCeq and CO2eq are based on the sIAQ output, they are expected to perform optimally in stationnary applications where the main source of VOCs in the environment comes from human activity (e.g. in a bedroom).
*/
// Function that is looped forever
void loop(void)
{
  unsigned long time_trigger = millis();
  if (iaqSensor.run()) { // If new data is available
    output = String(time_trigger);
    output += ", " + String(iaqSensor.rawTemperature);
    output += ", " + String(iaqSensor.pressure);
    output += ", " + String(iaqSensor.rawHumidity);
    output += ", " + String(iaqSensor.gasResistance);
    output += ", " + String(iaqSensor.iaq);
    output += ", " + String(iaqSensor.iaqAccuracy);
    output += ", " + String(iaqSensor.temperature);
    output += ", " + String(iaqSensor.humidity);
    output += ", " + String(iaqSensor.staticIaq);
    output += ", " + String(iaqSensor.co2Equivalent);
    output += ", " + String(iaqSensor.breathVocEquivalent);
    Serial.println(output);
    LOG("Gas resistance %.2f kOhm", iaqSensor.gasResistance / 1000);
    LOG("Air Quality (Good<100>BAD) = ", iaqSensor.iaq);


  } else {
    checkIaqSensorStatus();
  }
}

int64_t GetTimestamp() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return (tv.tv_sec * 1000LL + (tv.tv_usec / 1000LL));
}


// Helper function definitions
void checkIaqSensorStatus(void)
{
  if (iaqSensor.status != BSEC_OK) {
    if (iaqSensor.status < BSEC_OK) {
      output = "BSEC error code : " + String(iaqSensor.status);
      Serial.println(output);
      for (;;)
        errLeds(); /* Halt in case of failure */
    } else {
      output = "BSEC warning code : " + String(iaqSensor.status);
      Serial.println(output);
    }
  }

  if (iaqSensor.bme680Status != BME680_OK) {
    if (iaqSensor.bme680Status < BME680_OK) {
      output = "BME680 error code : " + String(iaqSensor.bme680Status);
      Serial.println(output);
      for (;;)
        errLeds(); /* Halt in case of failure */
    } else {
      output = "BME680 warning code : " + String(iaqSensor.bme680Status);
      Serial.println(output);
    }
  }
}

void errLeds(void)
{
  Serial.println("Error");
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(100);
  digitalWrite(LED_BUILTIN, LOW);
  delay(100);
}
