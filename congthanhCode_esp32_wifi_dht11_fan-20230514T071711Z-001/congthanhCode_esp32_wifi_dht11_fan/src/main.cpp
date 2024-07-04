#include "WiFi.h"
#include "ESPAsyncWebServer.h"
#include "SPIFFS.h"
#include "index_html.h"
#include "dht11.h"

#define PINFAN 4 // chân xuất tín hiệu ra relay

AsyncWebServer server(80); // port 80 
int flag_fan = 0; // gắn cờ tắt/bật quạt
int flag_manual = 0; // gắn cờ chế độ manual/auto

//con esp32 sẽ kết nối vào wifi này,
// khuyến khích bắt vào wifi do chính laptop của mình phát ra
const char* ssid = "esp32_thanh";
const char* password = "tamsonam";
String processor(const String& var); // hàm xử lý, đây là 1 phần để kết nối với JavaScript

void setup() 
{
  Serial.begin(115200); // khai báo tốc độ giao tiếp
  pinMode(PINFAN, OUTPUT); // khai báo chân quạt: OUTPUT
  digitalWrite(PINFAN, HIGH); // để khi vừa kết nối quạt tắt
  dht.begin(); // begin dht11
  if(!SPIFFS.begin(true)) // kiểm tra trang thái hoạt động của SPIFFS
  {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }
  WiFi.begin(ssid, password); // kết nối wifi
  while (WiFi.status() != WL_CONNECTED) // kiểm tra trạng thái kết nối wifi
  {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println(WiFi.localIP()); // in ra địa chỉ IP
  // đường dẫn khi mở web, ấn f12 để hiểu hơn về hàm
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    flag_manual = 0; // mặc định quạt chạy ở chế độ auto
    request->send_P(200, "text/html", index_html, processor);
  });
  // lấy thông tin nhiệt độ
  server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    request->send_P(200, "text/plain", readDHTTemperature().c_str());
  });
  // lấy thông tin độ ẩm
  server.on("/humidity", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    request->send_P(200, "text/plain", readDHTHumidity().c_str());
  });
  // làm đường dẫn để quạt ở chế độ auto, cái này giống hệt đường dẫn mặc định ("/")
  server.on("/auto", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    flag_manual = 0;
    request->send_P(200, "text/html", index_html, processor);
  }); 
  //làm đường dẫn bật quạt thủ công
  server.on("/manual/on", HTTP_GET, [](AsyncWebServerRequest *request)
  {
      flag_fan = 1; //quạt bật
      flag_manual = 1; // chuyển sáng chế độ manual
    request->send_P(200, "text/html", index_html, processor);
  });
  // làm đường dẫn tắt quạt thủ công
  server.on("/manual/off", HTTP_GET, [](AsyncWebServerRequest *request)
  {
      flag_fan = 0; // quạt tắt
      flag_manual = 1; // chế độ manual
    request->send_P(200, "text/html", index_html, processor);
  }); 
  server.begin(); // khởi chạy "server" ( cái khai báo ở chỗ port 80)
}

void loop() {
  // lấy thông tin vaf in ra nhiệt độ, độ ẩm
  float t = dht.readTemperature();
  float h = dht.readHumidity();
  Serial.print(t);
  Serial.println("*C");
  Serial.print(h);
  Serial.println("%");
  Serial.println();
  if ( flag_manual == 0 && t > 30) // chế độ auto và nhiệt độ trên 30 độ
  {
    digitalWrite(PINFAN, LOW); //bật quạt
  }
  else if (flag_manual == 0 && t <= 30) // chế độ auto và nhiệt độ dưới 30 độ
  {
    digitalWrite(PINFAN, HIGH); // tắt quạt
  }
  else if ( flag_manual == 1 && flag_fan == 0) //manual và ấn vào nút "OFF"
    {
      digitalWrite(PINFAN, HIGH); // tắt quạt
    }
  else if ( flag_manual == 1 && flag_fan == 1) // manual và ấn vào nút "ON"
    {
      digitalWrite(PINFAN, LOW); // bật quạt 
    }
  delay(1000); // cứ 1s thì thực hiện vòng xử lý trên 1 lần
}

//nguyên mẫu hàm đã khai báo ở trên
String processor(const String& var)
{
  if(var == "TEMPERATURE")
  {
    return readDHTTemperature();
  }
  else if(var == "HUMIDITY")
  {
    return readDHTHumidity();
  }
  return String();
}
