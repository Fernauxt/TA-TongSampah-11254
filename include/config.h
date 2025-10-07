#pragma once

// -- konfig pin sensor dan aktuator --

const int pin_servo = 13; // pin servo motor

const int pin_trigger_kapasitas = 12; // pin trigger sensor ultrasonik
const int pin_echo_kapasitas = 14; // pin echo sensor ultrasonik

// -- konfig fisik dan logika --

const int jarak_deteksi_tangan = 15; // dalam cm

const unsigned long durasi_servo = 5000; // dalam ms (5 detik)

const float kapasitas_tong = 50.0; // kalibrasi kapasitas tong dalam liter
const float batas_tong = 5.0; // dalam liter, batas maksimal