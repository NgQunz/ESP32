/*
 ****************************************************************************
 * @file	wifi_coap.h                                                           *
 * @author	Nguyen Thi Quynh                                             *
 * @date	15/01/2025
 * @ver 1.0                                                       *
 ****************************************************************************
 */
#include <wifi_coap.h>

Thing::CoAP::Server server;
Thing::CoAP::ESP::UDPPacketProvider udpProvider;

const char *ssid = "Q";
const char *password = "qqqqqqqq";
String URL = "http://192.168.47.20/ESP32_COAP_1/testdata.php";

const unsigned long interval = 5000; // Update every
unsigned long previousMillis = 0;
int old_heartrate = 0;
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
void Wifi_Coap_Setup()
{
    Serial.println("Initializing");
    // Initialize heart rate sensor
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
            if (heartrate == 0)
            {
                heartrate = old_heartrate;
            }
            snprintf(result, sizeof(result), "{\"tempc\": %.1f, \"heartrate\": %d}", tempc, heartrate);

            Serial.print("Sending response: ");
            Serial.println(result);

            return Thing::CoAP::Status::Ok(result); });

    server.Start();
}

void Run()
{
    server.Process();
    // Update data every 100ms
    updateSimulatedData();
    unsigned long currentMillis = millis();
    if (heartrate != 0)
    {
        old_heartrate = heartrate;
    }
    if (currentMillis - previousMillis >= interval)
    {
        previousMillis = currentMillis;
        if (heartrate == 0)
        {
            heartrate = old_heartrate;
        }
        String postData = "tempc=" + String(tempc) + "&heartrate=" + String(heartrate);

        HTTPClient http;
        http.begin(URL);
        http.addHeader("Content-Type", "application/x-www-form-urlencoded");
        int httpCode = http.POST(postData);
        String payload = http.getString();

        Serial.print("URL Chart : ");
        Serial.println("http://192.168.47.20/ESP32_COAP_1/index.html");
        Serial.print("payload : ");
        Serial.println(payload);
        Warning();
        Serial.println("---------------------------------");
    }
}

/********************************* END OF FILE ********************************/
/******************************************************************************/