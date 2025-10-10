#pragma once

// -- konfig wifi --
const char *wifi_ssid = "ADVAN V1 PRO-8A105E";
const char *wifi_pass = "R1caGorengzhu";

// -- konfig firebase --
#define firebase_url "https://smart-trash-system-v54-default-rtdb.asia-southeast1.firebasedatabase.app/"
#define firebase_key "Jnf7Zykov76fdiZg8UQaHejGgVvgQgH5ehNX18z8"

// -- konfig perangkat --
#define device_id "smartbin-01"

// -- konfig pin sensor dan aktuator --
const int servo_pin = 13; // pin servo motor
const int trigger_pin = 12; // pin trigger sensor ultrasonik
const int echo_pin = 14; // pin echo sensor ultrasonik
const int ir_pin = 27; // pin sensor infrared

// -- konfig fisik dan logika --
const unsigned long servo_duration = 5000; // dalam ms (5 detik)
const float bin_capacity = 50.0; // tinggi fisik tong sampah (cm)
const float bin_threshold = 5.0; // batas maksimal jarak untuk deteksi penuh (cm)
const unsigned long firebase_interval = 10000; // interval kirim data ke firebase (ms)