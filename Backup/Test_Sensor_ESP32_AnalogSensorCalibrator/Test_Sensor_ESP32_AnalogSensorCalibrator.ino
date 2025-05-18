#include <Arduino.h>
#include "AnalogSensorCalibrator.h"

// Define the analog pin for the sensor
#define SENSOR_PIN 32

// Create the sensor calibrator instance
AnalogSensorCalibrator sensor(SENSOR_PIN, 3.3, 4095.0);

// Global variables
unsigned long lastReadTime = 0;
const unsigned long READ_INTERVAL = 1000;  // Read every second

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 3000)
    ;  // Wait for serial connection

  Serial.println("\n===== Analog Sensor Calibrator Example =====");

  // Initialize the sensor (also handles EEPROM.begin for ESP platforms)
  sensor.begin();

  // Try to load existing calibration
  if (sensor.loadCalibration()) {
    Serial.println("Calibration loaded successfully!");
    sensor.printCalibrationDetails();
  } else {
    Serial.println("No valid calibration found");
  }

  // Print menu
  printMenu();
}

void loop() {
  // Periodically read and display sensor values if enabled
  if (millis() - lastReadTime > READ_INTERVAL) {
    // Uncomment the next line if you want automatic readings
    // readSensor();
    lastReadTime = millis();
  }

  // Check for user commands
  if (Serial.available() > 0) {
    char cmd = Serial.read();

    switch (cmd) {
      case '1':
        readSensor();
        printMenu();
        break;
      case '2':
        onePointCalibration();
        printMenu();
        break;
      case '3':
        twoPointCalibration();
        printMenu();
        break;
      case '4':
        multiPointCalibration();
        printMenu();
        break;
      case '5':
        sensor.printCalibrationDetails();
        printMenu();
        break;
      case '6':
        clearCalibration();
        printMenu();
        break;
      case '?':
        printMenu();
        break;
    }
  }
}

// Display menu options
void printMenu() {
  Serial.println("\n===== MENU =====");
  Serial.println("1: Read Sensor Value");
  Serial.println("2: One-Point Calibration");
  Serial.println("3: Two-Point Calibration");
  Serial.println("4: Multi-Point Calibration");
  Serial.println("5: Show Calibration Details");
  Serial.println("6: Clear Calibration");
  Serial.println("?: Show This Menu");
  Serial.println("================");
}

// Read and display sensor values continuously until user presses any key
void readSensor() {
  Serial.println("\n===== CONTINUOUS SENSOR READINGS =====");
  Serial.println("Press any key to stop...");
  delay(1000);  // Give user time to read the message

  // Clear any existing input in the buffer
  while (Serial.available()) {
    Serial.read();
  }

  while (!Serial.available()) {
    float raw = sensor.readRaw();
    float voltage = sensor.readVoltage();
    float value = sensor.readValue();

    // Display values on separate lines
    Serial.print("Raw ADC: ");
    Serial.print(raw);
    Serial.print(" | Voltage: ");
    Serial.print(voltage, 3);
    Serial.print("V | Calibrated: ");
    Serial.println(value, 3);

    delay(200);  // Update 5 times per second
  }

  // Clear the input buffer
  while (Serial.available()) {
    Serial.read();
  }

  Serial.println("\nSensor reading stopped");
}

// Clear calibration data
void clearCalibration() {
  Serial.println("\nClearing calibration data...");
  sensor.clearCalibrationPoints();
  Serial.println("Calibration data cleared");
}

// One-point calibration procedure
void onePointCalibration() {
  Serial.println("\n===== ONE-POINT CALIBRATION =====");
  Serial.println("This will calibrate the sensor using a single reference point");

  // Stabilize readings
  Serial.println("Stabilizing readings...");
  delay(1000);
  for (int i = 0; i < 10; i++) {
    sensor.readAverage();
    delay(100);
  }

  // Display current reading
  float voltage = sensor.readVoltage();
  Serial.print("Current voltage reading: ");
  Serial.print(voltage, 3);
  Serial.println("V");

  // Get known value from user
  Serial.print("Enter the known value for this voltage: ");

  // Clear any existing input in the buffer
  while (Serial.available()) {
    Serial.read();
  }

  while (!Serial.available()) {
    delay(10);
  }

  float knownValue = Serial.parseFloat();
  while (Serial.available()) { Serial.read(); }  // Clear buffer
  Serial.println(knownValue);

  // Perform calibration
  if (sensor.calibrateOnePoint(knownValue, voltage)) {
    Serial.println("Calibration successful!");
    sensor.saveCalibration();
    sensor.printCalibrationDetails();
  } else {
    Serial.println("Calibration failed. Check your input values.");
  }
}

// Two-point calibration procedure
void twoPointCalibration() {
  Serial.println("\n===== TWO-POINT CALIBRATION =====");
  Serial.println("This will calibrate using two reference points");

  // First point
  Serial.println("\nSTEP 1: Set up your first reference point");
  Serial.println("Press Enter when ready...");

  // Clear any existing input in the buffer
  while (Serial.available()) {
    Serial.read();
  }

  while (!Serial.available()) {
    delay(10);
  }
  while (Serial.available()) { Serial.read(); }

  delay(1000);
  for (int i = 0; i < 10; i++) {
    sensor.readAverage();
    delay(100);
  }

  float voltage1 = sensor.readVoltage();
  Serial.print("First point voltage: ");
  Serial.print(voltage1, 3);
  Serial.println("V");

  Serial.print("Enter the known value for this point: ");

  // Clear any existing input in the buffer
  while (Serial.available()) {
    Serial.read();
  }

  while (!Serial.available()) {
    delay(10);
  }
  float value1 = Serial.parseFloat();
  while (Serial.available()) { Serial.read(); }
  Serial.println(value1);

  // Second point
  Serial.println("\nSTEP 2: Set up your second reference point");
  Serial.println("Press Enter when ready...");

  // Clear any existing input in the buffer
  while (Serial.available()) {
    Serial.read();
  }

  while (!Serial.available()) {
    delay(10);
  }
  while (Serial.available()) { Serial.read(); }

  delay(1000);
  for (int i = 0; i < 10; i++) {
    sensor.readAverage();
    delay(100);
  }

  float voltage2 = sensor.readVoltage();
  Serial.print("Second point voltage: ");
  Serial.print(voltage2, 3);
  Serial.println("V");

  Serial.print("Enter the known value for this point: ");

  // Clear any existing input in the buffer
  while (Serial.available()) {
    Serial.read();
  }

  while (!Serial.available()) {
    delay(10);
  }
  float value2 = Serial.parseFloat();
  while (Serial.available()) { Serial.read(); }
  Serial.println(value2);

  // Perform calibration
  if (sensor.calibrateTwoPoint(value1, voltage1, value2, voltage2)) {
    Serial.println("Two-point calibration successful!");
    sensor.saveCalibration();
    sensor.printCalibrationDetails();
  } else {
    Serial.println("Calibration failed. The two points may be too close.");
  }
}

// Multi-point calibration procedure
void multiPointCalibration() {
  Serial.println("\n===== MULTI-POINT CALIBRATION =====");
  Serial.print("How many calibration points (2-10)? ");

  // Clear any existing input in the buffer
  while (Serial.available()) {
    Serial.read();
  }

  while (!Serial.available()) {
    delay(10);
  }
  int numPoints = Serial.parseInt();
  while (Serial.available()) { Serial.read(); }
  Serial.println(numPoints);

  if (numPoints < 2 || numPoints > 10) {
    Serial.println("Invalid number of points. Use 2-10 points.");
    return;
  }

  // Set max points and start calibration
  sensor.setMaxCalibrationPoints(numPoints);
  sensor.startCalibration(CALIBRATION_MULTI_POINT);

  // Collect calibration points
  for (int i = 0; i < numPoints; i++) {
    Serial.print("\nPoint ");
    Serial.print(i + 1);
    Serial.print("/");
    Serial.print(numPoints);
    Serial.println(":");

    Serial.println("Set up your reference point");
    Serial.println("Press Enter when ready...");

    // Clear any existing input in the buffer
    while (Serial.available()) {
      Serial.read();
    }

    while (!Serial.available()) {
      delay(10);
    }
    while (Serial.available()) { Serial.read(); }

    delay(1000);
    for (int j = 0; j < 10; j++) {
      sensor.readAverage();
      delay(100);
    }

    float voltage = sensor.readVoltage();
    Serial.print("Voltage: ");
    Serial.print(voltage, 3);
    Serial.println("V");

    Serial.print("Enter the known value for this point: ");

    // Clear any existing input in the buffer
    while (Serial.available()) {
      Serial.read();
    }

    while (!Serial.available()) {
      delay(10);
    }
    float value = Serial.parseFloat();
    while (Serial.available()) { Serial.read(); }
    Serial.println(value);

    sensor.addCalibrationPoint(value, voltage);
  }

  // Choose interpolation method
  Serial.println("\nSelect interpolation method:");
  Serial.println("1: Linear interpolation between points");
  Serial.println("2: Polynomial curve fitting");

  // Clear any existing input in the buffer
  while (Serial.available()) {
    Serial.read();
  }

  while (!Serial.available()) {
    delay(10);
  }
  int method = Serial.parseInt();
  while (Serial.available()) { Serial.read(); }
  Serial.println(method);

  if (method == 2) {
    sensor.setInterpolationMethod(1);  // Polynomial

    int maxDegree = min(numPoints - 1, 3);
    Serial.print("Enter polynomial degree (1-");
    Serial.print(maxDegree);
    Serial.println("):");

    // Clear any existing input in the buffer
    while (Serial.available()) {
      Serial.read();
    }

    while (!Serial.available()) {
      delay(10);
    }
    int degree = Serial.parseInt();
    while (Serial.available()) { Serial.read(); }
    Serial.println(degree);

    if (degree >= 1 && degree <= maxDegree) {
      sensor.setPolynomialDegree(degree);
    } else {
      Serial.println("Invalid degree, using degree 2");
      sensor.setPolynomialDegree(2);
    }
  } else {
    sensor.setInterpolationMethod(0);  // Linear
  }

  if (sensor.calculateCalibration()) {
    Serial.println("Multi-point calibration successful!");
    sensor.saveCalibration();
    sensor.printCalibrationDetails();
  } else {
    Serial.println("Calibration failed");
  }
}