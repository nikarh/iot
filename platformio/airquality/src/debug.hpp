void debugBme280(
  float temp,
  float press,
  float humi_uncalibrated,
  float humi
) {
  Serial.print("Temperature = ");
  Serial.print(temp);
  Serial.println(" *C");
  Serial.print("Pressure = ");
  Serial.print(press);
  Serial.println(" hPa");
  Serial.print("Uncalibrated humidity = ");
  Serial.print(humi_uncalibrated);
  Serial.println(" %");
  Serial.print("Humidity = ");
  Serial.print(humi);
  Serial.println(" %");
}

void debugCo2(float ppm) {
  Serial.print("CO2: ");
  Serial.print(ppm);
  Serial.println();
}