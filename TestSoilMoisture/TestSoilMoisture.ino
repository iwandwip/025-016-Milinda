#define ENABLE_SENSOR_MODULE
#define ENABLE_SENSOR_MODULE_UTILITY
#define ENABLE_SENSOR_MLX90614

#include "Kinematrix.h"

SensorModule sensor;

void setup() {
  Serial.begin(115200);
  sensor.addModule("mlx", new MLX90614Sens);
  sensor.init();
}

void loop() {
  sensor.update([]() {
    sensor.debug();
  });
}
