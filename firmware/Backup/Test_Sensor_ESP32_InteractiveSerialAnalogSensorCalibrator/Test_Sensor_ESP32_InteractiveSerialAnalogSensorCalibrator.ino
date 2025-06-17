#include <Arduino.h>
#include "AnalogSensorCalibrator.h"
#include "InteractiveSerialAnalogSensorCalibrator.h"

// Define the analog pin for the sensor
#define SENSOR_PIN 32

// Create the interactive calibrator instance
// Parameters: (pin, referenceVoltage, adcRange)
// For ESP32: reference voltage 3.3V, adcRange will use default 4095.0
InteractiveSerialAnalogSensorCalibrator sensor(SENSOR_PIN, 3.3);

// Optional: Custom menu items
const char* customMenuOptions[] = {
  "1: Read Sensor Values",
  "2: Calibrate (One-Point)",
  "3: Calibrate (Two-Points)",
  "4: Calibrate (Multi-Point)",
  "5: Show Calibration Info",
  "6: Clear Calibration",
  "7: Load Profile",
  "8: Save Profile",
  "?: Show Menu"
};

void setup() {
  // Initialize serial communication
  Serial.begin(115200);

  // Wait for serial connection
  while (!Serial && millis() < 3000)
    ;

  Serial.println("\n===== Analog Sensor Interactive Calibrator Example =====");

  // Initialize the sensor with Serial interface and 15 second timeout
  sensor.begin(&Serial, 15000);

  // Optional: Customize display
  sensor.setDisplayUnits("pH");   // Set the display units
  sensor.setDisplayPrecision(2);  // Set precision to 2 decimal places

  // Optional: Set reading interval (milliseconds)
  sensor.setReadInterval(500);  // Read every 500ms when in continuous mode

  // Optional: Set custom menu prompt and options
  sensor.setMenuPrompt("\n===== pH SENSOR CALIBRATION =====");
  // Uncomment the next line to use custom menu items
  // sensor.setMenuOptions(customMenuOptions, 9);

  // Try to load saved preferences
  if (sensor.loadPreferences()) {
    Serial.println("User preferences loaded");
  }

  // Try to load calibration from profile 0
  if (sensor.loadCalibrationProfile(0)) {
    Serial.println("Default calibration profile loaded");
  } else {
    Serial.println("No valid calibration profile found - sensor uncalibrated");
  }
}

void loop() {
  // Update the sensor - this handles serial input and continuous readings
  sensor.update();

  // You can add additional code here that runs alongside the interactive
  // calibrator, for example:

  // Read the sensor periodically for your application needs
  static unsigned long lastAppReading = 0;
  if (millis() - lastAppReading > 2000) {  // Every 2 seconds
    // Your application code here
    // Example: Use the calibrated value in your application
    float value = sensor.readValue();

    // Don't print readings if we're in continuous reading mode
    // to avoid conflicting output
    if (!sensor.isCalibrated()) {
      // Maybe notify the user they should calibrate
      // Serial.println("Sensor not calibrated - please run calibration");
    }

    lastAppReading = millis();
  }
}