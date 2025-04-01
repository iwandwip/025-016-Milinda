/*
 * Contoh sederhana penggunaan FuzzySugeno pada ESP32 dengan constant output
 * Input: Suhu (temperature)
 * Output: Kecepatan kipas (fan speed) - menggunakan constant output
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
FuzzySugeno fuzzy(1, 1, 3, 3);

// Fungsi setup fuzzy system
void setupFuzzySystem() {
  // Tambahkan variabel input suhu (rentang 0-50 derajat Celcius)
  fuzzy.addInputVariable("Suhu", 0.0f, 50.0f);

  // Definisikan fuzzy set untuk suhu (hanya 3 set untuk kesederhanaan)
  float paramsDingin[] = { 0.0f, 0.0f, 25.0f };    // Triangular: dingin
  float paramsNormal[] = { 15.0f, 25.0f, 35.0f };  // Triangular: normal
  float paramsPanas[] = { 25.0f, 50.0f, 50.0f };   // Triangular: panas

  fuzzy.addFuzzySet(0, "Dingin", Fuzzy::TRIANGULAR, paramsDingin);  // index 0
  fuzzy.addFuzzySet(0, "Normal", Fuzzy::TRIANGULAR, paramsNormal);  // index 1
  fuzzy.addFuzzySet(0, "Panas", Fuzzy::TRIANGULAR, paramsPanas);    // index 2

  // Definisikan constant output untuk kecepatan kipas
  fuzzy.addConstantOutput("KecepatanLambat", 20.0f);  // output 0
  fuzzy.addConstantOutput("KecepatanSedang", 50.0f);  // output 1
  fuzzy.addConstantOutput("KecepatanCepat", 80.0f);   // output 2

  // Tambahkan aturan-aturan fuzzy
  // IF Suhu is Dingin THEN KecepatanKipas = KecepatanLambat
  int antecedentVars1[] = { 0 };                                // Suhu (input variable 0)
  int antecedentSets1[] = { 0 };                                // Dingin (set 0)
  fuzzy.addRule(antecedentVars1, antecedentSets1, 1, 0, true);  // Output index 0 (KecepatanLambat)

  // IF Suhu is Normal THEN KecepatanKipas = KecepatanSedang
  int antecedentVars2[] = { 0 };                                // Suhu
  int antecedentSets2[] = { 1 };                                // Normal
  fuzzy.addRule(antecedentVars2, antecedentSets2, 1, 1, true);  // Output index 1 (KecepatanSedang)

  // IF Suhu is Panas THEN KecepatanKipas = KecepatanCepat
  int antecedentVars3[] = { 0 };                                // Suhu
  int antecedentSets3[] = { 2 };                                // Panas
  fuzzy.addRule(antecedentVars3, antecedentSets3, 1, 2, true);  // Output index 2 (KecepatanCepat)

  // Aktifkan debug mode untuk melihat detail proses
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

  Serial.println("Initializing Simple Fuzzy Sugeno System...");
  setupFuzzySystem();

  Serial.println("\n=== Sistem Fuzzy Sugeno Sederhana (Constant Output) ===");
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

    // Tampilkan informasi detail
    Serial.println("\nProses Fuzzy Sugeno dengan Constant Output:");

    // Tingkat keanggotaan input untuk setiap set (simulasi untuk penjelasan)
    float dinginDegree = (temperature < 25) ? (25 - temperature) / 25 : 0;
    float normalDegree = 0;
    if (temperature >= 15 && temperature < 25) {
      normalDegree = (temperature - 15) / 10;
    } else if (temperature >= 25 && temperature < 35) {
      normalDegree = (35 - temperature) / 10;
    }
    float panasDegree = (temperature > 25) ? (temperature - 25) / 25 : 0;
    if (panasDegree > 1) panasDegree = 1;

    Serial.println("1. Fuzzifikasi Input:");
    Serial.print("   - Tingkat keanggotaan Dingin: ");
    Serial.println(dinginDegree);
    Serial.print("   - Tingkat keanggotaan Normal: ");
    Serial.println(normalDegree);
    Serial.print("   - Tingkat keanggotaan Panas: ");
    Serial.println(panasDegree);

    Serial.println("\n2. Evaluasi Aturan:");
    Serial.print("   - Aturan 1 (Dingin): memberikan nilai konstanta 20% dengan bobot ");
    Serial.println(dinginDegree);
    Serial.print("   - Aturan 2 (Normal): memberikan nilai konstanta 50% dengan bobot ");
    Serial.println(normalDegree);
    Serial.print("   - Aturan 3 (Panas): memberikan nilai konstanta 80% dengan bobot ");
    Serial.println(panasDegree);

    Serial.println("\n3. Aggregasi dan Defuzzifikasi:");
    Serial.print("   - Hasil akhir adalah rata-rata tertimbang: ");
    Serial.print(fanSpeed);
    Serial.println("%");

    // Konversi persentase kecepatan kipas ke nilai PWM (0-255)
    int pwmValue = map(fanSpeed, 0, 100, 0, 255);

    // Terapkan ke kipas
    ledcWrite(PWM_CHANNEL, pwmValue);

    Serial.print("\nNilai PWM kipas: ");
    Serial.println(pwmValue);

    // Perbedaan dengan Mamdani
    Serial.println("\nPerbedaan dengan Fuzzy Mamdani:");
    Serial.println("- Sugeno menggunakan nilai konstanta untuk output (bukan himpunan fuzzy)");
    Serial.println("- Hasil akhir dihitung dengan weighted average (rata-rata tertimbang)");
    Serial.println("- Tidak memerlukan proses defuzzifikasi yang kompleks seperti centroid");
    Serial.println("- Lebih efisien secara komputasi, cocok untuk sistem embedded");

    // Tampilkan menu kembali
    Serial.println("\n=== Sistem Fuzzy Sugeno Sederhana (Constant Output) ===");
    Serial.println("Masukkan nilai suhu (0-50) untuk pengujian:");

    // Bebas memori
    delete[] outputs;
  }
}