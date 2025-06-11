#include <SPI.h>
#include <MFRC522.h>

// Khai báo chân kết nối
#define SS_PIN 5   // SDA
#define RST_PIN 21 // RST

void rfid_setup();
void rfid_run(); // Hàm chạy RFID