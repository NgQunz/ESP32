#include <header.h>

RF24 radio(16, 5); // CE, CSN
const byte diachi[6] = "12345";

void NRF_RX_SETUP()
{
  Serial.begin(9600);
  
  if (!radio.begin()) 
  {
    Serial.println("Module không khởi động được...!!");
    while (1) {}
  } 
  
Serial.begin(9600);  radio.openReadingPipe(1, diachi);
  //Lệnh openReadingPipe có số đường truyền từ 0-5
  //Nhưng đường 0 đã được dùng cho ghi (mặc định)
  //Vì vậy chỉ dùng 1-5, nếu dùng không sẽ bị chồng lấn
  //Mở 1 kênh có địa chỉ 12345 trên đường truyền 1
  //kênh này chỉ đọc data trên địa chỉ 12345   
  radio.setPALevel(RF24_PA_MIN);
  radio.setChannel(120);
  radio.setDataRate(RF24_250KBPS);
  radio.startListening();
  
  if (!radio.available())
  {
    Serial.println("Chưa kết nối được với TX...!!");
    Serial.println("CHỜ KẾT NỐI.......");
  } 
}
void RUN_RX()
{
  //Serial.begin(9600);
  if (flag_tx==1)
  {
    //JoystickData receivedData;
    if (radio.available()) {
      radio.read(&receivedData, sizeof(receivedData));
      Serial.print("Received Data - X: ");
      receivedData.x_Pick_Up=-receivedData.x_Pick_Up;
      Serial.print(receivedData.x_Pick_Up);
      Serial.print(", Y: ");
      Serial.print(receivedData.y_Pick_Up);
      Serial.print(", Switch: ");
      Serial.println(receivedData.switchState_Pick_Up);
      Serial.print("X1: ");
      receivedData.x_Control_Motor=-receivedData.x_Control_Motor;
      Serial.println(receivedData.x_Control_Motor);
      Serial.print("Y1");
      Serial.println(receivedData.y_Control_Motor);
      Serial.print("POTEN");
      Serial.println(receivedData.Poten);

    }
    flag_tx=0;
  }
}
