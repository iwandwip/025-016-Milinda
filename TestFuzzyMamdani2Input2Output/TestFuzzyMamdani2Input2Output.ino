#define ENABLE_MODULE_FUZZY_MAMDANI
#define ENABLE_MODULE_FUZZY_SUGENO
#define ENABLE_MODULE_FUZZY_TSUKAMOTO

#include "Kinematrix.h"

// Konstanta PWM untuk kipas
#define FAN_PIN 16
#define PWM_CHANNEL 0
#define PWM_FREQUENCY 5000
#define PWM_RESOLUTION 8  // 8-bit resolusi (0-255)

// Instansiasi FuzzyMamdani dengan parameter:
// maxInputs, maxOutputs, maxRules, maxSetsPerVar
FuzzyMamdani fuzzy(2, 2, 10, 3);  // Memperbaiki jumlah maksimum rules dari 9 ke 10

// Fungsi setup fuzzy system
void setupFuzzySystem() {
  // Tambahkan variabel input
  fuzzy.addInputVariable("Suhu", 0.0f, 50.0f);         // Input 0: Suhu (0-50°C)
  fuzzy.addInputVariable("Kelembaban", 0.0f, 100.0f);  // Input 1: Kelembaban (0-100%)

  // Tambahkan variabel output
  fuzzy.addOutputVariable("KecepatanKipas", 0.0f, 100.0f);  // Output 0: Kecepatan Kipas (0-100%)
  fuzzy.addOutputVariable("SuhuAC", 16.0f, 30.0f);          // Output 1: Suhu AC (16-30°C)

  // Definisikan fuzzy set untuk Input Suhu (3 sets)
  float paramsDingin[] = { 0.0f, 0.0f, 25.0f };     // Triangular: dingin
  float paramsNormal[] = { 15.0f, 25.0f, 35.0f };   // Triangular: normal
  float paramsPanas[] = { 25.0f, 50.01f, 50.01f };  // Triangular: panas (Perbaikan untuk nilai batas)

  fuzzy.addFuzzySet(0, true, "Dingin", Fuzzy::TRIANGULAR, paramsDingin);  // Input 0, Set 0
  fuzzy.addFuzzySet(0, true, "Normal", Fuzzy::TRIANGULAR, paramsNormal);  // Input 0, Set 1
  fuzzy.addFuzzySet(0, true, "Panas", Fuzzy::TRIANGULAR, paramsPanas);    // Input 0, Set 2

  // Definisikan fuzzy set untuk Input Kelembaban (3 sets)
  float paramsKering[] = { 0.0f, 0.0f, 50.0f };       // Triangular: kering
  float paramsNormalH[] = { 30.0f, 50.0f, 70.0f };    // Triangular: normal
  float paramsBasah[] = { 50.0f, 100.01f, 100.01f };  // Triangular: basah (Perbaikan untuk nilai batas)

  fuzzy.addFuzzySet(1, true, "Kering", Fuzzy::TRIANGULAR, paramsKering);   // Input 1, Set 0
  fuzzy.addFuzzySet(1, true, "Normal", Fuzzy::TRIANGULAR, paramsNormalH);  // Input 1, Set 1
  fuzzy.addFuzzySet(1, true, "Basah", Fuzzy::TRIANGULAR, paramsBasah);     // Input 1, Set 2

  // Definisikan fuzzy set untuk Output Kecepatan Kipas (3 sets)
  float paramsLambat[] = { 0.0f, 0.0f, 50.0f };     // Triangular: lambat
  float paramsSedang[] = { 25.0f, 50.0f, 75.0f };   // Triangular: sedang
  float paramsCepat[] = { 50.0f, 100.0f, 100.0f };  // Triangular: cepat

  fuzzy.addFuzzySet(0, false, "Lambat", Fuzzy::TRIANGULAR, paramsLambat);  // Output 0, Set 0
  fuzzy.addFuzzySet(0, false, "Sedang", Fuzzy::TRIANGULAR, paramsSedang);  // Output 0, Set 1
  fuzzy.addFuzzySet(0, false, "Cepat", Fuzzy::TRIANGULAR, paramsCepat);    // Output 0, Set 2

  // Definisikan fuzzy set untuk Output Suhu AC (3 sets)
  float paramsDinginAC[] = { 16.0f, 16.0f, 23.0f };  // Triangular: dingin
  float paramsNormalAC[] = { 19.0f, 23.0f, 27.0f };  // Triangular: normal
  float paramsHangat[] = { 23.0f, 30.0f, 30.0f };    // Triangular: hangat

  fuzzy.addFuzzySet(1, false, "Dingin", Fuzzy::TRIANGULAR, paramsDinginAC);  // Output 1, Set 0
  fuzzy.addFuzzySet(1, false, "Normal", Fuzzy::TRIANGULAR, paramsNormalAC);  // Output 1, Set 1
  fuzzy.addFuzzySet(1, false, "Hangat", Fuzzy::TRIANGULAR, paramsHangat);    // Output 1, Set 2

  // Tambahkan aturan-aturan fuzzy
  // Format: antecedentVars, antecedentSets, numAntecedents, consequentVar, consequentSet, useAND

  // Rule 1: IF Suhu Dingin AND Kelembaban Kering THEN Kipas Lambat AND AC Normal
  int antecedentVars1[] = { 0, 1 };                                // Suhu, Kelembaban
  int antecedentSets1[] = { 0, 0 };                                // Dingin, Kering
  fuzzy.addRule(antecedentVars1, antecedentSets1, 2, 0, 0, true);  // Output 0 (Kipas): Lambat
  fuzzy.addRule(antecedentVars1, antecedentSets1, 2, 1, 1, true);  // Output 1 (AC): Normal

  // Rule 2: IF Suhu Dingin AND Kelembaban Normal THEN Kipas Lambat AND AC Normal
  int antecedentVars2[] = { 0, 1 };                                // Suhu, Kelembaban
  int antecedentSets2[] = { 0, 1 };                                // Dingin, Normal
  fuzzy.addRule(antecedentVars2, antecedentSets2, 2, 0, 0, true);  // Output 0 (Kipas): Lambat
  fuzzy.addRule(antecedentVars2, antecedentSets2, 2, 1, 1, true);  // Output 1 (AC): Normal

  // Rule 3: IF Suhu Dingin AND Kelembaban Basah THEN Kipas Sedang AND AC Dingin
  int antecedentVars3[] = { 0, 1 };                                // Suhu, Kelembaban
  int antecedentSets3[] = { 0, 2 };                                // Dingin, Basah
  fuzzy.addRule(antecedentVars3, antecedentSets3, 2, 0, 1, true);  // Output 0 (Kipas): Sedang
  fuzzy.addRule(antecedentVars3, antecedentSets3, 2, 1, 0, true);  // Output 1 (AC): Dingin

  // Rule 4: IF Suhu Normal AND Kelembaban Kering THEN Kipas Sedang AND AC Normal
  int antecedentVars4[] = { 0, 1 };                                // Suhu, Kelembaban
  int antecedentSets4[] = { 1, 0 };                                // Normal, Kering
  fuzzy.addRule(antecedentVars4, antecedentSets4, 2, 0, 1, true);  // Output 0 (Kipas): Sedang
  fuzzy.addRule(antecedentVars4, antecedentSets4, 2, 1, 1, true);  // Output 1 (AC): Normal

  // Rule 5: IF Suhu Normal AND Kelembaban Normal THEN Kipas Sedang AND AC Normal
  int antecedentVars5[] = { 0, 1 };                                // Suhu, Kelembaban
  int antecedentSets5[] = { 1, 1 };                                // Normal, Normal
  fuzzy.addRule(antecedentVars5, antecedentSets5, 2, 0, 1, true);  // Output 0 (Kipas): Sedang
  fuzzy.addRule(antecedentVars5, antecedentSets5, 2, 1, 1, true);  // Output 1 (AC): Normal

  // Set metode defuzzifikasi
  fuzzy.setDefuzzificationMethod(Fuzzy::CENTROID);

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

  Serial.println("Initializing Fuzzy Mamdani System...");
  setupFuzzySystem();

  Serial.println("\n=== Sistem Kendali Fuzzy Mamdani ===");
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

    // Tampilkan prompt lagi
    Serial.println("\n=== Sistem Kendali Fuzzy Mamdani ===");
    Serial.println("Format input: [suhu],[kelembaban]");
    Serial.println("Contoh: 25,60");

    // Bebas memori
    delete[] outputs;
  }
}