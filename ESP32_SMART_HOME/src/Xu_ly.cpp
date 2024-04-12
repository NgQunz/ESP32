#include <header.h>

void Wifi_Connect()
{
  WiFi.begin(SSID1, PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
}
void Xu_ly_den(int L1)
{
  if (L1 == 0)
  {
    Serial.println("_Bật đèn");
  }
  else if (L1 == 1)
  {
    Serial.println("_Tắt đèn");
  }
}
void Xu_ly_quat(int F1)
{
  if (F1 == 0)
  {
    Serial.println("_Bật quạt");
  }
  else if (F1 == 1)
  {
    Serial.println( "_Tắt quạt");
  }
}