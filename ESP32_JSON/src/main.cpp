
// #include <ArduinoJson.h>
// #include <PubSubClient.h>
// #include <WiFi.h>
// #include <NTPClient.h>
// #include <WiFiUdp.h>
// #include <Wire.h>
// WiFiClient espClient;
// PubSubClient client(espClient);

// const char* ssid = "ESP32";
// const char* password = "12345678";
// const char *mqtt_broker = "broker.mqttdashboard.com";
// const int mqtt_port = 1883;
// const char *topic_pub = "Esp32/Mqtt";
// const char *topic_sub = "Mqtt/Esp32";
// const char *mqtt_username = "Demo";
// const char *mqtt_password = "public";

// DynamicJsonDocument data(1024);
// char Buffer[1000];
// void MQTT_Connect();
// void MQTT_Pub_Sub();
// //void callback(char *topic, byte *payload, unsigned int length) ;
// void Data_to_Json(String js_temp, String js_humi, String js_soil);
// void Detext(String Str);
// hw_timer_t *Timer0_Cfg = NULL;
// volatile uint8_t flag_stt = 0;

// void IRAM_ATTR MQTT() {
//   flag_stt = 0;
// }
// void setup() {
//   Serial.begin(9600);
//   WiFi.begin(ssid, password);
//   while (WiFi.status() != WL_CONNECTED) {
//     delay(1000);
//     Serial.println("Connecting to WiFi...");
//   }
//   Serial.println("Connected to WiFi");
//   MQTT_Connect();
//   Serial.begin(115200);
//   Timer0_Cfg = timerBegin(0, 8000, true);
//   timerAttachInterrupt(Timer0_Cfg, &MQTT, true);
//   timerAlarmWrite(Timer0_Cfg, 5000, true);
//   timerAlarmEnable(Timer0_Cfg);
// }
// String temperature = "25";
//     String humidity = "60";
//     String soilMoisture = "35";
// void loop() {
//   client.loop();
//   if (flag_stt) {
//     Data_to_Json(temperature, humidity, soilMoisture);
//     client.publish(topic_pub, Buffer);
//     flag_stt = 0;
//   }

// }

// void MQTT_Connect() {
//   client.setServer(mqtt_broker, mqtt_port);
//   //client.setCallback(callback);

//   while (!client.connected()) {
//     String client_id = "esp32-client-";
//     client_id += String(WiFi.macAddress());
//     Serial.printf("The client %s connects to the public mqtt broker\n", client_id.c_str());
//     if (client.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
//       Serial.println("Public emqx mqtt broker connected");
//       MQTT_Pub_Sub();
//     } else {
//       Serial.print("failed with state ");
//       Serial.print(client.state());
//       delay(2000);
//     }
//   }
// }

// void MQTT_Pub_Sub() {
//   client.subscribe(topic_sub);
// }

// void Data_to_Json(String js_temp, String js_humi, String js_soil) {
//   data.clear();
//   data["temp"] = js_temp;
//   data["humi"] = js_humi;
//   data["soil"] = js_soil;
// }
// void Detext(String Str) {
//   deserializeJson(data, Str);
//   String sTemp = data["temp"].as<String>();
//   String sHumi = data["humi"].as<String>();
//   String sSoil = data["soil"].as<String>();
//   int val_Temp = sTemp.toInt();
//   int val_Humi = sHumi.toInt();
//   int val_Soil = sSoil.toInt();
//   Serial.println("Humi: " + String(val_Temp));
//   Serial.println("Temp: " + String(val_Humi));
//   Serial.println("Soil: " + String(val_Soil));
// }
#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <Wire.h>
WiFiClient espClient;
PubSubClient client(espClient);

const char *ssid = "ESP32";
const char *password = "12345678";
const char *mqtt_broker = "broker.mqttdashboard.com";
const int mqtt_port = 1883;
const char *topic_pub = "Esp/Mqt";
const char *topic_sub = "Mqt/Esp";
const char *mqtt_username = "Quan";
const char *mqtt_password = "Nquan";

DynamicJsonDocument data(1024);
char Buffer[1000];
void MQTT_Connect();
void MQTT_Pub_Sub();
void callback(char *topic, byte *payload, unsigned int length);
void Data_to_Json(String js_temp, String js_humi, String js_soil);
void Detext(String Str);
void To_mqtt_browser();
hw_timer_t *Timer0_Cfg = NULL;
uint8_t flag_stt = 0;
int count=0;

void IRAM_ATTR MQTT()
{
  flag_stt = 1;
}
void setup()
{
  Serial.begin(9600);
  // Timer0_Cfg = timerBegin(0, 8000, true);
  // timerAttachInterrupt(Timer0_Cfg, &MQTT, true);
  // timerAlarmWrite(Timer0_Cfg, 50000 , true);
  // timerAlarmEnable(Timer0_Cfg);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  MQTT_Connect();
  Timer0_Cfg = timerBegin(0, 8000, true);
  timerAttachInterrupt(Timer0_Cfg, &MQTT, true);
  timerAlarmWrite(Timer0_Cfg, 1000, true);
  timerAlarmEnable(Timer0_Cfg);
}
void loop()
{
  client.loop();
  if (flag_stt)
  {
    count++;
    MQTT_Pub_Sub();
    flag_stt = 0;
  }
  if (count==50)
  {
    To_mqtt_browser();
    count=0;
  }
}
void To_mqtt_browser()
{
  String temperature = "25";
  String humidity = "60";
  String soilMoisture = "35";
  Serial.print("Acp");
  Data_to_Json(temperature, humidity, soilMoisture);
  client.publish(topic_pub, Buffer);
}
void MQTT_Connect()
{
  client.setServer(mqtt_broker, mqtt_port);
  client.setCallback(callback);
  while (!client.connected())
  {
    String client_id = "esp32-client-";
    client_id += WiFi.macAddress();
    Serial.printf("The client %s connects to the public mqtt broker\n", client_id.c_str());

    if (client.connect(client_id.c_str(), mqtt_username, mqtt_password))
    {
      Serial.println("Public emqx mqtt broker connected");
      //MQTT_Pub_Sub();
    }
    else
    {
      Serial.print("Failed with state ");
      Serial.println(client.state());
      delay(2000);
    }
  }
}

void MQTT_Pub_Sub()
{
  client.subscribe(topic_sub);
}

void Data_to_Json(String js_temp, String js_humi, String js_soil)
{
  data.clear();
  data["temp"] = js_temp;
  data["humi"] = js_humi;
  data["soil"] = js_soil;
  serializeJson(data, Buffer);
}
void Detext(String Str)
{
  deserializeJson(data, Str);
  String sTemp = data["temp"].as<String>();
  String sHumi = data["humi"].as<String>();
  String sSoil = data["soil"].as<String>();
  int val_Temp = sTemp.toInt();
  int val_Humi = sHumi.toInt();
  int val_Soil = sSoil.toInt();
  Serial.println("Humi: " + String(val_Temp));
  Serial.println("Temp: " + String(val_Humi));
  Serial.println("Soil: " + String(val_Soil));
}
void callback(char *topic, byte *payload, unsigned int length)
{
  Serial.println("Message arrived in topic: ");
  Serial.println(topic);
  Serial.print("Message: ");
  for (int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  for (int i = 0; i < length; i++)
  {
    
    Buffer[i] = (char)payload[i];
  }
  Buffer[length] = '\0';
  Detext(Buffer);
}