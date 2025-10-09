#pragma once

// -- indeks perangkat sebagai id unik --
// #define device_index 1 // ganti dengan indeks unik untuk setiap perangkat (misal: 1, 2, 3, ...)

// -- konfig pin sensor dan aktuator --
const int servo_pin = 13; // pin servo motor
const int trigger_pin = 12; // pin trigger sensor ultrasonik
const int echo_pin = 14; // pin echo sensor ultrasonik
const int ir_pin = 27; // pin sensor infrared

// -- konfig fisik dan logika --
const unsigned long servo_duration = 5000; // dalam ms (5 detik)
const float bin_capacity = 50.0; // tinggi fisik tong sampah (cm)
const float bin_threshold = 5.0; // batas maksimal jarak (cm) untuk deteksi penuh
const unsigned long data_send_interval = 10000; // interval pengiriman data ke server (ms) - 10 detik
const unsigned long sensor_read_interval = 5000; // interval pembacaan sensor (ms) - 5 detik

// -- konfig backend --

// kredensial WiFi
const char *wifi_ssid = "ADVAN V1 PRO-8A105E"; // ssid wifi
const char *wifi_password = "R1caGorengzhu";  // password wifi

// kredensial Thinger.io
#define thinger_username "michaelwk_"          // username Thinger.io
#define thinger_device_id "ta-sampah-11254"   // device id Thinger.io
#define thinger_device_credential "210711254" // kredensial device Thinger.io