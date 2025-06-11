#include <lcd_library.h>

void setup() {
  setup_lcd();
}

void loop() {
  while (1) {  // Đảm bảo chạy liên tục
    run();
  }
}
