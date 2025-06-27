void setupSensor(){
  pinMode(soilPin , INPUT);
  pinMode(uvPin , OUTPUT);
  digitalWrite(uvPin , HIGH);
  guva.begin(guvaPin , 3.3 , 1023);
 Serial.println("Adafruit MLX90614 Emissivity Setter.\n");

  // init sensor
  if (!mlx.begin()) {
    Serial.println("Error connecting to MLX sensor. Check wiring.");
    while (1);
  };

  // read current emissivity
  Serial.print("Current emissivity = "); Serial.println(mlx.readEmissivity());

  // set new emissivity
  Serial.print("Setting emissivity = "); Serial.println(new_emissivity);
  mlx.writeEmissivity(new_emissivity); // this does the 0x0000 erase write

  // read back
  Serial.print("New emissivity = "); Serial.println(mlx.readEmissivity());

  // done
  Serial.print("DONE. Restart the module.");

}

void readSensor(){
  soilVal = map(analogRead(soilPin) , 0 , 4095 , 0 , 100);
  guvaVal = guva.read(); 
  Serial.println(analogRead(guvaPin));
  mlxVal = mlx.readAmbientTempC();
}