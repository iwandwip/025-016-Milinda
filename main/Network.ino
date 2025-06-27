
void setupNetwork() {
  // Mulai koneksi ke Wi-Fi
  Serial.println("Connecting to WiFi...");
  
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  
  // Tunggu sampai terhubung
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);  // Tunggu 1 detik
    Serial.print(".");
  }

  // Jika berhasil terhubung, tampilkan IP Address
  Serial.println();
  Serial.println("WiFi Connected");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

void setupDatabase() {

  Firebase.printf("Firebase Client v%s\n", FIREBASE_CLIENT_VERSION);

  set_ssl_client_insecure_and_buffer(ssl_client);

  Serial.println("Initializing app...");
  initializeApp(aClient, app, getAuth(user_auth), auth_debug_print, "🔐 authTask");

  // Or intialize the app and wait.
  // initializeApp(aClient, app, getAuth(user_auth), 120 * 1000, auth_debug_print);

  app.getApp<RealtimeDatabase>(Database);

  Database.url(DATABASE_URL);
  app.loop();

  Serial.println("Initializing app finish!!");
}



void updateData() {
  // To maintain the authentication and async tasks
  // app.loop();

  if (app.ready()) {
    Serial.println("------------------------------");
    Serial.println("🕒 Await get values");
    Serial.println("------------------------------");
    set_await();
  }
}



void show_status(bool status) {
  if (status)
    Serial.println("Success");
  else
    Firebase.printf("Error, msg: %s, code: %d\n", aClient.lastError().message().c_str(), aClient.lastError().code());
}

void set_await() {

  // object_t json, obj1, obj2, obj3, obj4;
  // JsonWriter writer;

  // writer.create(obj1, "kelembapan", string_t(String(h)));
  // writer.create(obj2, "status_penyemprotan", string_t(DTO ? "Tidak Perlu disiram" : "Perlu Disiram"));
  // writer.create(obj3, "suhu", string_t(String(t)));
  // writer.create(obj4, "uv", number_t(uv));


  // writer.join(json, 4, obj1, obj2, obj3, obj4);
  // // To print object_t
  // // Serial.println(json);

  // Serial.println("Setting the JSON value... ");
  // bool status = Database.set<object_t>(aClient, "status_terakhir", json);
  // show_status(status);
}