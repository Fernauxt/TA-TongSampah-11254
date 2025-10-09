#define _DEBUG_

#include <Arduino.h>
#include <ESP32Servo.h>
#include <WiFi.h>
#include <ThingerESP32.h>
#include <NewPing.h>
#include "config.h"

// -- inisialisasi objek global
Servo motorServo;
ThingerESP32 thing(thinger_username, thinger_device_id, thinger_device_credential);
NewPing sonar(trigger_pin, echo_pin, bin_capacity);

// -- inisiasi variabel global
unsigned long lidOpenTime = 0;
bool statusServo = false; // false = tutup, true = buka
int currentCapacity = 0; // kapasitas saat ini

// -- fungsi helper
// float readDistance(int trigPin, int echoPin); // dalam cm
int countCapacityPercentage(float distance); // dalam %
void openLid();
void closeLid();
void connectToWiFi();

// -- fungsi setup
void setup()
{
  // Memulai komunikasi serial pada kecepatan 115200 bps
  Serial.begin(115200);
  Serial.println("Inisialisasi Sistem Inti Tong Sampah Pintar (singular mode)...");

  // Beri jeda sedikit agar serial monitor siap
  delay(1000);

  pinMode(ir_pin, INPUT_PULLUP);
  motorServo.attach(servo_pin);
  closeLid();

  // Setup koneksi WiFi dan Thinger.io
  Serial.println("Menghubungkan ke WiFi...");
  connectToWiFi();
  
  // Setup Thinger.io resources
  // char resourceName[32];
  // sprintf(resourceName, "bin_capacity_%d", device_index);
  thing["capacity"] >> outputValue(currentCapacity);

  Serial.println("=====================================");
  Serial.println("Setup Selesai. ESP32 siap.");
  Serial.println("Proyek Tong Sampah Pintar Dimulai!");
  Serial.println("=====================================");
}

void loop()
{
  unsigned long currentMillis = millis();

  if (WiFi.status() != WL_CONNECTED) {
    connectToWiFi();
  } else {
    thing.handle(); // menangani komunikasi Thinger.io
  }

  // logika buka tutup tong sampah
  if (digitalRead(ir_pin) == LOW && !statusServo)  {
    Serial.println("Tangan terdeteksi! Membuka tong...");
    openLid();
    lidOpenTime = millis();
  }

  if (statusServo && (millis() - lidOpenTime >= servo_duration))
  {
    Serial.println("Waktu habis. Menutup tong...");
    closeLid();
  }

  // logika kapasitas tong sampah -- serial monitor
  static unsigned long lastCapacityCheck = 0;
  if (currentMillis - lastCapacityCheck >= 2000)  {
    lastCapacityCheck = currentMillis;
    float distance = sonar.ping_cm(); // fungsi baca jarak
    if (distance == 0) {
      distance = bin_capacity; // jika tidak ada echo, anggap jarak di luar kapasitas
    }

    currentCapacity = countCapacityPercentage(distance); // fungsi hitung kapasitas

    Serial.print("Status IR: ");
    Serial.print(digitalRead(ir_pin) == LOW ? "HALANGAN" : "AMAN");
    Serial.print(" | ");
    Serial.print("Jarak Kapasitas: ");
    Serial.print(distance);
    Serial.print(" cm | ");
    Serial.print("Kapasitas Penuh: ");
    Serial.print(currentCapacity);
    Serial.println(" %");

    if (distance <= bin_threshold)
    {
      Serial.println("Peringatan: Kapasitas tong sampah penuh!");
    }

    lastCapacityCheck = currentMillis;
  }
}

void connectToWiFi()
{
  Serial.print("Menghubungkan ke SSID: ");
  Serial.println(wifi_ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(wifi_ssid, wifi_password);

  unsigned long startAttemptTime = millis();
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    // Timeout setelah 10 detik
    if (millis() - startAttemptTime > 10000) {
      Serial.println("\nGagal terhubung ke WiFi. Periksa kredensial dan jangkauan sinyal.");
      return;
    }
  }

  Serial.println("\nTerhubung ke WiFi!");
  Serial.print("Alamat IP: ");
  Serial.println(WiFi.localIP());
}

// -- implementasi fungsi helper
void openLid()
{
  Serial.println("TANGAN TERDETEKSI! Membuka tutup...");
  motorServo.write(90); // buka 90 derajat
  statusServo = true;
}

void closeLid()
{
  Serial.println("Menutup tutup...");
  motorServo.write(0); // tutup
  statusServo = false;
}

float readDistance(int trigPin, int echoPin)
{
  // Pastikan trigger pin LOW
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  // Kirim pulsa HIGH selama 10 mikrodetik
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Baca durasi pulsa HIGH pada echo pin
  long duration = pulseIn(echoPin, HIGH);

  // Hitung jarak dalam cm (kecepatan suara ~34300 cm/s)
  float distance = (duration * 0.034) / 2; // dibagi 2 karena bolak-balik

  return distance;
}

int countCapacityPercentage(float distance)
{
  distance = constrain(distance, bin_threshold, bin_capacity); // batasi jarak
  float filledHeight = bin_capacity - distance; // tinggi terisi
  float measuredCapacity = bin_capacity - bin_threshold; // kapasitas terukur

  if (measuredCapacity <= 0)
    return 0; // hindari pembagian nol

  float capacityPercent = (filledHeight / measuredCapacity) * 100.0; // dalam persen

  return constrain((int)capacityPercent, 0, 100); // batasi antara 0-100%
}