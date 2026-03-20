// #include <Wire.h>
// #include <Adafruit_VL53L0X.h>
// #include <Arduino.h>

// #define SDA_PIN 21
// #define SCL_PIN 22

// #define XSHUT_1 14
// #define XSHUT_2 15
// #define XSHUT_3 23

// #define LOX1_ADDR 0x30
// #define LOX2_ADDR 0x31
// #define LOX3_ADDR 0x32

// Adafruit_VL53L0X lox1 = Adafruit_VL53L0X();
// Adafruit_VL53L0X lox2 = Adafruit_VL53L0X();
// Adafruit_VL53L0X lox3 = Adafruit_VL53L0X();

// void setID()
// {
//     // Tắt tất cả sensor
//     digitalWrite(XSHUT_1, LOW);
//     digitalWrite(XSHUT_2, LOW);
//     digitalWrite(XSHUT_3, LOW);
//     delay(10);

//     // Bật sensor 1
//     digitalWrite(XSHUT_1, HIGH);
//     delay(10);
//     if (!lox1.begin(LOX1_ADDR, true, &Wire))
//     {
//         Serial.println("Không tìm thấy VL53L0X 1");
//         while (1)
//             ;
//     }

//     // Bật sensor 2
//     digitalWrite(XSHUT_2, HIGH);
//     delay(10);
//     if (!lox2.begin(LOX2_ADDR, true, &Wire))
//     {
//         Serial.println("Không tìm thấy VL53L0X 2");
//         while (1)
//             ;
//     }

//     // Bật sensor 3
//     digitalWrite(XSHUT_3, HIGH);
//     delay(10);
//     if (!lox3.begin(LOX3_ADDR, true, &Wire))
//     {
//         Serial.println("Không tìm thấy VL53L0X 3");
//         while (1)
//             ;
//     }
// }

// void readSensor(Adafruit_VL53L0X &lox, const char *name)
// {
//     VL53L0X_RangingMeasurementData_t measure;

//     lox.rangingTest(&measure, false);

//     Serial.print(name);
//     Serial.print(": ");

//     if (measure.RangeStatus != 4)
//     {
//         Serial.print(measure.RangeMilliMeter);
//         Serial.println(" mm");
//     }
//     else
//     {
//         Serial.println("Out of range");
//     }
// }

// void setup()
// {
//     Serial.begin(9600);

//     pinMode(XSHUT_1, OUTPUT);
//     pinMode(XSHUT_2, OUTPUT);
//     pinMode(XSHUT_3, OUTPUT);

//     Wire.begin(SDA_PIN, SCL_PIN);

//     setID();

//     Serial.println("Khoi tao xong 3 cam bien");
// }

// void loop()
// {
//     readSensor(lox1, "Right");
//     readSensor(lox2, "Front");
//     readSensor(lox3, "Left");
//     Serial.println("----------------");
//     delay(200);
// }