/*
 * Contoh penggunaan FuzzySugeno pada ESP32 (Versi yang sudah diperbaiki)
 * Input: Suhu (temperature)
 * Output: Kecepatan kipas (fan speed)
 */

#define ENABLE_MODULE_FUZZY_MAMDANI
#define ENABLE_MODULE_FUZZY_SUGENO
#define ENABLE_MODULE_FUZZY_TSUKAMOTO

#include "Kinematrix.h"

// Pin PWM untuk kontrol kipas
#define FAN_PIN 16

// Konstanta PWM
#define PWM_CHANNEL 0
#define PWM_FREQUENCY 5000
#define PWM_RESOLUTION 8  // 8-bit resolusi (0-255)

// Instansiasi FuzzySugeno dengan parameter:
// maxInputs, maxOutputs, maxRules, maxSetsPerVar
FuzzySugeno fuzzy(1, 1, 5, 5);

// Fungsi setup fuzzy system
void setupFuzzySystem() {
  // Tambahkan variabel input suhu (rentang 0-50 derajat Celcius)
  fuzzy.addInputVariable("Suhu", 0.0f, 50.0f);

  // Tambahkan variabel output kecepatan kipas (rentang 0-100%)
  fuzzy.addOutputVariable("KecepatanKipas", 0.0f, 100.0f);

  // Definisikan fuzzy set untuk suhu (input)
  float paramsUdingin[] = { 0.0f, 0.0f, 15.0f, 20.0f };   // Trapezoidal: sangat dingin
  float paramsDingin[] = { 15.0f, 20.0f, 25.0f };         // Triangular: dingin
  float paramsNormal[] = { 20.0f, 25.0f, 30.0f };         // Triangular: normal
  float paramsPanas[] = { 25.0f, 30.0f, 35.0f };          // Triangular: panas
  float paramsUPanas[] = { 30.0f, 35.0f, 50.0f, 50.0f };  // Trapezoidal: sangat panas

  fuzzy.addFuzzySet(0, true, "UDingin", Fuzzy::TRAPEZOIDAL, paramsUdingin);  // index 0
  fuzzy.addFuzzySet(0, true, "Dingin", Fuzzy::TRIANGULAR, paramsDingin);     // index 1
  fuzzy.addFuzzySet(0, true, "Normal", Fuzzy::TRIANGULAR, paramsNormal);     // index 2
  fuzzy.addFuzzySet(0, true, "Panas", Fuzzy::TRIANGULAR, paramsPanas);       // index 3
  fuzzy.addFuzzySet(0, true, "UPanas", Fuzzy::TRAPEZOIDAL, paramsUPanas);    // index 4

  // Definisikan output konstanta untuk kecepatan kipas (output variabel 0)
  fuzzy.addConstantOutput(0, "Mati", 0.0f);           // index 0
  fuzzy.addConstantOutput(0, "SangatLambat", 20.0f);  // index 1
  fuzzy.addConstantOutput(0, "Lambat", 40.0f);        // index 2
  fuzzy.addConstantOutput(0, "Cepat", 60.0f);         // index 3
  fuzzy.addConstantOutput(0, "SangatCepat", 90.0f);   // index 4

  // Tambahkan aturan-aturan fuzzy
  // IF Suhu is UDingin THEN KecepatanKipas = Mati
  int antecedentVars1[] = { 0 };                                   // Suhu (input variable 0)
  int antecedentSets1[] = { 0 };                                   // UDingin (set 0)
  fuzzy.addRule(antecedentVars1, antecedentSets1, 1, 0, 0, true);  // Output variabel 0, set 0 (Mati)

  // IF Suhu is Dingin THEN KecepatanKipas = SangatLambat
  int antecedentVars2[] = { 0 };                                   // Suhu
  int antecedentSets2[] = { 1 };                                   // Dingin
  fuzzy.addRule(antecedentVars2, antecedentSets2, 1, 0, 1, true);  // Output variabel 0, set 1 (SangatLambat)

  // IF Suhu is Normal THEN KecepatanKipas = Lambat
  int antecedentVars3[] = { 0 };                                   // Suhu
  int antecedentSets3[] = { 2 };                                   // Normal
  fuzzy.addRule(antecedentVars3, antecedentSets3, 1, 0, 2, true);  // Output variabel 0, set 2 (Lambat)

  // IF Suhu is Panas THEN KecepatanKipas = Cepat
  int antecedentVars4[] = { 0 };                                   // Suhu
  int antecedentSets4[] = { 3 };                                   // Panas
  fuzzy.addRule(antecedentVars4, antecedentSets4, 1, 0, 3, true);  // Output variabel 0, set 3 (Cepat)

  // IF Suhu is UPanas THEN KecepatanKipas = SangatCepat
  int antecedentVars5[] = { 0 };                                   // Suhu
  int antecedentSets5[] = { 4 };                                   // UPanas
  fuzzy.addRule(antecedentVars5, antecedentSets5, 1, 0, 4, true);  // Output variabel 0, set 4 (SangatCepat)

  // Aktifkan debug mode jika diperlukan
  fuzzy.setDebugMode(true);

  if (fuzzy.hasError()) {
    Serial.print("Error setting up fuzzy system: ");
    Serial.println(fuzzy.getErrorMessage());
  } else {
    Serial.println("Fuzzy system setup successful");
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  // Setup PWM untuk kipas
  ledcSetup(PWM_CHANNEL, PWM_FREQUENCY, PWM_RESOLUTION);
  ledcAttachPin(FAN_PIN, PWM_CHANNEL);

  Serial.println("Initializing Fuzzy Sugeno System...");
  setupFuzzySystem();

  Serial.println("\n=== Sistem Fuzzy Sugeno Suhu-Kipas ===");
  Serial.println("Masukkan nilai suhu (0-50) untuk pengujian:");
}

void loop() {
  // Baca input suhu dari Serial Monitor
  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');
    input.trim();

    float temperature = input.toFloat();

    // Validasi input
    if (temperature < 0 || temperature > 50) {
      Serial.println("Error: Suhu harus berada dalam rentang 0-50 derajat Celcius");
      return;
    }

    Serial.print("Suhu input: ");
    Serial.print(temperature);
    Serial.println(" °C");

    // Evaluasi menggunakan fuzzy system
    float inputValues[] = { temperature };
    float *outputs = fuzzy.evaluate(inputValues);

    if (outputs == nullptr) {
      Serial.print("Error evaluating fuzzy system: ");
      Serial.println(fuzzy.getErrorMessage());
      return;
    }

    float fanSpeed = outputs[0];

    Serial.print("Kecepatan kipas (hasil fuzzy): ");
    Serial.print(fanSpeed);
    Serial.println("%");

    // Konversi persentase kecepatan kipas ke nilai PWM (0-255)
    int pwmValue = map(fanSpeed, 0, 100, 0, 255);

    // Terapkan ke kipas
    ledcWrite(PWM_CHANNEL, pwmValue);

    Serial.print("Nilai PWM kipas: ");
    Serial.println(pwmValue);

    // Klasifikasi linguistik kecepatan kipas
    String speedCategory;
    if (fanSpeed < 10) speedCategory = "Mati";
    else if (fanSpeed < 30) speedCategory = "Sangat Lambat";
    else if (fanSpeed < 50) speedCategory = "Lambat";
    else if (fanSpeed < 70) speedCategory = "Cepat";
    else speedCategory = "Sangat Cepat";

    Serial.print("Kategori kecepatan: ");
    Serial.println(speedCategory);

    // Tampilkan menu kembali
    Serial.println("\n=== Sistem Fuzzy Sugeno Suhu-Kipas ===");
    Serial.println("Masukkan nilai suhu (0-50) untuk pengujian:");

    // Bebas memori
    delete[] outputs;
  }
}