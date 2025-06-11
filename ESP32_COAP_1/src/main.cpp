#include <WiFi.h>
#include <Arduino.h>
#include "Thing.CoAP.h"
#include <HTTPClient.h>
#include <Adafruit_MLX90614.h>
#include <SPI.h>
#include <SimpleKalmanFilter.h>
#include <DFRobot_Heartrate.h>

DFRobot_Heartrate Heart_rate(DIGITAL_MODE);

SimpleKalmanFilter bo_loc(2, 2, 0.001);

// Declare our CoAP server and the packet handler
Thing::CoAP::Server server;
Thing::CoAP::ESP::UDPPacketProvider udpProvider;

Adafruit_MLX90614 mlx = Adafruit_MLX90614();
// Change here your WiFi settings
const char *ssid = "102 Mini";
const char *password = "khongchopass";

// Simulated data
float tempc = 0.0;
int heartrate = 0;
int k_heartrate = 0;

// WiFi URL for HTTP POST
String URL = "http://192.168.1.124/ESP32_COAP_1/testdata.php";

// Variables for timing
const unsigned long interval = 2000; // Update every 100ms
unsigned long previousMillis = 0;

// Pin for heart rate sensor
#define heartPin 32

// Function to simulate tempcerature and read heart rate
void updateSimulatedData()
{
  // uint8_t ratevalue;
  // tempc = mlx.readObjectTempC();    // Generate a random tempcerature between 20.0 and 35.0
  // Heart_rate.getValue(heartPin);    // A1 foot sampled values
  // ratevalue = Heart_rate.getRate(); // Get heart rate value
  // heartrate = ratevalue;
  // Serial.print(heartrate);
  // // heartrate = analogRead(heartPin); // Read heart rate sensor value
  // // heartrate = map(heartrate, 0, 4095, 0, 255);
  // for (int i = 0; i <= 9; i++)
  // {
  //   heartrate = bo_loc.updateEstimate(heartrate);
  //   // tempc = bo_loc.updateEstimate(tempc);
  // }
  tempc = 37.9;
  heartrate = 70;

  // heartrate = random(80, 120);
}
void Warning()
{
  if (tempc <= 35)
  {
    Serial.println("Warning: Tempcerature is too low");
  }
  if (tempc >= 38)
  {
    Serial.println("Warning: Tempcerature is too high");
  }
  if (heartrate <= 60)
  {
    Serial.println("Warning: Heart rate is too low");
  }
  if (heartrate >= 100)
  {
    Serial.println("Warning: Heart rate is too high");
  }
}

void Wifi_Connection()
{
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  Serial.println("IP Address:");
  Serial.println(WiFi.localIP());
}

void setup()
{
  // Initializing the Serial
  Serial.begin(9600);
  mlx.begin();
  Serial.println("Initializing");

  // Try and wait until a connection to WiFi was made
  Wifi_Connection();

  // Configure our server to use our packet handler (It will use UDP)
  server.SetPacketProvider(udpProvider);

  // Create an endpoint called "tempc_heartrate"
  server.CreateEndpoint("tempc_heartrate", Thing::CoAP::ContentFormat::TextPlain, true)
      .OnGet([](Thing::CoAP::Request &request)
             {
            Serial.println("GET request received for 'tempc_heartrate'");
            updateSimulatedData();
            // Format data as JSON
            char result[50];
            snprintf(result, sizeof(result), "{\"tempc\": %.1f, \"heartrate\": %d}", tempc, heartrate);

            Serial.print("Sending response: ");
            Serial.println(result);

            return Thing::CoAP::Status::Ok(result); });

  server.Start();

  // Configure heart rate pin
  pinMode(heartPin, INPUT);
}

void loop()
{
  server.Process();
  // Update data every 100ms
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval)
  {
    previousMillis = currentMillis;
    updateSimulatedData();
    String postData = "tempc=" + String(tempc) + "&heartrate=" + String(heartrate);

    HTTPClient http;
    http.begin(URL);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    int httpCode = http.POST(postData);
    String payload = http.getString();
    Serial.print("URL Chart : ");
    Serial.println("http://192.168.1.124/ESP32_COAP_1/index.html");
    Serial.print("Data : ");
    Serial.println(postData);
    Serial.print("httpCode : ");
    Serial.println(httpCode);
    Serial.print("payload : ");
    Serial.println(payload);
    Warning();
    Serial.println("---------------------------------");
  }
}
