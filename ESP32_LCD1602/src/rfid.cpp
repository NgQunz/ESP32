#include <rfid.h>
MFRC522 mfrc522(SS_PIN, RST_PIN); // Tạo đối tượng MFRC522

void rfid_setup()
{
    Serial.begin(115200); // Khởi động Serial
    SPI.begin();          // Khởi động SPI
    mfrc522.PCD_Init();   // Khởi động RC522
    Serial.println("Đặt thẻ RFID gần đầu đọc...");
}
void rfid_run()
{
    if (!mfrc522.PICC_IsNewCardPresent())
    {
        Serial.println("Không có thẻ mới.");
        delay(1000); // Đợi 1 giây trước khi kiểm tra lại
        return;
    }

    // Kiểm tra có thể đọc thẻ không
    if (!mfrc522.PICC_ReadCardSerial())
    {

        return;
    }

    // In UID thẻ
    Serial.print("UID của thẻ: ");
    for (byte i = 0; i < mfrc522.uid.size; i++)
    {
        if (mfrc522.uid.uidByte[i] < 0x10)
        {
            Serial.print("0");
        }
        Serial.print(mfrc522.uid.uidByte[i], HEX);
        Serial.print(" ");
    }
    Serial.println();
    delay(1000);
}