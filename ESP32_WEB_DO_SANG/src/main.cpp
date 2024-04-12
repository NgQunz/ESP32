#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
int a[]={4,16,17,18,19,27,32,22};
String outputState()
{
  for (int i=0;i<8;i++)
    {if(digitalRead(a[i]))
    {
      return "checked";
    }
    else
    {
      return "";
    }
  }
}
// Replace with your network credentials
const char* ssid = "ESP32";
const char* password = "12345678";
const char* PARAM_INPUT_2 = "state";

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

const char index_html[] PROGMEM = R"nquan(
<!DOCTYPE HTML><html>
<head>
  <title>ESP Web Server</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="icon" href="data:,">
  <style>
    html {font-family: Arial; display: inline-block; text-align: center;}
    h2 {font-size: 3.0rem;}
    p {font-size: 3.0rem;}
    body {max-width: 600px; margin:0px auto; padding-bottom: 25px;}
    .switch {position: relative; display: inline-block; width: 120px; height: 68px} 
    .switch input {display: none}
    .slider {position: absolute; top: 0; left: 0; right: 0; bottom: 0; background-color: #ccc; border-radius: 6px}
    .slider:before {position: absolute; content: ""; height: 52px; width: 52px; left: 8px; bottom: 8px; background-color: #fff; -webkit-transition: .4s; transition: .4s; border-radius: 3px}
    input:checked+.slider {background-color: #b30000}
    input:checked+.slider:before {-webkit-transform: translateX(52px); -ms-transform: translateX(52px); transform: translateX(52px)}
  </style>
</head>
<body>
  <h2>ESP Web Server</h2>
  %BUTTONPLACEHOLDER%
<script>function toggleCheckbox(element) {
  var xhr = new XMLHttpRequest();
  if(element.checked){ xhr.open("GET", "/update?output="+element.id+"&state=0", true); }
  else { xhr.open("GET", "/update?output="+element.id+"&state=1", true); }
  xhr.send();
}
</script>
</body>
</html>
)nquan";

// Replaces placeholder with button section in your web page
String processor(const String& var){
  //Serial.println(var);
  "<h4>Output - GPIO </h4><label class=\"switch\"><input type=\"checkbox\" onchange=\"toggleCheckbox(this)\" id=\"4\" ";
  if(var == "BUTTONPLACEHOLDER"){
    String buttons = "";
    //for (int i=0;i<8;i++)
    buttons += "<h4>Output - GPIO </h4><label class=\"switch\"><input type=\"checkbox\" onchange=\"toggleCheckbox(this)\" id=\"4\" " + outputState() + "><span class=\"slider\"></span></label>";
    return buttons;
  }
  return String();
}
void setup(){
  // Serial port for debugging purposes
  Serial.begin(115200);
  
  for (int i=0;i<8;i++)
  {
    pinMode(a[i], OUTPUT);
    digitalWrite(a[i],1);
  }
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  // Print ESP Local IP Address
  Serial.println(WiFi.localIP());

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });

  // Send a GET request to <ESP_IP>/update?output=<inputMessage1>&state=<inputMessage2>
  server.on("/update", HTTP_GET, [] (AsyncWebServerRequest *request) {
    String inputMessage2;
    // GET input1 value on <ESP_IP>/update?output=<inputMessage1>&state=<inputMessage2>
    for (int i=0;i<8;i++)
    { if (request->hasParam(PARAM_INPUT_2)) 
    {
        inputMessage2 = request->getParam(PARAM_INPUT_2)->value();
        //digitalWrite(a[i], inputMessage2.toInt());
        sliderValue = inputMessage;
        ledcWrite(ledChannel, sliderValue.toInt());
      }
      else {
        inputMessage2 = "No message sent";
      }
    }
    Serial.print("GPIO: ");
    Serial.print(" - Set to: ");
    Serial.println(inputMessage2);
    request->send(200, "text/plain", "OK");
  });

  // Start server
  server.begin();
}

void loop() {

}