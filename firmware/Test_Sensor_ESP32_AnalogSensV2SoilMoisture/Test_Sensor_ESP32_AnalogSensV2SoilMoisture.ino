#define ENABLE_SENSOR_MODULE_V2
#define ENABLE_INTERACTIVE_SERIAL_GENERAL_SENSOR_CALIBRATOR_V2
#define ENABLE_SENSOR_CALIBRATION_MODULE_V2
#define ENABLE_SENSOR_FILTER_V2
#define ENABLE_SENSOR_UTILITY_V2
#define ENABLE_SENSOR_ALERT_SYSTEM_V2

#define ENABLE_SENSOR_ANALOG_V2
#include "Kinematrix.h"

SensorCalibrationModuleV2 sensor;

void setup() {
  Serial.begin(115200);
  EEPROM.begin(512);

  sensor.setSerialOutput(&Serial);
  sensor.setEEPROMStartAddress(0);

  sensor.addSensor("soil", []() -> BaseSensV2* {
    auto sens = new AnalogSensV2(36, 3.3, 4095.0, [](BaseSensV2* s, int analogValue, float voltageValue) {
      float moisture = voltageValue * 1.5;
      s->updateValue("moist", moisture);
    });
    sens->addCustomValue("moist", "Moisture", "%", 2, TYPE_FLOAT);
    sens->setUpdateInterval(100);
    return sens;
  });
  sensor.init();
  sensor.discoverSensorValues();
}

void loop() {
  sensor.update();
  // sensor.debugAll();
}