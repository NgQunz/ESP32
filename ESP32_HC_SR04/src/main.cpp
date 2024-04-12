#include <Arduino.h>

#define trig_right 17
#define echo_right 16
#define trig_left 22
#define echo_left 23
#define trig_mid 18
#define echo_mid 19
#define LED_1 27
#define LED_2 26
#define LED_3 25
#define LED_4 33

void On_led_1();
void On_led_2();
void On_led_3();
void On_led_4();
float measureDistance(int trigPin, int echoPin);
void Xu_ly_vat_can();
void Off_all();
unsigned long previousMillis = 0;
const long interval = 1000;

void setup()
{
    Serial.begin(9600);
    pinMode(trig_right, OUTPUT);
    pinMode(echo_right, INPUT);
    pinMode(trig_left, OUTPUT);
    pinMode(echo_left, INPUT);
    pinMode(trig_mid, OUTPUT);
    pinMode(echo_mid, INPUT);
    pinMode(LED_1, OUTPUT);
    pinMode(LED_2, OUTPUT);
    pinMode(LED_3, OUTPUT);
    pinMode(LED_4, OUTPUT);
    digitalWrite(LED_1, 1);
    digitalWrite(LED_2, 1);
    digitalWrite(LED_3, 1);
    digitalWrite(LED_4, 1);
}

void loop()
{
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval)
    {
        previousMillis = currentMillis;
        Xu_ly_vat_can();
    }
}

float measureDistance(int trigPin, int echoPin)
{
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    unsigned long duration = pulseIn(echoPin, HIGH);
    float distance = duration * 0.034 / 2;
    return distance;
}
void Xu_ly_vat_can()
{
    float rightDistance = measureDistance(trig_right, echo_right);
    Serial.printf("right: %.2f cm\n", rightDistance);
    float leftDistance = measureDistance(trig_left, echo_left);
    float midDistance = measureDistance(trig_mid, echo_mid);
    Serial.printf("left: %.2f cm\n", leftDistance);
    Serial.printf("mid: %.2f cm\n", midDistance);
    if (midDistance <= 5)
    {
        Off_all();
        On_led_4();
        if (rightDistance <= 5)
        {
            if (leftDistance <= 5)
            {
                Off_all();
                On_led_1();
            }
            else
            {
                Off_all();
                On_led_3();
            }
        }
        else
        {
            if (leftDistance <= 5)
            {
                Off_all();
                On_led_2();
            }
        }
    }
    else if (midDistance >= 5 && rightDistance >= 5 && leftDistance >= 5)
    {
        Off_all();
        On_led_2();
        Serial.printf("1\n");
    }
}
void Off_all()
{
    digitalWrite(LED_1, 1);
    digitalWrite(LED_2, 1);
    digitalWrite(LED_3, 1);
    digitalWrite(LED_4, 1);
}
void On_led_1()
{
    digitalWrite(LED_1, 0);
}
void On_led_2()
{
    digitalWrite(LED_2, 0);
}
void On_led_3()
{
    digitalWrite(LED_3, 0);
}
void On_led_4()
{
    digitalWrite(LED_4, 0);
}
