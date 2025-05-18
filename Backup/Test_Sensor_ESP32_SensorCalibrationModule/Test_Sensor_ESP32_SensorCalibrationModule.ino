#define ENABLE_SENSOR_MODULE
#define ENABLE_SENSOR_MODULE_UTILITY

#define ENABLE_INTERACTIVE_SERIAL_GENERAL_SENSOR_CALIBRATOR
#define ENABLE_SENSOR_CALIBRATION_MODULE

#define ENABLE_SENSOR_ANALOG

#include "Kinematrix.h"

// Pin analog untuk sensor ESP32
#define SOIL_MOISTURE_PIN 36  // GPIO36 (ADC1_CH0)
#define PH_SENSOR_PIN 39      // GPIO39 (ADC1_CH3)
#define TDS_SENSOR_PIN 34     // GPIO34 (ADC1_CH6)

// Referensi tegangan dan resolusi ADC untuk ESP32
#define VREF 3.3
#define ADC_RESOLUTION 4095

// Deklarasikan sensors sebagai variabel global
SensorCalibrationModule sensors;

void setup() {
  Serial.begin(115200);

  // Tunggu serial terhubung
  while (!Serial) {
    delay(10);
  }

  Serial.println("Inisialisasi sistem sensor pada ESP32...");

  // Konfigurasi ADC ESP32 (opsional, untuk meningkatkan akurasi)
  analogReadResolution(12);        // ESP32 memiliki ADC 12-bit (0-4095)
  analogSetAttenuation(ADC_11db);  // Pilih atenuasi yang sesuai (jangkauan 0-3.3V)

  // Buat instance sensor analog dengan lambda functions

  // Sensor kelembaban tanah dengan lambda
  auto soilMoisture = new AnalogSens(SOIL_MOISTURE_PIN, VREF, ADC_RESOLUTION, [](JsonVariant sensorRef, int analogValue, float voltage) {
    // Konversi nilai analog ke persentase kelembaban
    // Catatan: ESP32 mungkin memiliki nilai air dan udara yang berbeda
    // dibandingkan Arduino. Nilai berikut hanya perkiraan
    float airValue = 1500;  // Nilai ADC saat sensor di air (perlu dikalibrasi)
    float dryValue = 3200;  // Nilai ADC saat sensor di udara (perlu dikalibrasi)

    float moisturePercent = 100.0 - ((analogValue - airValue) / (dryValue - airValue)) * 100.0;

    // Batasi nilai dalam rentang 0-100%
    moisturePercent = constrain(moisturePercent, 0.0, 100.0);

    sensorRef["percent"] = moisturePercent;
  });

  // Sensor pH dengan lambda
  auto phSensor = new AnalogSens(PH_SENSOR_PIN, VREF, ADC_RESOLUTION, [](JsonVariant sensorRef, int analogValue, float voltage) {
    // Untuk sensor pH pada ESP32, nilai raw bisa berbeda
    // Contoh rumus (perlu dikalibrasi): pH = 7 - ((voltage - 2.5) / 0.18)
    float phValue = 7.0 - ((voltage - 1.65) / 0.18);

    // Batasi nilai dalam rentang pH yang masuk akal
    phValue = constrain(phValue, 0.0, 14.0);

    sensorRef["ph"] = phValue;
  });

  // Sensor TDS dengan lambda
  auto tdsSensor = new AnalogSens(TDS_SENSOR_PIN, VREF, ADC_RESOLUTION, [](JsonVariant sensorRef, int analogValue, float voltage) {
    // Perhitungan TDS untuk ESP32
    float temperature = 25.0;  // Suhu default untuk kompensasi
    float compensationCoefficient = 1.0 + 0.02 * (temperature - 25.0);
    float compensationVoltage = voltage / compensationCoefficient;

    // Rumus konversi TDS
    // TDS = faktorkalibrasi * EC (μS/cm)
    // Rumus EC: EC = (voltage / VREF) * 200.0
    float ec = (compensationVoltage / VREF) * 200.0;
    float tdsValue = ec * 0.5;  // Faktor konversi dari EC ke TDS (ppm)

    sensorRef["tds"] = tdsValue;
    sensorRef["ec"] = ec;
  });

  // Tambahkan sensor ke modul
  sensors.addModule("soil", soilMoisture);
  sensors.addModule("ph", phSensor);
  sensors.addModule("tds", tdsSensor);

  // Inisialisasi semua sensor
  sensors.init();

  // Tambahkan kalibrator untuk setiap sensor dan konfigurasi
  sensors.addCalibrator("soil");
  sensors.setCalibrationParameter("soil", "%", 1);  // unit = %, presisi = 1 desimal

  sensors.addCalibrator("ph");
  sensors.setCalibrationParameter("ph", "pH", 2);  // unit = pH, presisi = 2 desimal

  sensors.addCalibrator("tds");
  sensors.setCalibrationParameter("tds", "ppm", 0);  // unit = ppm, presisi = 0 desimal

  // Mulai kalibrasi interaktif melalui Serial
  Serial.println("Masuk ke mode kalibrasi. Ketik 'h' untuk bantuan.");
  sensors.startCalibrationMode(&Serial);

  // Coba muat kalibrasi dari EEPROM (jika ada)
  if (sensors.loadAllCalibrations()) {
    Serial.println("Berhasil memuat kalibrasi sebelumnya dari EEPROM");
  } else {
    Serial.println("Tidak ada kalibrasi tersimpan atau gagal memuat");
  }
}

void loop() {
  // Update semua sensor
  sensors.update();

  // Akses nilai terkalibrasi atau nilai mentah
  float soilMoisturePercent = sensors.getCalibratedValue("soil");
  float phValue = sensors.getCalibratedValue("ph");
  float tdsValue = sensors.getCalibratedValue("tds");

  // Akses nilai mentah (raw) dan tegangan
  float soilRaw = sensors["soil"]["raw"].as<float>();
  float soilVolt = sensors["soil"]["volt"].as<float>();

  float phRaw = sensors["ph"]["raw"].as<float>();
  float phVolt = sensors["ph"]["volt"].as<float>();

  float tdsRaw = sensors["tds"]["raw"].as<float>();
  float tdsVolt = sensors["tds"]["volt"].as<float>();

  // Cetak nilai sensor (format lebih baik untuk ESP32)
  // Serial.println("===== SENSOR READINGS =====");

  // Serial.print("Soil Moisture: ");
  // Serial.print(soilMoisturePercent, 1);
  // Serial.print("% \t(Raw: ");
  // Serial.print(soilRaw, 0);
  // Serial.print(", ");
  // Serial.print(soilVolt, 3);
  // Serial.println("V)");

  // Serial.print("pH: ");
  // Serial.print(phValue, 2);
  // Serial.print(" \t(Raw: ");
  // Serial.print(phRaw, 0);
  // Serial.print(", ");
  // Serial.print(phVolt, 3);
  // Serial.println("V)");

  // Serial.print("TDS: ");
  // Serial.print(tdsValue, 0);
  // Serial.print(" ppm \t(Raw: ");
  // Serial.print(tdsRaw, 0);
  // Serial.print(", ");
  // Serial.print(tdsVolt, 3);
  // Serial.println("V)");

  // Serial.println("=========================");

  // Menggunakan debug untuk tampilan alternatif
  // sensors.debug();

  delay(2000);
}