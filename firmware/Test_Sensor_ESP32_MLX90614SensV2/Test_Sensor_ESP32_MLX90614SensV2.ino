#define ENABLE_SENSOR_MODULE_V2
#define ENABLE_INTERACTIVE_SERIAL_GENERAL_SENSOR_CALIBRATOR_V2
#define ENABLE_SENSOR_CALIBRATION_MODULE_V2

#define ENABLE_SENSOR_MLX90614_V2
#include "Kinematrix.h"

SensorCalibrationModuleV2 sensor;

void setup() {
  Serial.begin(115200);
  EEPROM.begin(512);

  sensor.setSerialOutput(&Serial);
  sensor.setEEPROMStartAddress(0);

  sensor.addSensor("temperature", new MLX90614SensV2);
  sensor.init();
  sensor.discoverSensorValues();
}

void loop() {
  sensor.update();
  sensor.debugAll();
}