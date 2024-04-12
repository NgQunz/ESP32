
/*
 * https://maniacbug.github.io/RF24/classRF24.html
 * 
 * VCC - 3.3v
 * GND - GND
 * CSN - 8
 * CE - 7
 * SCK - 13 mặc định SPI
 * MOSI - 11 SPI
 * MISO - 12 SPI
 */

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(7, 8); // CE, CSN
const byte diachi[6] = "12345"; //Mảng kí tự dạng chuỗi có 6 kí tự

void setup() 
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

void loop() 
{
  Serial.println("CHỜ KẾT NỐI.......");
  const char text[] = "Hello Green Technology"; //Mảng chứa chuỗi kí tự
  radio.write(&text, sizeof(text));
  // &: Trả lại địa chỉ của một biến.
  // sizeof: trả về số byte bộ nhớ của một biến 
  //hoặc là trả về tổng số byte bộ nhớ của một mảng
  Serial.println("đã gửi");
  delay(1000);
}
// TEST 3

// #include <SPI.h>
// #include <nRF24L01.h>
// #include <RF24.h>

// RF24 radio(7, 8); // CE, CSN
// const byte diachi[6] = "12345"; //Mảng kí tự dạng chuỗi có 6 kí tự

// int nutnhan = 6; boolean gtnutnhan = 0;
// int bientro = A0; int gtbientro; int gtbiendoi;

// byte mang[2]; //Mảng có 2 phần tử

// void setup() 
// {
//   Serial.begin(9600);

//   if (!radio.begin()) 
//   {
//     Serial.println("Module không khởi động được...!!");
//     while (1) {}
//   }   
//   radio.openWritingPipe(diachi);
//   radio.setPALevel(RF24_PA_MIN); //Cài bộ khuyết địa công suất ở mức MIN
//   radio.setChannel(80);
//   radio.setDataRate(RF24_250KBPS);  
//   radio.stopListening(); //Cài đặt module là TX
//   if (!radio.available())
//   {
//     Serial.println("Chưa kết nối được với RX...!!");
//     Serial.println("CHỜ KẾT NỐI.......");
//   }

//   pinMode(nutnhan, INPUT_PULLUP);
//   pinMode(bientro, INPUT);
// }

// void loop() 
// {
//   gtbientro = analogRead(bientro);
//   gtbiendoi = map(gtbientro, 0, 1023, 0, 180);
//   mang[0] = gtbiendoi; //Ghi gtbiendoi vào phần tử 0
//   Serial.print("Góc Servo: "); Serial.print(gtbiendoi); Serial.print("   ");
  
//   gtnutnhan = digitalRead(nutnhan);
//   mang[1] = gtnutnhan;
//   Serial.print("Giá Trị nút nhấn: "); Serial.println(gtnutnhan);

//   radio.write(&mang, sizeof(mang));
  
//   delay(50);
// }
 
// TEST 4

// #include <SPI.h>
// #include <nRF24L01.h>
// #include <RF24.h>

// RF24 radio(7, 8); // CE, CSN
// const byte diachi[][6] = {"12345", "10000"}; //0, 1

// int led = 6;
// boolean nutnhan = 0;
// int bientro = A0; int gtbientro; int gtbiendoi;

// void setup() 
// {
//   Serial.begin(9600);

//   if (!radio.begin()) 
//   {
//     Serial.println("Module không khởi động được...!!");
//     while (1) {}
//   }   
//   radio.openWritingPipe(diachi[1]);
//   //Chỉ có thể mở 1 đường ghi
//   //Lệnh openWritingPipe có số đường truyền mặc định là 0
//   //Mở 1 kênh có địa chỉ 10000 trên đường truyền 0
//   //kênh này chỉ ghi data trên địa chỉ 10000   
//   radio.openReadingPipe(1, diachi[0]);
//   //Có thể mở 6 đường đọc cùng lúc
//   //Nhưng đường 0 mặc định dùng cho ghi
//   //Lệnh openReadingPipe có thể mở đường truyền từ 1-5
//   //Đọc data của địa chỉ 12345 trên đường truyền 1
     
//   radio.setPALevel(RF24_PA_MIN); //Cài bộ khuyết địa công suất ở mức MIN
//   radio.setChannel(80);
//   radio.setDataRate(RF24_250KBPS);    
//   if (!radio.available())
//   {
//     Serial.println("Chưa kết nối được với RX...!!");
//     Serial.println("CHỜ KẾT NỐI.......");
//   }

//   pinMode(led, OUTPUT);
//   pinMode(bientro, INPUT);
// }

// void loop() 
// {
//   radio.stopListening(); //Ngưng nhận
//     gtbientro = analogRead(bientro);
//     gtbiendoi = map(gtbientro, 0, 1023, 0, 180);
//     radio.write(&gtbiendoi, sizeof(gtbiendoi));
//     Serial.print("Biến trở gửi: "); Serial.print(gtbiendoi); Serial.print("   ");
//     delay(10);
      
//   radio.startListening(); //Bắt đầu nhận
//     while(!radio.available());
//     radio.read(&nutnhan, sizeof(nutnhan));
//     Serial.print("Nhận nút nhấn: "); Serial.println(nutnhan);
//     if (nutnhan == HIGH)
//      {
//        digitalWrite(led, HIGH);
//      }
//     else
//      {
//        digitalWrite(led, LOW);
//      }
//     delay(10);  
// }

// TEST LED

// #include <SPI.h>
// #include <nRF24L01.h>
// #include <RF24.h>

// RF24 radio(7, 8); // CE, CSN
// const byte diachi[6] = "12345"; //Mảng kí tự dạng chuỗi có 6 kí tự

// int nutnhan = 6; boolean gtnutnhan = 0;

// void setup() 
// {
//   Serial.begin(9600);

//   if (!radio.begin()) 
//   {
//     Serial.println("Module không khởi động được...!!");
//     while (1) {}
//   }   
//   radio.openWritingPipe(diachi); 
//   //Lệnh openWritingPipe mặc định là đường truyền 0
//   //mở 1 kênh có địa chỉ 12345 trên đường truyền 0
//   // kênh này ghi data lên địa chỉ 12345  
//   radio.setPALevel(RF24_PA_MIN); //Cài bộ khuyết đại công suất ở mức MIN
//   radio.setChannel(80);
//   radio.setDataRate(RF24_250KBPS);
//   radio.stopListening(); //Cài đặt module là TX
//   if (!radio.available())
//   {
//     Serial.println("Chưa kết nối được với RX...!!");
//     Serial.println("CHỜ KẾT NỐI.......");
//   }

//   pinMode(nutnhan, INPUT_PULLUP);
// }

// void loop() 
// {
//   gtnutnhan = digitalRead(nutnhan);
//   Serial.print("Giá Trị nút nhấn: "); Serial.println(gtnutnhan);
//   radio.write(&gtnutnhan, sizeof(gtnutnhan));
//   delay(500);
// }
