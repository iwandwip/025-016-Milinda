/*
 * Perbandingan Fuzzy Logic: Sugeno, Mamdani, dan Tsukamoto
 * Contoh sederhana dengan 1 input dan 1 output
 * 
 * Input: Suhu (0-50°C)
 * Output: Kecepatan kipas (0-100%)
 */

#define ENABLE_MODULE_FUZZY_MAMDANI
#define ENABLE_MODULE_FUZZY_SUGENO
#define ENABLE_MODULE_FUZZY_TSUKAMOTO

#include "Kinematrix.h"

// Deklarasi objek fuzzy
FuzzyMamdani fuzzyMamdani(1, 1, 3, 3);      // 1 input, 1 output, 3 rules, 3 sets per var
FuzzySugeno fuzzySugeno(1, 1, 3, 3);        // 1 input, 1 output, 3 rules, 3 sets per var
FuzzyTsukamoto fuzzyTsukamoto(1, 1, 3, 3);  // 1 input, 1 output, 3 rules, 3 sets per var

void setupFuzzyMamdani() {
  // Tambahkan variabel input
  fuzzyMamdani.addInputVariable("Suhu", 0.0f, 50.0f);  // Input suhu (0-50°C)

  // Tambahkan variabel output
  fuzzyMamdani.addOutputVariable("KipasSpeed", 0.0f, 100.0f);  // Output kecepatan kipas (0-100%)

  // Definisikan fuzzy set untuk Input Suhu
  float paramsDingin[] = { 0.0f, 0.0f, 25.0f };    // Triangular: dingin
  float paramsNormal[] = { 15.0f, 25.0f, 35.0f };  // Triangular: normal
  float paramsPanas[] = { 25.0f, 50.0f, 50.0f };   // Triangular: panas

  fuzzyMamdani.addFuzzySet(0, true, "Dingin", Fuzzy::TRIANGULAR, paramsDingin);
  fuzzyMamdani.addFuzzySet(0, true, "Normal", Fuzzy::TRIANGULAR, paramsNormal);
  fuzzyMamdani.addFuzzySet(0, true, "Panas", Fuzzy::TRIANGULAR, paramsPanas);

  // Definisikan fuzzy set untuk Output Kecepatan Kipas
  float paramsLambat[] = { 0.0f, 0.0f, 50.0f };     // Triangular: lambat
  float paramsSedang[] = { 25.0f, 50.0f, 75.0f };   // Triangular: sedang
  float paramsCepat[] = { 50.0f, 100.0f, 100.0f };  // Triangular: cepat

  fuzzyMamdani.addFuzzySet(0, false, "Lambat", Fuzzy::TRIANGULAR, paramsLambat);
  fuzzyMamdani.addFuzzySet(0, false, "Sedang", Fuzzy::TRIANGULAR, paramsSedang);
  fuzzyMamdani.addFuzzySet(0, false, "Cepat", Fuzzy::TRIANGULAR, paramsCepat);

  // Aturan fuzzy
  // Rule 1: IF Suhu Dingin THEN Kipas Lambat
  int antecedentVars1[] = { 0 };                                          // Suhu
  int antecedentSets1[] = { 0 };                                          // Dingin
  fuzzyMamdani.addRule(antecedentVars1, antecedentSets1, 1, 0, 0, true);  // Output: Lambat

  // Rule 2: IF Suhu Normal THEN Kipas Sedang
  int antecedentVars2[] = { 0 };                                          // Suhu
  int antecedentSets2[] = { 1 };                                          // Normal
  fuzzyMamdani.addRule(antecedentVars2, antecedentSets2, 1, 0, 1, true);  // Output: Sedang

  // Rule 3: IF Suhu Panas THEN Kipas Cepat
  int antecedentVars3[] = { 0 };                                          // Suhu
  int antecedentSets3[] = { 2 };                                          // Panas
  fuzzyMamdani.addRule(antecedentVars3, antecedentSets3, 1, 0, 2, true);  // Output: Cepat

  // Set metode defuzzifikasi
  fuzzyMamdani.setDefuzzificationMethod(Fuzzy::CENTROID);
}

void setupFuzzySugeno() {
  // Tambahkan variabel input
  fuzzySugeno.addInputVariable("Suhu", 0.0f, 50.0f);  // Input suhu (0-50°C)

  // Tambahkan variabel output
  fuzzySugeno.addOutputVariable("KipasSpeed", 0.0f, 100.0f);  // Output kecepatan kipas (0-100%)

  // Definisikan fuzzy set untuk Input Suhu (sama dengan Mamdani)
  float paramsDingin[] = { 0.0f, 0.0f, 25.0f };    // Triangular: dingin
  float paramsNormal[] = { 15.0f, 25.0f, 35.0f };  // Triangular: normal
  float paramsPanas[] = { 25.0f, 50.0f, 50.0f };   // Triangular: panas

  fuzzySugeno.addFuzzySet(0, true, "Dingin", Fuzzy::TRIANGULAR, paramsDingin);
  fuzzySugeno.addFuzzySet(0, true, "Normal", Fuzzy::TRIANGULAR, paramsNormal);
  fuzzySugeno.addFuzzySet(0, true, "Panas", Fuzzy::TRIANGULAR, paramsPanas);

  // Definisi output konstanta untuk Sugeno
  fuzzySugeno.addConstantOutput(0, "Lambat", 20.0f);  // Output konstanta = 20%
  fuzzySugeno.addConstantOutput(0, "Sedang", 50.0f);  // Output konstanta = 50%
  fuzzySugeno.addConstantOutput(0, "Cepat", 80.0f);   // Output konstanta = 80%

  // Aturan fuzzy Sugeno
  // Rule 1: IF Suhu Dingin THEN Kipas Lambat
  int antecedentVars1[] = { 0 };                                         // Suhu
  int antecedentSets1[] = { 0 };                                         // Dingin
  fuzzySugeno.addRule(antecedentVars1, antecedentSets1, 1, 0, 0, true);  // Output: Lambat

  // Rule 2: IF Suhu Normal THEN Kipas Sedang
  int antecedentVars2[] = { 0 };                                         // Suhu
  int antecedentSets2[] = { 1 };                                         // Normal
  fuzzySugeno.addRule(antecedentVars2, antecedentSets2, 1, 0, 1, true);  // Output: Sedang

  // Rule 3: IF Suhu Panas THEN Kipas Cepat
  int antecedentVars3[] = { 0 };                                         // Suhu
  int antecedentSets3[] = { 2 };                                         // Panas
  fuzzySugeno.addRule(antecedentVars3, antecedentSets3, 1, 0, 2, true);  // Output: Cepat
}

void setupFuzzyTsukamoto() {
  // Tambahkan variabel input
  fuzzyTsukamoto.addInputVariable("Suhu", 0.0f, 50.0f);  // Input suhu (0-50°C)

  // Tambahkan variabel output
  fuzzyTsukamoto.addOutputVariable("KipasSpeed", 0.0f, 100.0f);  // Output kecepatan kipas (0-100%)

  // Definisikan fuzzy set untuk Input Suhu (sama dengan Mamdani dan Sugeno)
  float paramsDingin[] = { 0.0f, 0.0f, 25.0f };    // Triangular: dingin
  float paramsNormal[] = { 15.0f, 25.0f, 35.0f };  // Triangular: normal
  float paramsPanas[] = { 25.0f, 50.0f, 50.0f };   // Triangular: panas

  fuzzyTsukamoto.addFuzzySet(0, true, "Dingin", Fuzzy::TRIANGULAR, paramsDingin);
  fuzzyTsukamoto.addFuzzySet(0, true, "Normal", Fuzzy::TRIANGULAR, paramsNormal);
  fuzzyTsukamoto.addFuzzySet(0, true, "Panas", Fuzzy::TRIANGULAR, paramsPanas);

  // Definisi output monotonic untuk Tsukamoto yang dikoreksi
  // Perubahan parameter untuk menghasilkan output yang sesuai dengan Expected
  // Parameter yang benar untuk Tsukamoto
  // Definisi output monotonic untuk Tsukamoto - Parameter yang dikoreksi
  float paramsLambat[] = { 30.0f, 0.0f };   // Monotonic Decreasing (dingin)
  float paramsSedang[] = { 25.0f, 50.0f };  // Monotonic Increasing (normal)
  float paramsCepat[] = { 50.0f, 100.0f };  // Monotonic Increasing (panas)

  fuzzyTsukamoto.addFuzzySet(0, false, "Lambat", Fuzzy::MONOTONIC_DECREASING, paramsLambat);
  fuzzyTsukamoto.addFuzzySet(0, false, "Sedang", Fuzzy::MONOTONIC_INCREASING, paramsSedang);
  fuzzyTsukamoto.addFuzzySet(0, false, "Cepat", Fuzzy::MONOTONIC_INCREASING, paramsCepat);

  // Aturan fuzzy Tsukamoto
  // Rule 1: IF Suhu Dingin THEN Kipas Lambat
  int antecedentVars1[] = { 0 };                                            // Suhu
  int antecedentSets1[] = { 0 };                                            // Dingin
  fuzzyTsukamoto.addRule(antecedentVars1, antecedentSets1, 1, 0, 0, true);  // Output: Lambat

  // Rule 2: IF Suhu Normal THEN Kipas Sedang
  int antecedentVars2[] = { 0 };                                            // Suhu
  int antecedentSets2[] = { 1 };                                            // Normal
  fuzzyTsukamoto.addRule(antecedentVars2, antecedentSets2, 1, 0, 1, true);  // Output: Sedang

  // Rule 3: IF Suhu Panas THEN Kipas Cepat
  int antecedentVars3[] = { 0 };                                            // Suhu
  int antecedentSets3[] = { 2 };                                            // Panas
  fuzzyTsukamoto.addRule(antecedentVars3, antecedentSets3, 1, 0, 2, true);  // Output: Cepat
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("Inisialisasi Fuzzy Logic Systems...");

  // Setup ketiga jenis fuzzy
  setupFuzzyMamdani();
  setupFuzzySugeno();
  setupFuzzyTsukamoto();

  Serial.println("Sistem Fuzzy Siap!");
  Serial.println("\n=== Perbandingan Metode Fuzzy (Sugeno, Mamdani, Tsukamoto) ===");
  Serial.println("Masukkan suhu dalam format: [suhu]");
  Serial.println("Contoh: 25");

  // Output tabel perbandingan
  Serial.println("\n|---------------|------------------|------------------|---------------------|");
  Serial.println("| Input (Suhu °C) | Output Mamdani (%) | Output Sugeno (%) | Output Tsukamoto (%) |");
  Serial.println("|---------------|------------------|------------------|---------------------|");

  // Cetak hasil untuk berbagai input suhu
  for (int temp = 0; temp <= 50; temp += 5) {
    float inputValues[] = { (float)temp };

    // Evaluasi ketiga jenis fuzzy
    float *outputsMamdani = fuzzyMamdani.evaluate(inputValues);
    float *outputsSugeno = fuzzySugeno.evaluate(inputValues);
    float *outputsTsukamoto = fuzzyTsukamoto.evaluate(inputValues);

    if (outputsMamdani && outputsSugeno && outputsTsukamoto) {
      Serial.print("| ");
      Serial.print(temp);
      Serial.print("              | ");
      Serial.print(outputsMamdani[0], 2);
      Serial.print("             | ");
      Serial.print(outputsSugeno[0], 2);
      Serial.print("            | ");
      Serial.print(outputsTsukamoto[0], 2);
      Serial.println("               |");

      // Membebaskan memori
      delete[] outputsMamdani;
      delete[] outputsSugeno;
      delete[] outputsTsukamoto;
    }
  }

  Serial.println("|---------------|------------------|------------------|---------------------|");
}

void loop() {
  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');
    input.trim();

    float temperature = input.toFloat();

    // Validasi input
    if (temperature < 0 || temperature > 50) {
      Serial.println("Error: Nilai di luar rentang. Suhu: 0-50");
      return;
    }

    Serial.print("| Input: Suhu = ");
    Serial.print(temperature);
    Serial.print("°C");

    // Evaluasi ketiga jenis fuzzy
    float inputValues[] = { temperature };

    // Evaluasi Fuzzy Mamdani
    float *outputsMamdani = fuzzyMamdani.evaluate(inputValues);
    if (outputsMamdani == nullptr) {
      Serial.println("Error evaluasi Fuzzy Mamdani");
      return;
    }

    // Evaluasi Fuzzy Sugeno
    float *outputsSugeno = fuzzySugeno.evaluate(inputValues);
    if (outputsSugeno == nullptr) {
      delete[] outputsMamdani;
      Serial.println("Error evaluasi Fuzzy Sugeno");
      return;
    }

    // Evaluasi Fuzzy Tsukamoto
    float *outputsTsukamoto = fuzzyTsukamoto.evaluate(inputValues);
    if (outputsTsukamoto == nullptr) {
      delete[] outputsMamdani;
      delete[] outputsSugeno;
      Serial.println("Error evaluasi Fuzzy Tsukamoto");
      return;
    }

    // Tampilkan hasil
    Serial.print("| Mamdani  : ");
    Serial.print(outputsMamdani[0], 2);
    Serial.print("%");

    Serial.print("| Sugeno   : ");
    Serial.print(outputsSugeno[0], 2);
    Serial.print("%");

    Serial.print("| Tsukamoto: ");
    Serial.print(outputsTsukamoto[0], 2);
    Serial.print("%");

    Serial.println();

    // Membebaskan memori
    delete[] outputsMamdani;
    delete[] outputsSugeno;
    delete[] outputsTsukamoto;
  }
}