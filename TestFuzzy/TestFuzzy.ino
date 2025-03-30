/*
 * FuzzyComparison_Fan_Control.ino
 * 
 * Perbandingan kontrol kecepatan kipas menggunakan tiga metode fuzzy:
 * Mamdani, Sugeno, dan Tsukamoto
 * 
 * Input: Suhu dan Kelembapan
 * Output: Kecepatan Kipas
 */

#define ENABLE_MODULE_FUZZY_MAMDANI
#define ENABLE_MODULE_FUZZY_SUGENO
#define ENABLE_MODULE_FUZZY_TSUKAMOTO

#include "Kinematrix.h"

// Membuat instance untuk ketiga jenis fuzzy
FuzzyMamdani fuzzyMamdani(2, 1, 9, 3);      // 2 input, 1 output, 9 rules, 3 sets per var
FuzzySugeno fuzzySugeno(2, 1, 9, 3);        // 2 input, 1 output, 9 rules, 3 sets per var
FuzzyTsukamoto fuzzyTsukamoto(2, 1, 9, 3);  // 2 input, 1 output, 9 rules, 3 sets per var

// Buffer untuk input serial
String inputString = "";
boolean stringComplete = false;

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("\nPerbandingan Metode Fuzzy Logic untuk Kontrol Kipas");
  Serial.println("--------------------------------------------------");

  // Setup ketiga sistem fuzzy
  setupFuzzyMamdani();
  setupFuzzySugeno();
  setupFuzzyTsukamoto();

  // Instruksi untuk pengguna
  Serial.println("\nMasukkan nilai suhu dan kelembapan (format: suhu,kelembapan)");
  Serial.println("Contoh: 30,75");
  Serial.println("Range suhu: 15-40°C, Range kelembapan: 30-90%");
}

// Setup sistem fuzzy Mamdani
void setupFuzzyMamdani() {
  Serial.println("Mempersiapkan sistem Fuzzy Mamdani...");

  // Menambahkan variabel input - suhu (15-40°C)
  fuzzyMamdani.addInputVariable("Temperature", 15.0, 40.0);

  // Menambahkan fuzzy set untuk suhu
  float coldTemp[3] = { 15.0, 20.0, 25.0 };
  float normalTemp[3] = { 20.0, 25.0, 30.0 };
  float hotTemp[3] = { 25.0, 30.0, 40.0 };

  fuzzyMamdani.addFuzzySet(0, true, "Cold", Fuzzy::TRIANGULAR, coldTemp);
  fuzzyMamdani.addFuzzySet(0, true, "Normal", Fuzzy::TRIANGULAR, normalTemp);
  fuzzyMamdani.addFuzzySet(0, true, "Hot", Fuzzy::TRIANGULAR, hotTemp);

  // Menambahkan variabel input - kelembapan (30-90%)
  fuzzyMamdani.addInputVariable("Humidity", 30.0, 90.0);

  // Menambahkan fuzzy set untuk kelembapan
  float dryHumid[3] = { 30.0, 30.0, 50.0 };
  float normalHumid[3] = { 40.0, 60.0, 80.0 };
  float wetHumid[3] = { 70.0, 90.0, 90.0 };

  fuzzyMamdani.addFuzzySet(1, true, "Dry", Fuzzy::TRIANGULAR, dryHumid);
  fuzzyMamdani.addFuzzySet(1, true, "Normal", Fuzzy::TRIANGULAR, normalHumid);
  fuzzyMamdani.addFuzzySet(1, true, "Wet", Fuzzy::TRIANGULAR, wetHumid);

  // Menambahkan variabel output - kecepatan kipas (0-100%)
  fuzzyMamdani.addOutputVariable("FanSpeed", 0.0, 100.0);

  // Menambahkan fuzzy set untuk kecepatan kipas
  float lowSpeed[3] = { 0.0, 0.0, 40.0 };
  float medSpeed[3] = { 20.0, 50.0, 80.0 };
  float highSpeed[3] = { 60.0, 100.0, 100.0 };

  fuzzyMamdani.addFuzzySet(0, false, "Low", Fuzzy::TRIANGULAR, lowSpeed);
  fuzzyMamdani.addFuzzySet(0, false, "Medium", Fuzzy::TRIANGULAR, medSpeed);
  fuzzyMamdani.addFuzzySet(0, false, "High", Fuzzy::TRIANGULAR, highSpeed);

  // Menambahkan aturan fuzzy
  addMamdaniRules();

  // Set metode defuzzifikasi
  fuzzyMamdani.setDefuzzificationMethod(Fuzzy::CENTROID);

  Serial.println("Sistem Fuzzy Mamdani berhasil diinisialisasi");
}

// Setup sistem fuzzy Sugeno
void setupFuzzySugeno() {
  Serial.println("Mempersiapkan sistem Fuzzy Sugeno...");

  // Menambahkan variabel input - suhu (15-40°C)
  fuzzySugeno.addInputVariable("Temperature", 15.0, 40.0);

  // Menambahkan fuzzy set untuk suhu
  float coldTemp[3] = { 15.0, 20.0, 25.0 };
  float normalTemp[3] = { 20.0, 25.0, 30.0 };
  float hotTemp[3] = { 25.0, 30.0, 40.0 };

  fuzzySugeno.addFuzzySet(0, "Cold", Fuzzy::TRIANGULAR, coldTemp);
  fuzzySugeno.addFuzzySet(0, "Normal", Fuzzy::TRIANGULAR, normalTemp);
  fuzzySugeno.addFuzzySet(0, "Hot", Fuzzy::TRIANGULAR, hotTemp);

  // Menambahkan variabel input - kelembapan (30-90%)
  fuzzySugeno.addInputVariable("Humidity", 30.0, 90.0);

  // Menambahkan fuzzy set untuk kelembapan
  float dryHumid[3] = { 30.0, 30.0, 50.0 };
  float normalHumid[3] = { 40.0, 60.0, 80.0 };
  float wetHumid[3] = { 70.0, 90.0, 90.0 };

  fuzzySugeno.addFuzzySet(0, "Dry", Fuzzy::TRIANGULAR, dryHumid);
  fuzzySugeno.addFuzzySet(0, "Normal", Fuzzy::TRIANGULAR, normalHumid);
  fuzzySugeno.addFuzzySet(0, "Wet", Fuzzy::TRIANGULAR, wetHumid);

  // Menambahkan constant outputs untuk Sugeno
  fuzzySugeno.addConstantOutput("LowSpeed", 20.0);     // Low speed - 20%
  fuzzySugeno.addConstantOutput("MediumSpeed", 50.0);  // Medium speed - 50%
  fuzzySugeno.addConstantOutput("HighSpeed", 80.0);    // High speed - 80%

  // Menambahkan aturan fuzzy
  addSugenoRules();

  Serial.println("Sistem Fuzzy Sugeno berhasil diinisialisasi");
}

// Setup sistem fuzzy Tsukamoto
void setupFuzzyTsukamoto() {
  Serial.println("Mempersiapkan sistem Fuzzy Tsukamoto...");

  // Menambahkan variabel input - suhu (15-40°C)
  fuzzyTsukamoto.addInputVariable("Temperature", 15.0, 40.0);

  // Menambahkan fuzzy set untuk suhu
  float coldTemp[3] = { 15.0, 20.0, 25.0 };
  float normalTemp[3] = { 20.0, 25.0, 30.0 };
  float hotTemp[3] = { 25.0, 30.0, 40.0 };

  fuzzyTsukamoto.addFuzzySet(0, true, "Cold", Fuzzy::TRIANGULAR, coldTemp);
  fuzzyTsukamoto.addFuzzySet(0, true, "Normal", Fuzzy::TRIANGULAR, normalTemp);
  fuzzyTsukamoto.addFuzzySet(0, true, "Hot", Fuzzy::TRIANGULAR, hotTemp);

  // Menambahkan variabel input - kelembapan (30-90%)
  fuzzyTsukamoto.addInputVariable("Humidity", 30.0, 90.0);

  // Menambahkan fuzzy set untuk kelembapan
  float dryHumid[3] = { 30.0, 30.0, 50.0 };
  float normalHumid[3] = { 40.0, 60.0, 80.0 };
  float wetHumid[3] = { 70.0, 90.0, 90.0 };

  fuzzyTsukamoto.addFuzzySet(1, true, "Dry", Fuzzy::TRIANGULAR, dryHumid);
  fuzzyTsukamoto.addFuzzySet(1, true, "Normal", Fuzzy::TRIANGULAR, normalHumid);
  fuzzyTsukamoto.addFuzzySet(1, true, "Wet", Fuzzy::TRIANGULAR, wetHumid);

  // Menambahkan variabel output - kecepatan kipas (0-100%)
  fuzzyTsukamoto.addOutputVariable("FanSpeed", 0.0, 100.0);

  // Menambahkan monotonic fuzzy set untuk output (khusus Tsukamoto)
  float lowSpeedMono[2] = { 40.0, 0.0 };      // Monotonic decreasing: 40->0
  float medSpeedMonoInc[2] = { 20.0, 50.0 };  // Monotonic increasing: 20->50
  float medSpeedMonoDec[2] = { 80.0, 50.0 };  // Monotonic decreasing: 80->50
  float highSpeedMono[2] = { 60.0, 100.0 };   // Monotonic increasing: 60->100

  fuzzyTsukamoto.addFuzzySet(0, false, "Low", Fuzzy::MONOTONIC_DECREASING, lowSpeedMono);
  fuzzyTsukamoto.addFuzzySet(0, false, "Medium_Inc", Fuzzy::MONOTONIC_INCREASING, medSpeedMonoInc);
  fuzzyTsukamoto.addFuzzySet(0, false, "Medium_Dec", Fuzzy::MONOTONIC_DECREASING, medSpeedMonoDec);
  fuzzyTsukamoto.addFuzzySet(0, false, "High", Fuzzy::MONOTONIC_INCREASING, highSpeedMono);

  // Menambahkan aturan fuzzy
  addTsukamotoRules();

  Serial.println("Sistem Fuzzy Tsukamoto berhasil diinisialisasi");
}

// Aturan-aturan untuk Fuzzy Mamdani
void addMamdaniRules() {
  // 1. Jika suhu dingin dan kelembapan kering, kipas lambat
  int vars1[2] = { 0, 1 };
  int sets1[2] = { 0, 0 };                            // Cold, Dry
  fuzzyMamdani.addRule(vars1, sets1, 2, 0, 0, true);  // Output: Low

  // 2. Jika suhu dingin dan kelembapan normal, kipas lambat
  int vars2[2] = { 0, 1 };
  int sets2[2] = { 0, 1 };                            // Cold, Normal
  fuzzyMamdani.addRule(vars2, sets2, 2, 0, 0, true);  // Output: Low

  // 3. Jika suhu dingin dan kelembapan basah, kipas sedang
  int vars3[2] = { 0, 1 };
  int sets3[2] = { 0, 2 };                            // Cold, Wet
  fuzzyMamdani.addRule(vars3, sets3, 2, 0, 1, true);  // Output: Medium

  // 4. Jika suhu normal dan kelembapan kering, kipas sedang
  int vars4[2] = { 0, 1 };
  int sets4[2] = { 1, 0 };                            // Normal, Dry
  fuzzyMamdani.addRule(vars4, sets4, 2, 0, 1, true);  // Output: Medium

  // 5. Jika suhu normal dan kelembapan normal, kipas sedang
  int vars5[2] = { 0, 1 };
  int sets5[2] = { 1, 1 };                            // Normal, Normal
  fuzzyMamdani.addRule(vars5, sets5, 2, 0, 1, true);  // Output: Medium

  // 6. Jika suhu normal dan kelembapan basah, kipas cepat
  int vars6[2] = { 0, 1 };
  int sets6[2] = { 1, 2 };                            // Normal, Wet
  fuzzyMamdani.addRule(vars6, sets6, 2, 0, 2, true);  // Output: High

  // 7. Jika suhu panas dan kelembapan kering, kipas cepat
  int vars7[2] = { 0, 1 };
  int sets7[2] = { 2, 0 };                            // Hot, Dry
  fuzzyMamdani.addRule(vars7, sets7, 2, 0, 2, true);  // Output: High

  // 8. Jika suhu panas dan kelembapan normal, kipas cepat
  int vars8[2] = { 0, 1 };
  int sets8[2] = { 2, 1 };                            // Hot, Normal
  fuzzyMamdani.addRule(vars8, sets8, 2, 0, 2, true);  // Output: High

  // 9. Jika suhu panas dan kelembapan basah, kipas cepat
  int vars9[2] = { 0, 1 };
  int sets9[2] = { 2, 2 };                            // Hot, Wet
  fuzzyMamdani.addRule(vars9, sets9, 2, 0, 2, true);  // Output: High
}

// Aturan-aturan untuk Fuzzy Sugeno
void addSugenoRules() {
  // 1. Jika suhu dingin dan kelembapan kering, kipas lambat
  int vars1[2] = { 0, 1 };
  int sets1[2] = { 0, 0 };                        // Cold, Dry
  fuzzySugeno.addRule(vars1, sets1, 2, 0, true);  // Output: LowSpeed

  // 2. Jika suhu dingin dan kelembapan normal, kipas lambat
  int vars2[2] = { 0, 1 };
  int sets2[2] = { 0, 1 };                        // Cold, Normal
  fuzzySugeno.addRule(vars2, sets2, 2, 0, true);  // Output: LowSpeed

  // 3. Jika suhu dingin dan kelembapan basah, kipas sedang
  int vars3[2] = { 0, 1 };
  int sets3[2] = { 0, 2 };                        // Cold, Wet
  fuzzySugeno.addRule(vars3, sets3, 2, 1, true);  // Output: MediumSpeed

  // 4. Jika suhu normal dan kelembapan kering, kipas sedang
  int vars4[2] = { 0, 1 };
  int sets4[2] = { 1, 0 };                        // Normal, Dry
  fuzzySugeno.addRule(vars4, sets4, 2, 1, true);  // Output: MediumSpeed

  // 5. Jika suhu normal dan kelembapan normal, kipas sedang
  int vars5[2] = { 0, 1 };
  int sets5[2] = { 1, 1 };                        // Normal, Normal
  fuzzySugeno.addRule(vars5, sets5, 2, 1, true);  // Output: MediumSpeed

  // 6. Jika suhu normal dan kelembapan basah, kipas cepat
  int vars6[2] = { 0, 1 };
  int sets6[2] = { 1, 2 };                        // Normal, Wet
  fuzzySugeno.addRule(vars6, sets6, 2, 2, true);  // Output: HighSpeed

  // 7. Jika suhu panas dan kelembapan kering, kipas cepat
  int vars7[2] = { 0, 1 };
  int sets7[2] = { 2, 0 };                        // Hot, Dry
  fuzzySugeno.addRule(vars7, sets7, 2, 2, true);  // Output: HighSpeed

  // 8. Jika suhu panas dan kelembapan normal, kipas cepat
  int vars8[2] = { 0, 1 };
  int sets8[2] = { 2, 1 };                        // Hot, Normal
  fuzzySugeno.addRule(vars8, sets8, 2, 2, true);  // Output: HighSpeed

  // 9. Jika suhu panas dan kelembapan basah, kipas cepat
  int vars9[2] = { 0, 1 };
  int sets9[2] = { 2, 2 };                        // Hot, Wet
  fuzzySugeno.addRule(vars9, sets9, 2, 2, true);  // Output: HighSpeed
}

// Aturan-aturan untuk Fuzzy Tsukamoto
void addTsukamotoRules() {
  // 1. Jika suhu dingin dan kelembapan kering, kipas lambat
  int vars1[2] = { 0, 1 };
  int sets1[2] = { 0, 0 };                              // Cold, Dry
  fuzzyTsukamoto.addRule(vars1, sets1, 2, 0, 0, true);  // Output: Low

  // 2. Jika suhu dingin dan kelembapan normal, kipas lambat
  int vars2[2] = { 0, 1 };
  int sets2[2] = { 0, 1 };                              // Cold, Normal
  fuzzyTsukamoto.addRule(vars2, sets2, 2, 0, 0, true);  // Output: Low

  // 3. Jika suhu dingin dan kelembapan basah, kipas sedang (increasing)
  int vars3[2] = { 0, 1 };
  int sets3[2] = { 0, 2 };                              // Cold, Wet
  fuzzyTsukamoto.addRule(vars3, sets3, 2, 0, 1, true);  // Output: Medium_Inc

  // 4. Jika suhu normal dan kelembapan kering, kipas sedang (increasing)
  int vars4[2] = { 0, 1 };
  int sets4[2] = { 1, 0 };                              // Normal, Dry
  fuzzyTsukamoto.addRule(vars4, sets4, 2, 0, 1, true);  // Output: Medium_Inc

  // 5. Jika suhu normal dan kelembapan normal, kipas sedang (decreasing)
  int vars5[2] = { 0, 1 };
  int sets5[2] = { 1, 1 };                              // Normal, Normal
  fuzzyTsukamoto.addRule(vars5, sets5, 2, 0, 2, true);  // Output: Medium_Dec

  // 6. Jika suhu normal dan kelembapan basah, kipas cepat
  int vars6[2] = { 0, 1 };
  int sets6[2] = { 1, 2 };                              // Normal, Wet
  fuzzyTsukamoto.addRule(vars6, sets6, 2, 0, 3, true);  // Output: High

  // 7. Jika suhu panas dan kelembapan kering, kipas cepat
  int vars7[2] = { 0, 1 };
  int sets7[2] = { 2, 0 };                              // Hot, Dry
  fuzzyTsukamoto.addRule(vars7, sets7, 2, 0, 3, true);  // Output: High

  // 8. Jika suhu panas dan kelembapan normal, kipas cepat
  int vars8[2] = { 0, 1 };
  int sets8[2] = { 2, 1 };                              // Hot, Normal
  fuzzyTsukamoto.addRule(vars8, sets8, 2, 0, 3, true);  // Output: High

  // 9. Jika suhu panas dan kelembapan basah, kipas cepat
  int vars9[2] = { 0, 1 };
  int sets9[2] = { 2, 2 };                              // Hot, Wet
  fuzzyTsukamoto.addRule(vars9, sets9, 2, 0, 3, true);  // Output: High
}

void loop() {
  // Membaca input dari Serial Monitor
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    if (inChar == '\n') {
      stringComplete = true;
    } else {
      inputString += inChar;
    }
  }

  // Memproses input jika sudah lengkap
  if (stringComplete) {
    // Memproses string input
    float temperature = -1, humidity = -1;

    // Parsing nilai suhu dan kelembapan
    int commaIndex = inputString.indexOf(',');

    if (commaIndex > 0) {
      temperature = inputString.substring(0, commaIndex).toFloat();
      humidity = inputString.substring(commaIndex + 1).toFloat();
    }

    // Validasi nilai input
    if (temperature >= 15.0 && temperature <= 40.0 && humidity >= 30.0 && humidity <= 90.0) {

      Serial.print("\nInput diterima: Suhu=");
      Serial.print(temperature);
      Serial.print("°C, Kelembapan=");
      Serial.print(humidity);
      Serial.println("%");

      // Menyiapkan data untuk evaluasi fuzzy
      float inputs[2] = { temperature, humidity };

      // Evaluasi dengan Fuzzy Mamdani
      Serial.println("\n--- Hasil Evaluasi Fuzzy Mamdani ---");
      float* mamdaniOutputs = fuzzyMamdani.evaluate(inputs);
      if (mamdaniOutputs != nullptr) {
        Serial.print("Kecepatan Kipas: ");
        Serial.print(mamdaniOutputs[0]);
        Serial.println("%");
        delete[] mamdaniOutputs;
      } else {
        Serial.println("Gagal mengevaluasi sistem Mamdani");
        if (fuzzyMamdani.hasError()) {
          Serial.print("Error: ");
          Serial.println(fuzzyMamdani.getErrorMessage());
        }
      }

      // Evaluasi dengan Fuzzy Sugeno
      Serial.println("\n--- Hasil Evaluasi Fuzzy Sugeno ---");
      float* sugenoOutputs = fuzzySugeno.evaluate(inputs);
      if (sugenoOutputs != nullptr) {
        Serial.print("Kecepatan Kipas: ");
        Serial.print(sugenoOutputs[0]);
        Serial.println("%");
        delete[] sugenoOutputs;
      } else {
        Serial.println("Gagal mengevaluasi sistem Sugeno");
        if (fuzzySugeno.hasError()) {
          Serial.print("Error: ");
          Serial.println(fuzzySugeno.getErrorMessage());
        }
      }

      // Evaluasi dengan Fuzzy Tsukamoto
      Serial.println("\n--- Hasil Evaluasi Fuzzy Tsukamoto ---");
      float* tsukamotoOutputs = fuzzyTsukamoto.evaluate(inputs);
      if (tsukamotoOutputs != nullptr) {
        Serial.print("Kecepatan Kipas: ");
        Serial.print(tsukamotoOutputs[0]);
        Serial.println("%");
        delete[] tsukamotoOutputs;
      } else {
        Serial.println("Gagal mengevaluasi sistem Tsukamoto");
        if (fuzzyTsukamoto.hasError()) {
          Serial.print("Error: ");
          Serial.println(fuzzyTsukamoto.getErrorMessage());
        }
      }

      // Perbandingan hasil
      Serial.println("\n--- Perbandingan Hasil ---");
      Serial.println("Metode \t\t| Kecepatan Kipas");
      Serial.println("------------------------");

      if (mamdaniOutputs != nullptr && sugenoOutputs != nullptr && tsukamotoOutputs != nullptr) {
        Serial.print("Mamdani \t| ");
        Serial.print(mamdaniOutputs[0]);
        Serial.println("%");

        Serial.print("Sugeno \t\t| ");
        Serial.print(sugenoOutputs[0]);
        Serial.println("%");

        Serial.print("Tsukamoto \t| ");
        Serial.print(tsukamotoOutputs[0]);
        Serial.println("%");
      }

    } else {
      Serial.println("Input tidak valid. Gunakan format: suhu,kelembapan");
      Serial.println("Range suhu: 15-40°C, Range kelembapan: 30-90%");
    }

    // Reset input
    inputString = "";
    stringComplete = false;

    // Instruksi untuk pengujian berikutnya
    Serial.println("\nMasukkan nilai untuk evaluasi berikutnya (format: suhu,kelembapan)");
  }

  // Delay kecil untuk stabilitas
  delay(10);
}