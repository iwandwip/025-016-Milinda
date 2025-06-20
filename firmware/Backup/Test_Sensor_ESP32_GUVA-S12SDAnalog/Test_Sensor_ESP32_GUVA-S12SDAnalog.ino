#define ENABLE_SENSOR_MODULE
#define ENABLE_SENSOR_MODULE_UTILITY
#define ENABLE_SENSOR_ANALOG

#include "Kinematrix.h"

SensorModule sensor;

void setup() {
  Serial.begin(115200);
  sensor.addModule("uv", new AnalogSens(39, 3.3, 4096.0));
  sensor.init();
}

void loop() {
  sensor.update([]() {
    sensor.debug(500, false);
  });
}
