#define ENABLE_MODULE_FUZZY_MAMDANI
#define ENABLE_MODULE_FUZZY_SUGENO
#define ENABLE_MODULE_FUZZY_TSUKAMOTO

#include "Kinematrix.h"

#define FAN_PWM_PIN 26  // PWM pin for fan control

// Fuzzy system settings
#define MAX_INPUTS 1
#define MAX_OUTPUTS 1
#define MAX_RULES 3
#define MAX_SETS_PER_VAR 3

// PWM settings
#define PWM_CHANNEL 0
#define PWM_RESOLUTION 8     // 8-bit resolution (0-255)
#define PWM_FREQUENCY 25000  // 25 KHz

// Global variables
FuzzySugeno fuzzySystem(MAX_INPUTS, MAX_OUTPUTS, MAX_RULES, MAX_SETS_PER_VAR);
float lastTemperature = 25.0;  // Default value

void setupFuzzySystem() {
  // Add the temperature input variable (range: 10-40°C)
  fuzzySystem.addInputVariable("temperature", 10.0, 40.0);

  // Define temperature fuzzy sets
  // Cold temperature (triangular: 10, 15, 20)
  float coldParams[] = { 10.0, 15.0, 20.0 };
  fuzzySystem.addFuzzySet(0, "cold", Fuzzy::TRIANGULAR, coldParams);

  // Comfortable temperature (triangular: 15, 25, 35)
  float comfortableParams[] = { 15.0, 25.0, 35.0 };
  fuzzySystem.addFuzzySet(0, "comfortable", Fuzzy::TRIANGULAR, comfortableParams);

  // Hot temperature (triangular: 30, 35, 40)
  float hotParams[] = { 30.0, 35.0, 40.0 };
  fuzzySystem.addFuzzySet(0, "hot", Fuzzy::TRIANGULAR, hotParams);

  // Define fan speed outputs (Sugeno)
  // Low fan speed (constant: 50)
  fuzzySystem.addConstantOutput("lowSpeed", 50.0);

  // Medium fan speed (linear: 100 + 4*temp)
  // This will give values around 100-200 in the comfortable range
  float mediumCoeffs[] = { 100.0, 4.0 };  // 100 + 4*temp
  fuzzySystem.addLinearOutput("mediumSpeed", mediumCoeffs, 2);

  // High fan speed (constant: 250)
  fuzzySystem.addConstantOutput("highSpeed", 250.0);

  // Define rules
  // Rule 1: If temperature is cold, then fan speed is low
  int r1_vars[] = { 0 };                              // Input variable index (temperature)
  int r1_sets[] = { 0 };                              // Set index (cold)
  fuzzySystem.addRule(r1_vars, r1_sets, 1, 0, true);  // Output 0 = lowSpeed

  // Rule 2: If temperature is comfortable, then fan speed is medium
  int r2_vars[] = { 0 };                              // Input variable index (temperature)
  int r2_sets[] = { 1 };                              // Set index (comfortable)
  fuzzySystem.addRule(r2_vars, r2_sets, 1, 1, true);  // Output 1 = mediumSpeed

  // Rule 3: If temperature is hot, then fan speed is high
  int r3_vars[] = { 0 };                              // Input variable index (temperature)
  int r3_sets[] = { 2 };                              // Set index (hot)
  fuzzySystem.addRule(r3_vars, r3_sets, 1, 2, true);  // Output 2 = highSpeed

  // Check for errors in the fuzzy system setup
  if (fuzzySystem.hasError()) {
    Serial.print("Fuzzy system setup error: ");
    Serial.println(fuzzySystem.getErrorMessage());
  } else {
    Serial.println("Fuzzy system setup complete!");
  }
}

void setup() {
  // Initialize serial
  Serial.begin(115200);
  while (!Serial) {
    ;  // Wait for serial port to connect
  }

  // Setup PWM for fan control
  ledcSetup(PWM_CHANNEL, PWM_FREQUENCY, PWM_RESOLUTION);
  ledcAttachPin(FAN_PWM_PIN, PWM_CHANNEL);

  // Initialize fuzzy system
  setupFuzzySystem();

  // Print instructions
  Serial.println();
  Serial.println("ESP32 Fuzzy Fan Control");
  Serial.println("----------------------");
  Serial.println("Enter temperature value (10-40°C):");
}

void loop() {
  // Check if data is available from Serial
  if (Serial.available() > 0) {
    // Read the temperature from serial
    String input = Serial.readStringUntil('\n');
    input.trim();

    // Convert to float
    float temperature = input.toFloat();

    // Validate input
    if (temperature < 10.0 || temperature > 40.0) {
      Serial.println("Temperature must be between 10°C and 40°C");
    } else {
      // Update stored temperature
      lastTemperature = temperature;
      Serial.print("Temperature set to: ");
      Serial.print(temperature);
      Serial.println("°C");

      // Evaluate fuzzy system
      float inputValues[] = { temperature };
      float *outputs = fuzzySystem.evaluate(inputValues);

      if (outputs == nullptr) {
        Serial.print("Evaluation error: ");
        Serial.println(fuzzySystem.getErrorMessage());
      } else {
        // Get fan speed (result will be a weighted average of the three outputs)
        float fanSpeed = outputs[0];

        // Apply fan speed via PWM
        int pwmValue = constrain(round(fanSpeed), 0, 255);
        ledcWrite(PWM_CHANNEL, pwmValue);

        // Report results
        Serial.print("Fan speed: ");
        Serial.print(fanSpeed);
        Serial.print(" (PWM: ");
        Serial.print(pwmValue);
        Serial.println(")");

        // Calculate and show the contribution of each rule
        float inputs[] = { temperature };
        Serial.println("\nRule contributions:");
        if (temperature <= 20.0) {
          Serial.print("Cold rule: Strong contribution (");
          Serial.print(calculateTriangularMembership(temperature, 10.0, 15.0, 20.0) * 100);
          Serial.println("%)");
        } else {
          Serial.println("Cold rule: Minimal contribution");
        }

        if (temperature >= 15.0 && temperature <= 35.0) {
          Serial.print("Comfortable rule: Strong contribution (");
          Serial.print(calculateTriangularMembership(temperature, 15.0, 25.0, 35.0) * 100);
          Serial.println("%)");
        } else {
          Serial.println("Comfortable rule: Minimal contribution");
        }

        if (temperature >= 30.0) {
          Serial.print("Hot rule: Strong contribution (");
          Serial.print(calculateTriangularMembership(temperature, 30.0, 35.0, 40.0) * 100);
          Serial.println("%)");
        } else {
          Serial.println("Hot rule: Minimal contribution");
        }

        // Free the allocated memory
        delete[] outputs;
      }
    }

    Serial.println("\nEnter new temperature (10-40°C):");
  }

  // Small delay to prevent hogging the CPU
  delay(100);
}

// Helper function to calculate triangular membership (for displaying rule contributions)
float calculateTriangularMembership(float value, float a, float b, float c) {
  if (value <= a || value >= c) {
    return 0.0;
  } else if (value < b) {
    return (value - a) / (b - a);
  } else {
    return (c - value) / (c - b);
  }
}