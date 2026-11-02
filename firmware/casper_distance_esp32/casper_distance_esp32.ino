#include <WiFi.h>
#include <HTTPClient.h>
#include "secrets.h"  // Local-only configuration; do not upload to GitHub.

// HC-SR04 pins on the ESP32-S3 (adapt to your wiring if needed).
constexpr uint8_t TRIG_PIN = 10;
constexpr uint8_t ECHO_PIN = 11;

constexpr unsigned long WIFI_TIMEOUT_MS = 15000;
constexpr unsigned long MEASUREMENT_INTERVAL_MS = 2000;

bool connectWiFi() {
  if (WiFi.status() == WL_CONNECTED) {
    return true;
  }

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");

  const unsigned long startTime = millis();
  while (WiFi.status() != WL_CONNECTED &&
         millis() - startTime < WIFI_TIMEOUT_MS) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Wi-Fi connection failed. Will retry.");
    return false;
  }

  Serial.println("Wi-Fi connected.");
  Serial.print("ESP32 IP: ");
  Serial.println(WiFi.localIP());
  return true;
}

float readDistanceCm() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  const unsigned long duration = pulseIn(ECHO_PIN, HIGH, 30000UL);
  if (duration == 0) {
    return -1.0f; // No echo received within the timeout.
  }

  return duration * 0.0343f / 2.0f;
}

void sendDistanceCm(float distanceCm) {
  if (!connectWiFi()) {
    Serial.println("Skipping upload: Wi-Fi is unavailable.");
    return;
  }

  HTTPClient http;
  http.setTimeout(5000);
  if (!http.begin(SERVER_URL)) {
    Serial.println("Unable to initialize HTTP request.");
    return;
  }

  http.addHeader("Content-Type", "application/json");
  const String payload =
      String("{\"device\":\"ESP32-S3\",\"distance_cm\":") +
      String(distanceCm, 1) + "}";

  Serial.print("Sending: ");
  Serial.println(payload);

  const int httpCode = http.POST(payload);
  if (httpCode > 0) {
    Serial.print("HTTP status: ");
    Serial.println(httpCode);
    Serial.print("Server response: ");
    Serial.println(http.getString());
  } else {
    Serial.print("HTTP request failed: ");
    Serial.println(http.errorToString(httpCode));
  }

  http.end();
}

void setup() {
  Serial.begin(115200);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  digitalWrite(TRIG_PIN, LOW);

  connectWiFi();
  Serial.println("CASPER DISTANCE CLIENT READY");
}

void loop() {
  const float distanceCm = readDistanceCm();

  if (distanceCm < 0) {
    Serial.println("Sensor timeout: no echo received.");
  } else {
    Serial.print("Distance: ");
    Serial.print(distanceCm, 1);
    Serial.println(" cm");
    sendDistanceCm(distanceCm);
  }

  delay(MEASUREMENT_INTERVAL_MS);
}
