#include <Arduino.h>

void setup()
{
  // Memulai komunikasi serial pada kecepatan 115200 bps
  Serial.begin(115200);

  // Beri jeda sedikit agar serial monitor siap
  delay(1000);

  Serial.println("=====================================");
  Serial.println("Setup Selesai. ESP32 siap.");
  Serial.println("Proyek Tong Sampah Pintar Dimulai!");
  Serial.println("=====================================");
}

void loop()
{
  // Biarkan kosong untuk saat ini
}