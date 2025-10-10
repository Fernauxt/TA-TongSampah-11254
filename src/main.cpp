#include <Arduino.h>
#include <ESP32Servo.h>
#include <WiFi.h>
#include <FirebaseESP32.h>
#include <NewPing.h>
#include "config.h"

// -- objek global
Servo motorServo;
NewPing sonar(trigger_pin, echo_pin, bin_capacity); // jarak maksimal
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

// -- variabel global
unsigned long lidOpenTime = 0;
bool statusServo = false; // false = tutup, true = buka

// -- fungsi helper
// float readDistance(int trigPin, int echoPin); // dalam cm digantikan NewPing
int countCapacityPercentage(float distance); // dalam %
void openLid();
void closeLid();
void connectWiFi();

// ========== SETUP ==========
void setup()
{
  Serial.begin(115200);
  Serial.println("Inisialisasi Sistem Inti Tong Sampah Pintar...");
  delay(1000); // Beri jeda sedikit agar serial monitor siap

  // Inisialisasi pin sensor
  pinMode(ir_pin, INPUT_PULLUP); // infrared untuk deteksi tangan
  motorServo.attach(servo_pin);
  closeLid(); // pastikan tertutup saat mulai

  // Koneksi WiFi
  connectWiFi();
  config.database_url = firebase_url;
  config.signer.tokens.legacy_token = firebase_key;
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  Serial.println("=====================================");
  Serial.println("Setup Selesai. ESP32 siap.");
  Serial.println("Proyek Tong Sampah Pintar Dimulai!");
  Serial.println("=====================================");
}


// ========== LOOP UTAMA ==========
void loop()
{
  unsigned long currentMillis = millis();

  // cek koneksi WiFi
  if (WiFi.status() != WL_CONNECTED) {
    connectWiFi();
  }

  // logika buka/tutup
  if (digitalRead(ir_pin) == LOW && !statusServo) {
    openLid();
    lidOpenTime = currentMillis;
  }
  if (statusServo && (currentMillis - lidOpenTime >= servo_duration)) {
    closeLid();
  }

  // logika ukur kapasitas dan kirim data ke Firebase
  static unsigned long lastFirebaseUpdate = 0;
  if (currentMillis - lastFirebaseUpdate >= firebase_interval) {
    lastFirebaseUpdate = currentMillis;

    float distance = sonar.ping_cm(); // baca jarak dalam cm
    int capacityPercent = countCapacityPercentage(distance);
    String statusText;

    if (distance == 0) {
      // KASUS 1: Sensor gagal membaca (failsafe)
      capacityPercent = -1; // Nilai -1 sebagai penanda error
      statusText = "Sensor Error";
    } else {
      // KASUS 2: Sensor berhasil membaca
      // Jika jarak melebihi kapasitas, anggap saja kosong
      if (distance > bin_capacity) {
        distance = bin_capacity;
      }

      capacityPercent = countCapacityPercentage(distance);

      // Tentukan statusText berdasarkan persentase
      if (capacityPercent >= 95) {
        statusText = "Penuh";
      } else if (capacityPercent >= 85) {
        statusText = "Mendekati Penuh";
      } else if (capacityPercent >= 70) {
        statusText = "Hampir Penuh";
      } else if (capacityPercent >= 20) {
        statusText = "Terisi";
      } else {
        statusText = "Kosong";
      }
    }

    Serial.printf("Jarak : %.2f, Kapasitas: %d%%, Status: %s\n", distance, capacityPercent, statusText.c_str());
    Serial.print("Mengirim data ke Firebase... ");

    if (Firebase.ready()) {
      FirebaseJson json;
      json.set("capacity", capacityPercent);
      json.set("status", statusText);
      json.set("timestamp", ".sv");

      String devicePath = "/";
      devicePath += device_id;

      String current = devicePath;
      current += "/current";

      if (Firebase.setJSON(fbdo, current.c_str(), json)) {
        Serial.printf("Berhasil! Kapasitas: %d%%, Status: %s\n", capacityPercent, statusText.c_str());
      } else {
        Serial.print("Gagal kirim kapasitas: ");
        Serial.print(fbdo.errorReason());
      }

      String history = devicePath;
      history += "/history";
      
      if (Firebase.pushJSON(fbdo, history.c_str(), json)) {
        Serial.println("  -> Riwayat telah disimpan");
      } else {
        Serial.print("  -> Gagal simpan riwayat: ");
        Serial.print(fbdo.errorReason());
      }

    } else {
      Serial.print("Firebase tidak siap atau koneksi terputus");
    }
  }

  // // logika kapasitas tong sampah
  // static unsigned long lastCapacityCheck = 0;
  // if (millis() - lastCapacityCheck >= 2000) // cek setiap 2 detik
  // {
  //   float distance = readDistance(trigger_pin, echo_pin); // fungsi baca jarak

  //   int capacityPercent = countCapacityPercentage(distance); // fungsi hitung kapasitas

  //   if (handDetection == LOW)
  //   {
  //     Serial.print("HALANGAN");
  //   }
  //   else
  //   {
  //     Serial.print("AMAN");
  //   }
  //   Serial.print(" | "); // --> DIUBAH
  //   Serial.print("Jarak Kapasitas: ");
  //   Serial.print(distance);
  //   Serial.print(" cm | ");
  //   Serial.print("Kapasitas Penuh: ");
  //   Serial.print(capacityPercent);
  //   Serial.println(" %");

  //   if (distance <= bin_threshold)
  //   {
  //     Serial.println("Peringatan: Kapasitas tong sampah penuh!");
  //   }

  //   lastCapacityCheck = millis();
  // }
}

// -- implementasi fungsi helper
void openLid() {
  Serial.println("Tangan terdeteksi, membuka..");
  motorServo.write(90); // buka 90 derajat
  statusServo = true;
}

void closeLid() {
  Serial.println("Sudah terbuang, menutup...");
  motorServo.write(0); // tutup
  statusServo = false;
}

// float readDistance(int trigPin, int echoPin)
// {
//   // Pastikan trigger pin LOW
//   digitalWrite(trigPin, LOW);
//   delayMicroseconds(2);

//   // Kirim pulsa HIGH selama 10 mikrodetik
//   digitalWrite(trigPin, HIGH);
//   delayMicroseconds(10);
//   digitalWrite(trigPin, LOW);

//   // Baca durasi pulsa HIGH pada echo pin
//   long duration = pulseIn(echoPin, HIGH);

//   // Hitung jarak dalam cm (kecepatan suara ~34300 cm/s)
//   float distance = (duration * 0.034) / 2; // dibagi 2 karena bolak-balik

//   return distance;
// }

int countCapacityPercentage(float distance) {
  distance = constrain(distance, bin_threshold, bin_capacity); // batasi jarak
  float filledHeight = bin_capacity - distance; // tinggi terisi
  float measuredCapacity = bin_capacity - bin_threshold; // kapasitas terukur

  if (measuredCapacity <= 0)
    return 0; // hindari pembagian nol

  float capacityPercent = (filledHeight / measuredCapacity) * 100.0; // dalam persen

  return constrain((int)capacityPercent, 0, 100); // batasi antara 0-100%
}

void connectWiFi() {
  Serial.print("Menghubungkan ke WiFi: ");
  Serial.println(wifi_ssid);
  WiFi.begin(wifi_ssid, wifi_pass);

  int retryCount = 0;
  const int maxRetries = 20; // batasi percobaan koneksi

  while (WiFi.status() != WL_CONNECTED && retryCount < maxRetries) {
    delay(500);
    Serial.print(".");
    retryCount++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi terhubung.");
    Serial.print("Alamat IP: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\nGagal terhubung ke WiFi.");
  }
}