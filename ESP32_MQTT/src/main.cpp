#include <PubSubClient.h>
#include <NTPClient.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <Wire.h>

WiFiClient espClient;
PubSubClient client(espClient);
const char* ssid = "ESP32";
const char* password = "12345678";
const char *mqtt_broker = "broker.mqttdashboard.com";
const int mqtt_port= 1883;
const char *topic_pub = "Esp32/Mqtt";
const char *topic_sub= "Mqtt/Esp32";                 
const char *mqtt_username = "Demo";
const char *mqtt_password = "public";
void MQTT_Connect(void);
void MQTT_Pub_Sub();
void callback(char *topic, byte *payload, unsigned int length);

void setup(){
  Serial.begin(9600);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  MQTT_Connect();
}
void loop(){
	client.loop();
  MQTT_Pub_Sub();
}

void MQTT_Connect(){ 
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
void MQTT_Pub_Sub(){
	client.publish(topic_pub, "Hello LAB");
	client.subscribe(topic_sub);
}
void callback(char *topic, byte *payload, unsigned int length){
	Serial.println("Message arrived in topic: ");
	Serial.println(topic);
	Serial.println("Message");
	for(int i = 0; i < length; i++){
		Serial.print((char) payload[i]);
	}
	Serial.println();
}