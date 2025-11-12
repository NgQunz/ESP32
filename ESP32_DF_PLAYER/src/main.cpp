#include <HardwareSerial.h>
#include <DFRobotDFPlayerMini.h>

// Sử dụng UART2 của ESP32 (UART2 có thể map ra nhiều chân)
HardwareSerial mySerial(2);
DFRobotDFPlayerMini myDFPlayer;

void setup()
{
  Serial.begin(9600);

  // Khởi tạo UART2: RX=16, TX=17
  mySerial.begin(9600, SERIAL_8N1, 16, 17);

  if (!myDFPlayer.begin(mySerial))
  {
    Serial.println("Không tìm thấy DFPlayer Mini!");
    while (true)
      ;
  }
  Serial.println("DFPlayer Mini sẵn sàng");

  myDFPlayer.volume(20); // Âm lượng 0~30
  myDFPlayer.play(1);    // Phát file 0001.mp3
}

void loop()
{
  if (myDFPlayer.available())
  {
    if (myDFPlayer.readType() == DFPlayerPlayFinished)
    {
      Serial.println("Phát xong, lặp lại...");
      myDFPlayer.play(1);
    }
  }
}
