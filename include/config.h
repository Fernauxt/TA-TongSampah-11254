#pragma once

// -- konfig pin sensor dan aktuator --

const int servo_pin = 13; // pin servo motor

const int trigger_pin = 12; // pin trigger sensor ultrasonik
const int echo_pin = 14; // pin echo sensor ultrasonik

const int ir_pin = 27; // pin sensor infrared

// -- konfig fisik dan logika --
const unsigned long servo_duration = 5000; // dalam ms (5 detik)

const float bin_capacity = 50.0; // tinggi fisik tong sampah (cm)
const float bin_threshold = 5.0; // batas maksimal jarak (cm) untuk deteksi penuh