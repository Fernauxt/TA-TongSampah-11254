#include <Arduino.h>
#include <ESP32Servo.h>
#include "config.h"

// -- inisialisasi objek
Servo motorServo;
unsigned long lidOpenTime = 0;
bool statusServo = false; // false = tutup, true = buka

// -- fungsi helper
float readDistance(int trigPin, int echoPin); // dalam cm
int countCapacityPercentage(float distance); // dalam %
void openLid();
void closeLid();

// -- fungsi setup
void setup()
{
  // Memulai komunikasi serial pada kecepatan 115200 bps
  Serial.begin(115200);
  Serial.println("Inisialisasi Sistem Inti Tong Sampah Pintar...");

  // Beri jeda sedikit agar serial monitor siap
  delay(1000);

  // Inisialisasi pin sensor
  pinMode(trigger_pin, OUTPUT); // ultrasonik trigger
  pinMode(echo_pin, INPUT); // ultrasonik echo
  pinMode(ir_pin, INPUT_PULLUP); // infrared untuk deteksi tangan - aktivasi internal pull-down

  // Inisialisasi servo motor
  motorServo.attach(servo_pin);
  closeLid(); // pastikan tertutup saat mulai

  Serial.println("=====================================");
  Serial.println("Setup Selesai. ESP32 siap.");
  Serial.println("Proyek Tong Sampah Pintar Dimulai!");
  Serial.println("=====================================");
}

void loop()
{
  // Baca sensor tangan
  int handDetection = digitalRead(ir_pin);


  // logika buka tutup tong sampah
  if (handDetection == LOW && !statusServo)
  {
    Serial.println("Tangan terdeteksi! Membuka tutup...");
    openLid();
    lidOpenTime = millis();
  }

  if (statusServo && (millis() - lidOpenTime >= servo_duration))
  {
    Serial.println("Waktu habis. Menutup tutup...");
    closeLid();
  }

  // logika kapasitas tong sampah
  static unsigned long lastCapacityCheck = 0;
  if (millis() - lastCapacityCheck >= 2000) // cek setiap 2 detik
  {
    float distance = readDistance(trigger_pin, echo_pin); // fungsi baca jarak

    int capacityPercent = countCapacityPercentage(distance); // fungsi hitung kapasitas

    if (handDetection == LOW)
    {
      Serial.print("HALANGAN");
    }
    else
    {
      Serial.print("AMAN");
    }
    Serial.print(" | "); // --> DIUBAH
    Serial.print("Jarak Kapasitas: ");
    Serial.print(distance);
    Serial.print(" cm | ");
    Serial.print("Kapasitas Penuh: ");
    Serial.print(capacityPercent);
    Serial.println(" %");

    if (distance <= bin_threshold)
    {
      Serial.println("Peringatan: Kapasitas tong sampah penuh!");
    }

    lastCapacityCheck = millis();
  }
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