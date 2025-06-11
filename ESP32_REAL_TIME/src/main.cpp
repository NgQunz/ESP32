#include "weather.h"
#include "PrintPicture.h"

// Khởi tạo màn hình TFT với kích thước 240x240

unsigned long previousMillis = 0; // Biến để lưu thời điểm cuối cùng cập nhật dữ liệu
const long interval = 2000;       // Thời gian giữa các lần cập nhật dữ liệu, ở đây là 2 giây

void setup()
{
  tft.init();
  tft.setRotation(0);
  tft.fillScreen(TFT_BLACK);
}

void loop()
{
  unsigned long currentMillis = millis(); // Lấy thời gian hiện tại

  // Nếu đã đủ thời gian giữa các lần cập nhật dữ liệu
  if (currentMillis - previousMillis >= interval)
  {
    previousMillis = currentMillis; // Cập nhật lại thời điểm cuối cùng cập nhật dữ liệu

    float humidity = 10;    // Đọc độ ẩm từ cảm biến DHT
    float temperature = 10; // Đọc nhiệt độ từ cảm biến DHT

    // Xóa vị trí cũ của dữ liệu nhiệt độ và độ ẩm
    tft.fillCircle(120, 120, 118, TFT_BLACK);

    // Vẽ vòng tròn
    tft.drawCircle(120, 120, 118, TFT_RED);

    // Nếu không đọc được dữ liệu từ cảm biến
    // if (isnan(humidity) || isnan(temperature)) {
    //   tft.setTextColor(TFT_RED, TFT_BLACK); // Đặt màu chữ là màu đỏ
    //   tft.setTextSize(2);
    //   tft.setCursor(50, 100);
    //   tft.print("Failed to read from DHT sensor!");
    //   return;
    // }

    // Hiển thị độ ẩm và nhiệt độ trên màn hình
    tft.setTextColor(TFT_GREEN, TFT_BLACK); // Đặt màu chữ là màu xanh lá
    // tft.setTextSize(2);
    // tft.setCursor(50, 100);
    // tft.print("Humi: ");
    // tft.print(humidity);
    // tft.print("%");

    // tft.setCursor(50, 130);
    // tft.print("Temp: ");
    // tft.print(temperature);
    // tft.print("°C");
    tft.drawString("7", 120, 120, 7); // Vẽ hình tròn nhỏ ở giữa màn hình
    DrawPNGBitmap();
  }
}
