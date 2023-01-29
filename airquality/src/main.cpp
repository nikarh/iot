#include "optimize.hpp"

#include <WiFiClientSecure.h>
#include <Preferences.h>
#include <Wire.h>
#include <WiFi.h>

#include <ArduinoHA.h>
#include <ArduinoHADefines.h>
#include <HADevice.h>
#include <HAMqtt.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <MHZ.h>

#include "calibration.hpp"
#include "secrets.hpp"
#include "debug.hpp"
#include "ha.hpp"

#define UART_BAUD 9600

// GPIO pins
#define I2C_SDA 32
#define I2C_SCL 33
#define CO2_PWM 14
#define UART_RX 26
#define UART_TX 25

TwoWire i2cBme = TwoWire(0);
Adafruit_BME280 bme;
MHZ co2(UART_RX, UART_TX, CO2_PWM, MHZ19C);

HADevice device;
WiFiClientSecure wifiClient;
HAMqtt mqtt(wifiClient, device, 12);

Preferences preferences;
Calibration cal;
HA* ha;

void onCalibrationChange(HANumeric number, HANumber* sender) {
  auto value = number.toFloat();
  auto id = sender->uniqueId() + 13;

  Serial.print("Incoming calibration change for ");
  Serial.println(id);

  preferences.begin("airquality", false);
  preferences.putFloat(nameToPref(id), value);
  preferences.end();

  cal.set(id, value);

  if (strcmp(id, "temperature_shift") == 0) {
    bme.setTemperatureCompensation(value);
  }

  sender->setState(value);
}

void onCo2Calibrate(HAButton* sender) {
  Serial.println("Calibrating CO2 zero");
  co2.calibrateZero();
}

void initHass() {
  byte mac[6];
  WiFi.macAddress(mac);

  device.setUniqueId(mac, 6);
  device.setName("Air quality monitor");
  device.setSoftwareVersion("0.1");
  device.setManufacturer("nikarh");
  device.setModel("Giga Sensor Maximum");
  device.enableSharedAvailability();
  device.enableLastWill();

  mqtt.begin(mqtt_broker, mqtt_port, mqtt_username, mqtt_password);
}

void initWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());

  WiFi.setAutoReconnect(true);
  WiFi.persistent(true);
}

void setupTempSensor() {
  Serial.print("i2c clock ");
  Serial.println(i2cBme.getClock());

  i2cBme.begin(I2C_SDA, I2C_SCL, 100000);
  bool status = bme.begin(0x76, &i2cBme);
  if (!status) {
    Serial.print("Could not connect to BME sensor! error");
    Serial.println(i2cBme.getWriteError());
  }

  Serial.print("Setting temp compensation to ");
  Serial.print(cal.temperature);
  Serial.println();
  bme.setTemperatureCompensation(cal.temperature);

  bme.setSampling(
    Adafruit_BME280::MODE_NORMAL,
    Adafruit_BME280::SAMPLING_X8,  // Temp
    Adafruit_BME280::SAMPLING_X8,  // Press
    Adafruit_BME280::SAMPLING_X8,  // Humi
    Adafruit_BME280::FILTER_X2,
    Adafruit_BME280::STANDBY_MS_1000);
}

void updateTemperatureValues() {
  float temp = bme.readTemperature();
  float press = bme.readPressure() / 100.0F;

  float humi_uncalibrated = bme.readHumidity();
  float humi = cal.humidity.fix(humi_uncalibrated);

  ha->sensor_temp.setValue(temp);
  ha->sensor_press.setValue(press);
  ha->sensor_humi.setValue(humi);

  debugBme280(temp, press, humi_uncalibrated, humi);
}

void updateCo2() {
  if (!co2.isReady()) return;

  int ppm = co2.readCO2PWM();
  ha->sensor_co2.setValue(ppm);

  debugCo2(ppm);
}

void setup() {
  Serial.begin(UART_BAUD);
  wifiClient.setCACert(mqtt_ca);

  initWiFi();
  initHass();

  preferences.begin("airquality", false);
  cal = initCalibration(preferences, device.getUniqueId());
  preferences.end();

  ha = new HA(device.getUniqueId(), onCalibrationChange);
  ha->init(cal);
  ha->calibrate_co2.onCommand(onCo2Calibrate);

  setupTempSensor();

  co2.setAutoCalibrate(false);
}

unsigned long lastUpdateAt = 0;
void loop() {
  mqtt.loop();

  auto diff = millis() - lastUpdateAt;
  if (diff > 5000 || diff < 0) {
    updateTemperatureValues();
    updateCo2();
    lastUpdateAt = millis();
    Serial.println();
  }
}
