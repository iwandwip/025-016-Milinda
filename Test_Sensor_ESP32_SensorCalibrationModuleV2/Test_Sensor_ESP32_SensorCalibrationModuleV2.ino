#define ENABLE_SENSOR_MODULE_V2
#define ENABLE_INTERACTIVE_SERIAL_GENERAL_SENSOR_CALIBRATOR_V2
#define ENABLE_SENSOR_CALIBRATION_MODULE_V2

#define ENABLE_SENSOR_ANALOG_V2
#include "Kinematrix.h"

#define SENSOR_PIN_1 36
#define SENSOR_PIN_2 39
#define SENSOR_PIN_3 35

#define ADC_VREF 3.3
#define ADC_RANGE 4096

#define SENSOR_UPDATE_INTERVAL 100

#define EEPROM_SIZE 512
#define EEPROM_START_ADDRESS 0

SensorCalibrationModuleV2 sensorModule;

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("\nESP32 Sensor Calibration Example with Lambda Functions");

#ifdef ESP32
  EEPROM.begin(EEPROM_SIZE);
#endif

  sensorModule.setSerialOutput(&Serial);
  sensorModule.setEEPROMStartAddress(EEPROM_START_ADDRESS);

  sensorModule.addSensor("temperature", []() -> BaseSensV2* {
    auto* sensor = new AnalogSensV2(SENSOR_PIN_1, ADC_VREF, ADC_RANGE, [](BaseSensV2* s, int analogValue, float voltageValue) {
      float temperature = voltageValue * (100.0 / 3.3);
      s->updateValue("temp", temperature);
    });
    sensor->addCustomValue("temp", "Temperature", "°C", 1, TYPE_FLOAT);
    sensor->setUpdateInterval(SENSOR_UPDATE_INTERVAL);
    return sensor;
  });

  sensorModule.addSensor("pressure", []() -> BaseSensV2* {
    auto* sensor = new AnalogSensV2(SENSOR_PIN_2, ADC_VREF, ADC_RANGE, [](BaseSensV2* s, int analogValue, float voltageValue) {
      float pressure = voltageValue * (100.0 / 3.3);
      s->updateValue("pressure", pressure);
    });
    sensor->addCustomValue("pressure", "Pressure", "kPa", 2, TYPE_FLOAT);
    sensor->setUpdateInterval(SENSOR_UPDATE_INTERVAL);
    return sensor;
  });

  sensorModule.addSensor("light", []() -> BaseSensV2* {
    auto* sensor = new AnalogSensV2(SENSOR_PIN_3, ADC_VREF, ADC_RANGE, [](BaseSensV2* s, int analogValue, float voltageValue) {
      float light = voltageValue * (1000.0 / 3.3);
      s->updateValue("light", light);
    });
    sensor->addCustomValue("light", "Light", "lux", 0, TYPE_FLOAT);
    sensor->setUpdateInterval(SENSOR_UPDATE_INTERVAL);
    return sensor;
  });

  sensorModule.init();
  sensorModule.discoverSensorValues();

  // CalibrationLoadResult result = sensorModule.loadAllCalibrationsWithStatus();
  // Serial.println("\n===== CALIBRATION STATUS AT STARTUP =====");
  // Serial.print("Total sensors: ");
  // Serial.println(result.totalEntries);
  // Serial.print("Successfully loaded: ");
  // Serial.println(result.successCount);

  // if (result.successCount > 0) {
  //   sensorModule.printCalibrationStatus();
  // }

  Serial.println("Sensors initialized. Available commands:");
  Serial.println("'c': Enter calibration mode");
  Serial.println("'r': Read all sensor values");
  Serial.println("'s': Save all calibrations to EEPROM");
  Serial.println("'l': Load all calibrations from EEPROM");
  Serial.println();
}

void loop() {
  sensorModule.update();

  if (Serial.available() > 0) {
    char cmd = Serial.read();

    switch (cmd) {
      case 'c':
      case 'C':
        sensorModule.startCalibrationMode(300000);
        break;

      case 'r':
      case 'R':
        Serial.println("\n===== SENSOR READINGS =====");
        sensorModule.debugAll(true);
        break;

      case 's':
      case 'S':
        if (sensorModule.saveAllCalibrations()) {
          Serial.println("All calibrations saved successfully");
        } else {
          Serial.println("Error saving calibrations");
        }
        break;

      case 'l':
      case 'L':
        {
          CalibrationLoadResult result = sensorModule.loadAllCalibrationsWithStatus();
          Serial.println("\n===== LOADING CALIBRATIONS =====");
          Serial.print("Total sensors: ");
          Serial.println(result.totalEntries);

          if (result.successCount > 0) {
            Serial.print("Successfully loaded: ");
            Serial.println(result.successCount);
          }

          if (result.notCalibratedCount > 0) {
            Serial.print("Not yet calibrated: ");
            Serial.println(result.notCalibratedCount);
          }

          if (result.errorCount > 0) {
            Serial.print("Error loading: ");
            Serial.println(result.errorCount);
          }

          sensorModule.printCalibrationStatus();
          break;
        }
    }
  }

  delay(10);
}