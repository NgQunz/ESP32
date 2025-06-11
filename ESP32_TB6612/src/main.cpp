
#include <Arduino.h>

// Khai báo các chân kết nối cho động cơ 1
#define AIN1 26 // Chân AIN1 của động cơ 1 trên TB6612
#define AIN2 27 // Chân AIN2 của động cơ 1 trên TB6612
#define PWMA 25 // Chân PWMA (PWM) của động cơ 1 trên TB6612

// Khai báo các chân kết nối cho động cơ 2
#define BIN1 22 // Chân AIN1 của động cơ 2 trên TB6612
#define BIN2 23 // Chân AIN2 của động cơ 2 trên TB6612
#define PWMB 21 // Chân PWMB (PWM) của động cơ 2 trên TB6612

// #define STBY 35     // Chân STBY (Standby) trên TB6612

void setup()
{
  // Cấu hình các chân là đầu ra
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(PWMA, OUTPUT);

  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);
  pinMode(PWMB, OUTPUT);

  // pinMode(STBY, OUTPUT);

  // Bật module TB6612
  // digitalWrite(STBY, HIGH);
}

void loop()
{
  analogWrite(PWMA, 100); // Tốc độ tối đa
  // Chạy động cơ 1 theo chiều thuận
  digitalWrite(AIN1, HIGH);
  digitalWrite(AIN2, LOW);

  analogWrite(PWMB, 100); // Tốc độ tối đa
  // Chạy động cơ 2 theo chiều ngược lại
  digitalWrite(BIN1, LOW);
  digitalWrite(BIN2, HIGH);
}
#include "gpio.h"
#include "timer2_delay.h"

int n = 4;
void Blink_led_1()
{
  uint32_t i;
  for (i = 0; i < 4; i++)
  {
    GPIO_SetBits(GPIOA, (1 << i)); // On LED A 0-3 // phép dich trai, tuc la se bat theo ma nhi pha
    Delay_ms(500);                 // ví du led A0 se la 00000001, A1 la 00000010
                                   // GPIO_ResetBits(GPIOA, (1 << i));// Off LED A 0-3
  }
  for (i = 0; i < 4; i++)
  {
    // GPIO_SetBits(GPIOA, (1 << i));// On LED A 0-3 // phép dich trai, tuc la se bat theo ma nhi pha
    Delay_ms(500);                   // ví du led A0 se la 00000001, A1 la 00000010
    GPIO_ResetBits(GPIOA, (1 << i)); // Off LED A 0-3
  }
}
void Blink_led_2()
{
  uint32_t i;
  for (i = 0; i < 4; i++)
  {
    GPIO_SetBits(GPIOA, (1 << i));
    Delay_ms(500);
    GPIO_ResetBits(GPIOA, (1 << i));
  }
}
void Blink_led_fly()
{
  int i = 0;
  for (i = 0; i < 2; i++)
  {
    GPIO_SetBits(GPIOA, (1 << i));       // Bat LED ngoài cùng (A0 và A3)
    GPIO_SetBits(GPIOA, (1 << (3 - i))); // Bat LED ngoài cùng d?i di?n (A3 và A0)
    Delay_ms(500);
  }

  // Tat tu trong ra ngoài
  for (i = 1; i >= 0; i--)
  {
    GPIO_ResetBits(GPIOA, (1 << i));       // Tat LED trong cùng (A1 và A2)
    GPIO_ResetBits(GPIOA, (1 << (3 - i))); // Tat LED trong cùng d?i di?n (A2 và A1)
    Delay_ms(500);
  }
}
void Blink_led_water_drop()
{
  int i; // Declare variable i outside the loops

  // Water drop effect: light up LEDs from A0 to A3
  for (i = 0; i < n; i++)
  {
    GPIO_SetBits(GPIOA, (1 << i)); // Turn on LED from A0 to A3
    Delay_ms(500);                 // Delay between each LED turning on
    if (i != n - 1)
    {
      GPIO_ResetBits(GPIOA, (1 << i)); // Turn off LED after the drop
    }
  }
  n--;
  if (n == 0)
  {
    // Turn off all LEDs and repeat the effect
    for (i = 0; i < 4; i++)
    {
      GPIO_ResetBits(GPIOA, (1 << i)); // Turn off all LEDs from A0 to A3
    }
    Delay_ms(500); // Delay before starting the effect again
    n = 4;
  }
}

int main()
{
  timer_Init();
  led_Init();
  GPIO_Config();
  GPIO_SetBits(GPIOA, GPIO_Pin_9);    // ON LED 9
  GPIO_ResetBits(GPIOA, GPIO_Pin_10); // Off LED 10
  while (1)
  {
    Blink_led_snake();
  }
}
