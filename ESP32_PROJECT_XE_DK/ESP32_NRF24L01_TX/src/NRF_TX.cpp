#include <header.h>

SimpleKalmanFilter bo_loc(2, 2, 0.001);
RF24 radio(16, 5); // CE, CSN
const byte diachi[6] = "12345"; //Mảng kí tự dạng chuỗi có 6 kí tự

void POTEN_SETUP()
{
  pinMode(POTEN_PIN, INPUT);
}
int POTEN_RUN()
{
  int a= analogRead(POTEN_PIN);
  for (int i=0;i<4;i++)
  {
    a= bo_loc.updateEstimate(a);
  }
  a= map(a,0, 4095,0, 180);
  return a;
}
void TX_SETUP() 
{
  Serial.begin(9600);
  if (!radio.begin()) 
  {
    Serial.println("Module không khởi động được...!!");
    while (1) {}
  } 
  radio.openWritingPipe(diachi);
  //Lệnh openWritingPipe mặc định là đường truyền 0
  //mở 1 kênh có địa chỉ 12345 trên đường truyền 0
  // kênh này ghi data lên địa chỉ 12345  
  radio.setPALevel(RF24_PA_MIN); 
  //Cài bộ khuyết địa công suất ở mức MIN, MAX, HIGH, LOW
  radio.setChannel(80); // 125 kênh từ 0-124; TX và RX phải cùng kênh
                        // 2.4GHz ~ 2400Mhz, bước kênh là 1MHz
                        // setchannel(1) => 2401Mhz
                        // Cao nhất là 2525MHz, Tức là 2.525GHz
                        
  radio.setDataRate(RF24_250KBPS); //Tốc độ truyền dữ liệu trong không khí
                                   //250kbps, 1Mbps hoặc 2Mbps
                                   //250 thấp nhất nhưng truyền xa, 1Mb và 2Mb mạnh nhưng truyền không xa                                   
  /*                                   
   * Tốc độ truyền dữ liệu không khí 2Mbps, băng thông 2MHz bị chiếm dụng nhiều tần số kênh 
   * rộng hơn độ phân giải của cài đặt tần số kênh RF
   * Vì vậy, để đảm bảo các kênh không chồng chéo và giảm kết nối chéo ở chế độ 2Mbps
   * bạn cần giữ khoảng cách 2MHz giữa hai kênh.                                    
   * 
   * A: Xe TX-RX kênh 80, tốc độ truyền là 2Mb 80, 81, 82
   * B: Máy bay TX-RX kênh 83, tốc độ truyền là 250Kb                                    
   */
  radio.stopListening(); //Cài đặt module là TX

  if (!radio.available())
  {
    Serial.println("Chưa kết nối được với RX...!!");
    Serial.println("CHỜ KẾT NỐI.......");
  }  
}
void TX_TRANS() {
  JoystickData joystickData= VALUE_JOYSTICK();
  radio.write(&joystickData, sizeof(joystickData));
  Serial.println("Đã gửi dữ liệu");
  // Tiếp tục xử lý hoặc gửi các giá trị này
}
JoystickData VALUE_JOYSTICK() {
  JoystickData data;

  data.x_Pick_Up = analogRead(URX_Pick_Up);
  data.y_Pick_Up = analogRead(URY_Pick_Up);
  data.switchState_Pick_Up = digitalRead(SW_Pick_Up);
  //data.Poten= POTEN_RUN();
  int xValue_Control_Motor = analogRead(URX_Control_Motor);
  int yValue_Control_Motor = analogRead(URY_Control_Motor);
  data.x_Control_Motor = map(xValue_Control_Motor, 0, 4095, -255, 255);
  data.y_Control_Motor = map(yValue_Control_Motor, 0, 4095, -255, 255);
  //Serial.printf("%d %d %d %d %d\n",data.x_Pick_Up,data.y_Pick_Up, data.switchState_Pick_Up,data.x_Control_Motor, data.y_Control_Motor);
  return data;
}
void JOYSTICK_SETUP() {
  // Khởi tạo Serial Monitor
  Serial.begin(9600);
  // Khởi tạo chân đầu vào
  pinMode(URX_Pick_Up, INPUT);
  pinMode(URY_Pick_Up, INPUT);
  pinMode(SW_Pick_Up, INPUT_PULLUP); // Chân SW với chế độ INPUT_PULLUP
  pinMode(URX_Control_Motor, INPUT);
  pinMode(URY_Control_Motor, INPUT);

}