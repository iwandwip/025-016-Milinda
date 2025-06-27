void TaskDatabase(void *pvParameters) {
  setupNetwork();
  setupDatabase();
  while (true) {
    updateData();
    vTaskDelay(1000 / portTICK_PERIOD_MS);  // Delay dalam ms (misalnya 100ms)
  }
}

void TaskControl(void *pvParameters) {
  setupDisplay();
  setupSensor();
  while (true) {
    if(Serial.available()  ) if(Serial.read() == 'r') ESP.restart();
    readSensor();
    displayData();
    vTaskDelay(500 / portTICK_PERIOD_MS);  // Delay dalam ms (misalnya 100ms)
  }
}


TaskHandle_t DatabaseHandle ;
TaskHandle_t ControlHandle ;

// Fungsi setup RTOS
void setupRTOS() {
  // Membuat task untuk Database Firebase di Core 0
  xTaskCreatePinnedToCore(
    TaskDatabase,     // Nama fungsi task
    "TaskDatabase",   // Nama task
    10000,            // Ukuran stack (10KB)
    NULL,             // Parameter untuk task (NULL jika tidak ada)
    1,                // Prioritas task (1 adalah prioritas rendah)
    &DatabaseHandle,  // Handle untuk task
    0                 // Core 0
  );

  // Membuat task untuk Control Pin di Core 1
  xTaskCreatePinnedToCore(
    TaskControl,      // Nama fungsi task
    "TaskControl",    // Nama task
    10000,             // Ukuran stack (10KB) untuk kontrol pin
    NULL,             // Parameter untuk task (NULL jika tidak ada)
    2,                // Prioritas task (2 adalah prioritas menengah)
    &ControlHandle,   // Handle untuk task
    1                 // Core 1
  );


  Serial.println("Setup RTOS Finish !!");
}