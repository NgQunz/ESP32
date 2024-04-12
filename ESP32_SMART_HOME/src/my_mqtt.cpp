#include <header.h>

WiFiClient espClient;
PubSubClient client(espClient);

DynamicJsonDocument data(1024);
char Buffer[1000];
int temp = 25, gas = 30, Led1 = 1, Led2 = 1, Led3 = 1, Fan2 = 1, Fan1 = 1;
void Stay_Connect_MQTT(){
  client.loop();
}

void MQTT_Connect() {
  client.setServer(MQTT_BROKER, MQTT_PORT);
  client.setCallback(callback);
  while (!client.connected()) {
    String client_id = "esp32-client-";
    client_id += String(WiFi.macAddress());
    Serial.printf("The client %s connects to the public mqtt broker\n", client_id.c_str());
    if (client.connect(client_id.c_str(), MQTT_USERNAME, MQTT_PASSWORD)) {
      Serial.println("Public emqx mqtt broker connected");
    } else {
      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);
    }
  }
}

void MQTT_Pub_Sub() {
  client.subscribe(TOPIC_SUB);
}

void callback(char *topic, byte *payload, unsigned int length) {
  Serial.println("Message arrived in topic: ");
  Serial.println(topic);
  Serial.print("Message: ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  for (int i = 0; i < length; i++) {
    
    Buffer[i] = (char)payload[i];
  }
  Buffer[length] = '\0';
  Detext(Buffer);
}
void To_mqtt_browser(){
    Data_to_Json(temp, gas, Led1, Led2, Led3, Fan1, Fan2);
    client.publish(TOPIC_PUB, Buffer);
}
void Data_to_Json(int js_temp, int js_gas, int js_led1, int js_led2, int js_led3, int js_fan1, int js_fan2)
{
  int Length_Buffer;
  String ESP32_Data = "{\"temp\":\"" + String(js_temp) + "\"," + "\"gas\":\"" + String(js_gas) + "\"," +
                       "\"led1\":\"" + String(js_led1) + "\"," + "\"led2\":\"" + String(js_led2) + "\","+ 
                       "\"led3\":\"" + String(js_led3) + "\"," + "\"fan1\":\"" + String(js_fan1) + "\"," +
                       "\"fan2\":\"" + String(js_fan2) + "\"}";
           
 Length_Buffer = ESP32_Data.length()+1;
 ESP32_Data.toCharArray(Buffer,Length_Buffer);
}
void Detext(String Str) {
  deserializeJson(data, Str);

  if (data.containsKey("set")) {
    int Setup = data["set"].as<int>();
    if (Setup == 0) {
      if (data.containsKey("temp")) {
        temp = data["temp"].as<int>();
        Serial.println("Set Temp to : " + String(temp));
      }
      if (data.containsKey("gas")) {
        gas = data["gas"].as<int>();
        Serial.println("Set Gas Limit to: " + String(gas));
      }
    }
  }
  if (data.containsKey("ctr")) {
    int Control = data["ctr"].as<int>();
    if (Control == 0) {
      if (data.containsKey("led1")) {
        Led1 = data["led1"].as<int>();
        Serial.print("Led1 : " + String(Led1));
        Xu_ly_den(Led1);
      }
      if (data.containsKey("led2")) {
        Led2 = data["led2"].as<int>();
        Serial.print("Led2 : " + String(Led2));
        Xu_ly_den(Led2);
      }
      if (data.containsKey("led3")) {
        Led3 = data["led3"].as<int>();
        Serial.print("Led3 : " + String(Led3));
        Xu_ly_den(Led3);
      }
      if (data.containsKey("fan1")) {
        Fan1 = data["fan1"].as<int>();
        Serial.print("Fan1 : " + String(Fan1));
        Xu_ly_quat(Fan1);
      }
      if (data.containsKey("fan2")) {
        Fan2 = data["fan2"].as<int>();
        Serial.print("Fan2 : " + String(Fan2));
        Xu_ly_quat(Fan2);
      }
    }
  }
}