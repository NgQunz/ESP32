#include <vl53lx.h>

Adafruit_VL53L0X sensor1 = Adafruit_VL53L0X();
Adafruit_VL53L0X sensor2 = Adafruit_VL53L0X();
Adafruit_VL53L0X sensor3 = Adafruit_VL53L0X();
void vl53lx_setup()
{
    Serial.begin(9600);
    Wire.begin();

    pinMode(XSHUT1, OUTPUT);
    pinMode(XSHUT2, OUTPUT);
    pinMode(XSHUT3, OUTPUT);

    digitalWrite(XSHUT1, LOW);
    digitalWrite(XSHUT2, LOW);
    digitalWrite(XSHUT3, LOW);
    delay(10);

    // Khởi động sensor 1 và gán địa chỉ mới
    digitalWrite(XSHUT1, HIGH);
    delay(10);
    if (sensor1.begin(0x30))
    { // Gán địa chỉ mới 0x30
        Serial.println("Sensor 1 found at 0x30");
    }

    // Khởi động sensor 2 và gán địa chỉ mới
    digitalWrite(XSHUT2, HIGH);
    delay(10);
    if (sensor2.begin(0x31))
    { // Gán địa chỉ mới 0x31
        Serial.println("Sensor 2 found at 0x31");
    }

    // Khởi động sensor 3 và gán địa chỉ mới
    digitalWrite(XSHUT3, HIGH);
    delay(10);
    if (sensor3.begin(0x32))
    { // Gán địa chỉ mới 0x32
        Serial.println("Sensor 3 found at 0x32");
    }
}

void vl53lx_run()
{
    VL53L0X_RangingMeasurementData_t measure;

    sensor1.rangingTest(&measure, false);
    Serial.print("Sensor 1: ");
    Serial.println(measure.RangeMilliMeter);

    sensor2.rangingTest(&measure, false);
    Serial.print("Sensor 2: ");
    Serial.println(measure.RangeMilliMeter);

    sensor3.rangingTest(&measure, false);
    Serial.print("Sensor 3: ");
    Serial.println(measure.RangeMilliMeter);

    delay(500);
}
int wallFront()
{
    VL53L0X_RangingMeasurementData_t measure;
    sensor1.rangingTest(&measure, false);
    // Serial.println(measure.RangeMilliMeter);
    return (measure.RangeMilliMeter <= (SENSOR_LIMIT - 10));
}

int wallRight()
{
    VL53L0X_RangingMeasurementData_t measure;
    sensor2.rangingTest(&measure, false);
    Serial.print("1: ");
    Serial.println(measure.RangeMilliMeter);
    Serial.print(" ");
    return (measure.RangeMilliMeter < SENSOR_LIMIT );
}

int wallLeft()
{
    VL53L0X_RangingMeasurementData_t measure;
    sensor3.rangingTest(&measure, false);
    Serial.print("2: ");
    Serial.println(measure.RangeMilliMeter);
    return (measure.RangeMilliMeter < SENSOR_LIMIT -25);
}