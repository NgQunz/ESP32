#include <header.h>

SimpleKalmanFilter bo_loc(2, 2, 0.001);
RF24 radio(16, 5); // CE, CSN
char Buffer[1000];
const byte diachi[6] = "12345"; //Mảng kí tự dạng chuỗi có 6 kí tự
// Khai báo các chân kết nối joystick
#define URx1  33 // Chân URX
#define URy1  32 // Chân URY
#define SW_Pick  25  // Chân SW
#define URX_Motor  27 // Chân URX
#define URY_Motor  26 // Chân URY
#define POTEN_PIN 15
//1 x=0 tại x khoảng -5 đến -20 
//1 y từ -10 từ 10
int xValue_Pick;
int yValue_Pick;
int BTN1;
int x_Motor;
int y_Motor;
int POT;
void TX_SETUP() ;
void JOYSTICK_SETUP();
void Joystck_Read();
void TX_TRANS() ;
void POTEN_SETUP();
int POTEN_RUN();
void Data_to_Json(int x1, int y1, int BTN1, int xValue_Moto, int y2, int POT);
void setup()
{
  POTEN_SETUP();
  TX_SETUP();
  JOYSTICK_SETUP();
}
void loop(){

  TX_TRANS();
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
  Data_to_Json(xValue_Pick, yValue_Pick, BTN1, x_Motor, y_Motor, POT);
  radio.write(&Buffer, sizeof(&Buffer));
  Serial.println("Đã gửi dữ liệu");
  // Tiếp tục xử lý hoặc gửi các giá trị này
  delay(1000);
}
void Joystck_Read() {
  xValue_Pick = map(analogRead(URx1), 0, 4095, -255, 255);
  yValue_Pick = map(analogRead(URy1), 0, 4095, -255, 255);
  BTN1 = digitalRead(SW_Pick);
  x_Motor = map(analogRead(URX_Motor), 0, 4095, -255, 255);
  y_Motor = map(analogRead(URY_Motor), 0, 4095, -255, 255);
  POT= POTEN_RUN();
  //Serial.printf("%d %d %d %d %d\n",x1,y1, BTN1,x_Motor, y_Motor);
}
void JOYSTICK_SETUP() {
  // Khởi tạo Serial Monitor
  Serial.begin(9600);
  // Khởi tạo chân đầu vào
  pinMode(URx1, INPUT);
  pinMode(URy1, INPUT);
  pinMode(SW_Pick, INPUT_PULLUP); // Chân SW với chế độ INPUT_PULLUP
  pinMode(URX_Motor, INPUT);
  pinMode(URY_Motor, INPUT);
}
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
void Data_to_Json(int x1, int y1, int BTN1, int x2, int y2, int POT)
{
  int Length_Buffer;
  String ESP32_Data = "{\"x1\":\"" + String(x1) + "\"," + "\"y1\":\"" + String(y1) + "\"," +
                       "\"BTN1\":\"" + String(BTN1) + "\"," + "\"x2\":\"" + String(x2) + "\","+ 
                       "\"y2\":\"" + String(y2) + "\"," + "\"POT\":\"" + String(POT) + "\"}";
           
 Length_Buffer = ESP32_Data.length()+1;
 ESP32_Data.toCharArray(Buffer,Length_Buffer);
}
