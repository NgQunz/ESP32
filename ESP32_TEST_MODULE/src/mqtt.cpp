#include "header.h"

//wifi
WiFiClient espClient;
const char* ssid = "esp32_thanh";
const char* password = "tamsosau";
//mqtt
PubSubClient client(espClient);
const char *mqtt_broker= "broker.mqttdashboard.com";
const char *topic_pub = "esp32/pub";
const char *topic_sub = "esp32/sub";
const char *mqtt_username = "demo";
const char *mqtt_password = "123456";
const int mqtt_port = 1883;

//Json
DynamicJsonDocument data(1024);
char Buffer[1000];
int Length_Buffer;
String Home_Data;
String s_living_fan;
String s_living_light;
String s_kitchen_fan;
String s_kitchen_light;
String a;
String receive;

void Setup_Wifi() 
{
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void Data_to_Json(int t, int flag_living_fan, int flag_living_light, int flag_kitchen_fan, int flag_kitchen_light) 
{
  Home_Data = "{\"temp\":\"" + String(t) + "\"," +
                "\"living_fan\":\"" + String(flag_living_fan) + "\"," +
                "\"living_light\":\"" + String(flag_living_light) + "\"," +
                "\"kitchen_fan\":\"" + String(flag_kitchen_fan) + "\"," +
                "\"kitchen_light\":\"" + String(flag_kitchen_light) + "\"}";
  Length_Buffer = Home_Data.length()+1;
  Home_Data.toCharArray(Buffer,Length_Buffer);
  //Serial.println(Buffer);
}

void Detext(String Str)
{
  flag_auto = 0;
  deserializeJson(data, Str); 
  s_living_fan = data["living_fan"].as<String>();
  s_living_light = data["living_light"].as<String>();
  s_kitchen_fan = data["kitchen_fan"].as<String>();
  s_kitchen_light = data["kitchen_light"].as<String>();
  a = data["auto"].as<String>();
  
  flag_living_fan = s_living_fan.toInt();
  flag_living_light = s_living_light.toInt();
  flag_kitchen_fan = s_kitchen_fan.toInt();
  flag_kitchen_light = s_kitchen_light.toInt();
  flag_auto = a.toInt(); 
}

void MQTT_Pub_Sub(){
  Data_to_Json(t, flag_living_fan, flag_living_light, flag_kitchen_fan, flag_kitchen_light);
  if ( count_timer % 50 == 0) //5s gửi 1 lần
  {
	  client.publish(topic_pub, Buffer);
  }
}

void MQTT_Sub()
{
  client.subscribe(topic_sub);
}

void callback(char *topic, byte *payload, unsigned int length)
{
  receive = "";
	Serial.println("Message arrived in topic: ");
	Serial.println(topic);
	Serial.print("Message: ");
	for(int i = 0; i < length; i++){
		Serial.print((char) payload[i]);
    receive += (char) payload[i];
	}
  Serial.println();
  Serial.println("bien moi");
  Serial.println(receive);
  Serial.println();
  Detext(receive);
}

void MQTT_Connect()
{
  //connecting to a mqtt broker 
  client.setServer(mqtt_broker, mqtt_port);
  client.setCallback(callback);
  while (!client.connected()) {
    String client_id = "esp32-client-";
    client_id += String(WiFi.macAddress());
    Serial.printf("The client %s connects to the public mqtt broker\n", client_id.c_str());
    if(client.connect(client_id.c_str(), mqtt_username, mqtt_password)){
      Serial.println("Public emqx mqtt broker connected");
    } 
    else{
      Serial.print("failed with state ");
      Serial.print(client.state());
    }
  }
}

void Client_Loop()
{
  client.loop();
}
