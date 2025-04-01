/*
 * Contoh penggunaan FuzzySugeno pada ESP32 dengan 2 input dan 2 output
 * Input: Suhu (temperature) dan Kelembaban (humidity)
 * Output: Kecepatan kipas (fan speed) dan Suhu AC (AC temperature)
 * Semua output menggunakan tipe constant (bukan linear)
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
FuzzySugeno fuzzy(2, 2, 10, 3);

// Fungsi setup fuzzy system
void setupFuzzySystem() {
  // Tambahkan variabel input
  fuzzy.addInputVariable("Suhu", 0.0f, 50.0f);         // Input 0: Suhu (0-50°C)
  fuzzy.addInputVariable("Kelembaban", 0.0f, 100.0f);  // Input 1: Kelembaban (0-100%)

  // Definisikan fuzzy set untuk Input Suhu (3 sets)
  float paramsDingin[] = { 0.0f, 0.0f, 25.0f };    // Triangular: dingin
  float paramsNormal[] = { 15.0f, 25.0f, 35.0f };  // Triangular: normal
  float paramsPanas[] = { 25.0f, 50.0f, 50.0f };   // Triangular: panas

  fuzzy.addFuzzySet(0, "Dingin", Fuzzy::TRIANGULAR, paramsDingin);  // Input 0, Set 0
  fuzzy.addFuzzySet(0, "Normal", Fuzzy::TRIANGULAR, paramsNormal);  // Input 0, Set 1
  fuzzy.addFuzzySet(0, "Panas", Fuzzy::TRIANGULAR, paramsPanas);    // Input 0, Set 2

  // Definisikan fuzzy set untuk Input Kelembaban (3 sets)
  float paramsKering[] = { 0.0f, 0.0f, 50.0f };     // Triangular: kering
  float paramsNormalH[] = { 30.0f, 50.0f, 70.0f };  // Triangular: normal
  float paramsBasah[] = { 50.0f, 100.0f, 100.0f };  // Triangular: basah

  fuzzy.addFuzzySet(1, "Kering", Fuzzy::TRIANGULAR, paramsKering);   // Input 1, Set 0
  fuzzy.addFuzzySet(1, "Normal", Fuzzy::TRIANGULAR, paramsNormalH);  // Input 1, Set 1
  fuzzy.addFuzzySet(1, "Basah", Fuzzy::TRIANGULAR, paramsBasah);     // Input 1, Set 2

  // Definisikan output konstanta untuk Kecepatan Kipas (output 0)
  fuzzy.addConstantOutput("KipasLambat", 20.0f);  // Output 0, konstanta = 20%
  fuzzy.addConstantOutput("KipasSedang", 50.0f);  // Output 0, konstanta = 50%
  fuzzy.addConstantOutput("KipasCepat", 80.0f);   // Output 0, konstanta = 80%

  // Definisikan output konstanta untuk Suhu AC (output 1)
  fuzzy.addConstantOutput("ACDingin", 16.0f);  // Output 1, konstanta = 16°C
  fuzzy.addConstantOutput("ACNormal", 23.0f);  // Output 1, konstanta = 23°C
  fuzzy.addConstantOutput("ACHangat", 28.0f);  // Output 1, konstanta = 28°C

  // Tambahkan aturan-aturan fuzzy untuk output kipas dan AC
  // Format rule untuk Sugeno: antecedentVars, antecedentSets, numAntecedents, consequentOutput, useAND

  // Rule 1: IF Suhu Dingin AND Kelembaban Kering THEN Kipas Lambat, AC Normal
  int antecedentVars1[] = { 0, 1 };                             // Suhu, Kelembaban
  int antecedentSets1[] = { 0, 0 };                             // Dingin, Kering
  fuzzy.addRule(antecedentVars1, antecedentSets1, 2, 0, true);  // Output 0 (KipasLambat)
  fuzzy.addRule(antecedentVars1, antecedentSets1, 2, 4, true);  // Output 1 (ACNormal)

  // Rule 2: IF Suhu Dingin AND Kelembaban Normal THEN Kipas Lambat, AC Normal
  int antecedentVars2[] = { 0, 1 };                             // Suhu, Kelembaban
  int antecedentSets2[] = { 0, 1 };                             // Dingin, Normal
  fuzzy.addRule(antecedentVars2, antecedentSets2, 2, 0, true);  // Output 0 (KipasLambat)
  fuzzy.addRule(antecedentVars2, antecedentSets2, 2, 4, true);  // Output 1 (ACNormal)

  // Rule 3: IF Suhu Dingin AND Kelembaban Basah THEN Kipas Sedang, AC Dingin
  int antecedentVars3[] = { 0, 1 };                             // Suhu, Kelembaban
  int antecedentSets3[] = { 0, 2 };                             // Dingin, Basah
  fuzzy.addRule(antecedentVars3, antecedentSets3, 2, 1, true);  // Output 0 (KipasSedang)
  fuzzy.addRule(antecedentVars3, antecedentSets3, 2, 3, true);  // Output 1 (ACDingin)

  // Rule 4: IF Suhu Normal AND Kelembaban Kering THEN Kipas Sedang, AC Normal
  int antecedentVars4[] = { 0, 1 };                             // Suhu, Kelembaban
  int antecedentSets4[] = { 1, 0 };                             // Normal, Kering
  fuzzy.addRule(antecedentVars4, antecedentSets4, 2, 1, true);  // Output 0 (KipasSedang)
  fuzzy.addRule(antecedentVars4, antecedentSets4, 2, 4, true);  // Output 1 (ACNormal)

  // Rule 5: IF Suhu Normal AND Kelembaban Normal THEN Kipas Sedang, AC Normal
  int antecedentVars5[] = { 0, 1 };                             // Suhu, Kelembaban
  int antecedentSets5[] = { 1, 1 };                             // Normal, Normal
  fuzzy.addRule(antecedentVars5, antecedentSets5, 2, 1, true);  // Output 0 (KipasSedang)
  fuzzy.addRule(antecedentVars5, antecedentSets5, 2, 4, true);  // Output 1 (ACNormal)

  // Rule 6: IF Suhu Normal AND Kelembaban Basah THEN Kipas Cepat, AC Dingin
  int antecedentVars6[] = { 0, 1 };                             // Suhu, Kelembaban
  int antecedentSets6[] = { 1, 2 };                             // Normal, Basah
  fuzzy.addRule(antecedentVars6, antecedentSets6, 2, 2, true);  // Output 0 (KipasCepat)
  fuzzy.addRule(antecedentVars6, antecedentSets6, 2, 3, true);  // Output 1 (ACDingin)

  // Rule 7: IF Suhu Panas AND Kelembaban Kering THEN Kipas Cepat, AC Normal
  int antecedentVars7[] = { 0, 1 };                             // Suhu, Kelembaban
  int antecedentSets7[] = { 2, 0 };                             // Panas, Kering
  fuzzy.addRule(antecedentVars7, antecedentSets7, 2, 2, true);  // Output 0 (KipasCepat)
  fuzzy.addRule(antecedentVars7, antecedentSets7, 2, 4, true);  // Output 1 (ACNormal)

  // Rule 8: IF Suhu Panas AND Kelembaban Normal THEN Kipas Cepat, AC Dingin
  int antecedentVars8[] = { 0, 1 };                             // Suhu, Kelembaban
  int antecedentSets8[] = { 2, 1 };                             // Panas, Normal
  fuzzy.addRule(antecedentVars8, antecedentSets8, 2, 2, true);  // Output 0 (KipasCepat)
  fuzzy.addRule(antecedentVars8, antecedentSets8, 2, 3, true);  // Output 1 (ACDingin)

  // Rule 9: IF Suhu Panas AND Kelembaban Basah THEN Kipas Cepat, AC Dingin
  int antecedentVars9[] = { 0, 1 };                             // Suhu, Kelembaban
  int antecedentSets9[] = { 2, 2 };                             // Panas, Basah
  fuzzy.addRule(antecedentVars9, antecedentSets9, 2, 2, true);  // Output 0 (KipasCepat)
  fuzzy.addRule(antecedentVars9, antecedentSets9, 2, 3, true);  // Output 1 (ACDingin)

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

  Serial.println("\n=== Sistem Kendali Fuzzy Sugeno (Constant Output) ===");
  Serial.println("Format input: [suhu],[kelembaban]");
  Serial.println("Contoh: 25,60");
}

void loop() {
  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');
    input.trim();

    // Parse input format: suhu,kelembaban
    int commaIndex = input.indexOf(',');

    if (commaIndex == -1) {
      Serial.println("Error: Format input salah. Gunakan format: suhu,kelembaban");
      return;
    }

    float temperature = input.substring(0, commaIndex).toFloat();
    float humidity = input.substring(commaIndex + 1).toFloat();

    // Validasi input
    if (temperature < 0 || temperature > 50 || humidity < 0 || humidity > 100) {
      Serial.println("Error: Nilai di luar rentang. Suhu: 0-50, Kelembaban: 0-100");
      return;
    }

    Serial.print("Input: Suhu = ");
    Serial.print(temperature);
    Serial.print("°C, Kelembaban = ");
    Serial.print(humidity);
    Serial.println("%");

    // Evaluasi menggunakan fuzzy system
    float inputValues[] = { temperature, humidity };
    float *outputs = fuzzy.evaluate(inputValues);

    if (outputs == nullptr) {
      Serial.print("Error evaluating fuzzy system: ");
      Serial.println(fuzzy.getErrorMessage());
      return;
    }

    float fanSpeed = outputs[0];
    float acTemp = outputs[1];

    // Tampilkan hasil
    Serial.print("Output: Kecepatan Kipas = ");
    Serial.print(fanSpeed);
    Serial.print("%, Suhu AC = ");
    Serial.print(acTemp);
    Serial.println("°C");

    // Konversi ke nilai PWM dan kontrol kipas
    int pwmValue = map(fanSpeed, 0, 100, 0, 255);
    ledcWrite(PWM_CHANNEL, pwmValue);

    // Penjelasan tentang fuzzy Sugeno dengan constant output
    Serial.println("\nKeterangan tentang Fuzzy Sugeno dengan Constant Output:");
    Serial.println("- Setiap aturan fuzzy menghasilkan nilai konstanta tertentu");
    Serial.println("- Nilai akhir adalah rata-rata tertimbang dari nilai-nilai konstanta");
    Serial.println("- Bobot dihitung dari tingkat aktivasi tiap aturan");
    Serial.println("- Metode ini efisien secara komputasi dibanding Mamdani");

    // Kategori kecepatan kipas
    Serial.print("\nKategori kecepatan kipas: ");
    if (fanSpeed < 35) {
      Serial.println("Lambat");
    } else if (fanSpeed < 65) {
      Serial.println("Sedang");
    } else {
      Serial.println("Cepat");
    }

    // Kategori suhu AC
    Serial.print("Kategori suhu AC: ");
    if (acTemp < 20) {
      Serial.println("Dingin");
    } else if (acTemp < 25) {
      Serial.println("Normal");
    } else {
      Serial.println("Hangat");
    }

    // Tampilkan prompt lagi
    Serial.println("\n=== Sistem Kendali Fuzzy Sugeno (Constant Output) ===");
    Serial.println("Format input: [suhu],[kelembaban]");
    Serial.println("Contoh: 25,60");

    // Bebas memori
    delete[] outputs;
  }
}