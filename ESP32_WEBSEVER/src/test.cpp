// // #include "WiFi.h"
// // #include "ESPAsyncWebServer.h"
// // #include "SPIFFS.h"
// // #include "index_html.h"
// // #include "xu_ly.h"
// // #define LED 4 // chân xuất tín hiệu ra relay
// // AsyncWebServer server(80); // port 80 
// // int flag_led = 0; // gắn cờ tắt/bật đèn
// // int flag_manual = 0; // gắn cờ chế độ manual/auto

// // //con esp32 sẽ kết nối vào wifi này,
// // // khuyến khích bắt vào wifi do chính laptop của mình phát ra
// // const char* ssid = "ESP32;
// // const char* password = "12345678";
// // String processor(const String& var); // hàm xử lý, đây là 1 phần để kết nối với JavaScript

// // void setup() 
// // {
// //   Serial.begin(115200); // khai báo tốc độ giao tiếp
// //   pinMode(LED, OUTPUT); // khai báo chân LED: OUTPUT
// //   digitalWrite(LED, 1); // để khi vừa kết nối đèn tắt
// //   xu_ly.begin(); // begin xu_ly
// //   if(!SPIFFS.begin(true)) // kiểm tra trang thái hoạt động của SPIFFS
// //   {
// //     Serial.println("An Error has occurred while mounting SPIFFS");
// //     return;
// //   }
// //   WiFi.begin(ssid, password); // kết nối wifi
// //   while (WiFi.status() != WL_CONNECTED) // kiểm tra trạng thái kết nối wifi
// //   {
// //     delay(1000);
// //     Serial.println("Connecting to WiFi..");
// //   }
// //   Serial.println(WiFi.localIP()); // in ra địa chỉ IP
// //   //làm đường dẫn bật đèn thủ công
// //   server.on("/manual/on", HTTP_GET, [](AsyncWebServerRequest *request)
// //   {
// //       flag_led = 1; //đèn bật
// //       flag_manual = 1; // chuyển sáng chế độ manual
// //     request->send(200, "text/html", index_html, processor);
// //   });
// //   // làm đường dẫn tắt đèn thủ công
// //   server.on("/manual/off", HTTP_GET, [](AsyncWebServerRequest *request)
// //   {
// //       flag_led = 0; // đèn tắt
// //       flag_manual = 1; // chế độ manual
// //     request->send(200, "text/html", index_html, processor);
// //   }); 
// //   server.begin(); // khởi chạy "server" ( cái khai báo ở chỗ port 80)
// // }
// // void loop() {
// //   if ( flag_manual == 1 && flag_led == 0) //manual và ấn vào nút "OFF"
// //     {
// //       digitalWrite(LED, 1); // tắt đèn
// //     }
// //   else if ( flag_manual == 1 && flag_led == 1) // manual và ấn vào nút "ON"
// //     {
// //       digitalWrite(LED, 0); // bật đèn 
// //     }
// //   delay(1000); // cứ 1s thì thực hiện vòng xử lý trên 1 lần
// // }

// // //nguyên mẫu hàm đã khai báo ở trên
// // String processor(const String& var)
// // {
// //   if(var == "TEMPERATURE")
// //   {
// //     return readDHTTemperature();
// //   }
// //   else if(var == "HUMIDITY")
// //   {
// //     return readDHTHumidity();
// //   }
// //   return String();
// // }
// /*********
//   Rui Santos
//   Complete project details at https://RandomNerdTutorials.com/esp32-async-web-server-espasyncwebserver-library/
//   The above copyright notice and this permission notice shall be included in all
//   copies or substantial portions of the Software.
// *********/

// // Import required libraries
// #include <WiFi.h>
// #include <AsyncTCP.h>
// #include <ESPAsyncWebServer.h>
// int a[]={4,16,17,18,19,27,32};
// String outputState(int a[])
// {
//   for (int i=0;i<7;i++)
//   {if(digitalRead(a[i]))
//   {
//     return "checked";
//   }
//   else 
//   {
//     return "";
//   }}
// }
// // Replace with your network credentials
// const char* ssid = "ESP32";
// const char* password = "12345678";

// const char* PARAM_INPUT_1 = "output";
// const char* PARAM_INPUT_2 = "state";

// // Create AsyncWebServer object on port 80
// AsyncWebServer server(80);

// const char index_html[] PROGMEM = R"nquan(
// <!DOCTYPE HTML><html>
// <head>
//   <title>ESP Web Server</title>
//   <meta name="viewport" content="width=device-width, initial-scale=1">
//   <link rel="icon" href="data:,">
//   <style>
//     html {font-family: Arial; display: inline-block; text-align: center;}
//     h2 {font-size: 3.0rem;}
//     p {font-size: 3.0rem;}
//     body {max-width: 600px; margin:0px auto; padding-bottom: 25px;}
//     .switch {position: relative; display: inline-block; width: 120px; height: 68px} 
//     .switch input {display: none}
//     .slider {position: absolute; top: 0; left: 0; right: 0; bottom: 0; background-color: #ccc; border-radius: 6px}
//     .slider:before {position: absolute; content: ""; height: 52px; width: 52px; left: 8px; bottom: 8px; background-color: #fff; -webkit-transition: .4s; transition: .4s; border-radius: 3px}
//     input:checked+.slider {background-color: #b30000}
//     input:checked+.slider:before {-webkit-transform: translateX(52px); -ms-transform: translateX(52px); transform: translateX(52px)}
//   </style>
// </head>
// <body>
//   <h2>ESP Web Server</h2>
//   %BUTTONPLACEHOLDER%
// <script>function toggleCheckbox(element) {
//   var xhr = new XMLHttpRequest();
//   if(element.checked){ xhr.open("GET", "/update?output="+element.id+"&state=0", true); }
//   else { xhr.open("GET", "/update?output="+element.id+"&state=1", true); }
//   xhr.send();
// }
// </script>
// </body>
// </html>
// )nquan";

// // Replaces placeholder with button section in your web page
// String processor(const String& var){
//   //Serial.println(var);
//   if(var == "BUTTONPLACEHOLDER"){
//     String buttons = "";
//     //for (int i=0;i<7;i++)
//     buttons += "<h4>Output - GPIO 4</h4><label class=\"switch\"><input type=\"checkbox\" onchange=\"toggleCheckbox(this)\" id=\"4\" " + outputState(a) + "><span class=\"slider\"></span></label>";
//     return buttons;
//   }
//   return String();
// }
// void setup(){
//   // Serial port for debugging purposes
//   Serial.begin(115200);
  
//   for (int i=0;i<7;i++)
//   {
//     pinMode(a[i], OUTPUT);
//     digitalWrite(a[i],0);
//   }
//   // Connect to Wi-Fi
//   WiFi.begin(ssid, password);
//   while (WiFi.status() != WL_CONNECTED) {
//     delay(1000);
//     Serial.println("Connecting to WiFi..");
//   }

//   // Print ESP Local IP Address
//   Serial.println(WiFi.localIP());

//   // Route for root / web page
//   server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
//     request->send_P(200, "text/html", index_html, processor);
//   });

//   // Send a GET request to <ESP_IP>/update?output=<inputMessage1>&state=<inputMessage2>
//   server.on("/update", HTTP_GET, [] (AsyncWebServerRequest *request) {
//     String inputMessage1;
//     String inputMessage2;
//     // GET input1 value on <ESP_IP>/update?output=<inputMessage1>&state=<inputMessage2>
//     for (int i=0;i<7;i++)
//    { if (request->hasParam(PARAM_INPUT_1) && request->hasParam(PARAM_INPUT_2)) {
//       inputMessage1 = request->getParam(PARAM_INPUT_1)->value();
//       inputMessage2 = request->getParam(PARAM_INPUT_2)->value();
//       digitalWrite(a[i], inputMessage2.toInt());
//     }
//     else {
//       inputMessage1 = "No message sent";
//       inputMessage2 = "No message sent";
//     }}
//     Serial.print("GPIO: ");
//     Serial.print(inputMessage1);
//     Serial.print(" - Set to: ");
//     Serial.println(inputMessage2);
//     request->send(200, "text/plain", "OK");
//   });

//   // Start server
//   server.begin();
// }

// void loop() {

// }