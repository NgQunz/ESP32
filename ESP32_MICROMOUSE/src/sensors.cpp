#include "sensors.h"
#include "config.h"
#include <Wire.h>
#include <Adafruit_VL53L0X.h>

float yawDeg = 0.0f;
static float gyroZOffset = 0.0f;
static unsigned long lastGyroMicros = 0;

int RightMM = 8190;
int FrontMM = 8190;
int LeftMM = 8190;

static Adafruit_VL53L0X loxRight = Adafruit_VL53L0X();
static Adafruit_VL53L0X loxFront = Adafruit_VL53L0X();
static Adafruit_VL53L0X loxLeft = Adafruit_VL53L0X();

static VL53L0X_RangingMeasurementData_t measureRight;
static VL53L0X_RangingMeasurementData_t measureFront;
static VL53L0X_RangingMeasurementData_t measureLeft;

static void mpuWriteByte(uint8_t reg, uint8_t data)
{
    Wire.beginTransmission(MPU_ADDR);
    Wire.write(reg);
    Wire.write(data);
    Wire.endTransmission();
}

static int16_t mpuRead16(uint8_t reg)
{
    Wire.beginTransmission(MPU_ADDR);
    Wire.write(reg);
    Wire.endTransmission(false);
    Wire.requestFrom(MPU_ADDR, (uint8_t)2);

    int16_t hi = Wire.read();
    int16_t lo = Wire.read();
    return (hi << 8) | lo;
}

void initI2C()
{
    Wire.begin(I2C_SDA, I2C_SCL);
    delay(100);
}

void initMPU6050()
{
    mpuWriteByte(0x6B, 0x00); // wake up
    mpuWriteByte(0x1B, 0x00); // gyro ±250 dps
    mpuWriteByte(0x1C, 0x00); // accel ±2g
    delay(100);
}

void calibrateGyroZ()
{
    long sum = 0;
    const int samples = 1000;

    for (int i = 0; i < samples; i++)
    {
        int16_t gz = mpuRead16(0x47);
        sum += gz;
        delay(2);
    }

    gyroZOffset = (float)sum / samples;
}

void resetYaw()
{
    yawDeg = 0.0f;
    lastGyroMicros = micros();
}

void updateYaw()
{
    unsigned long now = micros();
    float dt = (now - lastGyroMicros) / 1000000.0f;
    lastGyroMicros = now;

    int16_t gzRaw = mpuRead16(0x47);
    float gzDps = (gzRaw - gyroZOffset) / 131.0f;
    yawDeg += gzDps * dt;
}

void setupTOFAddresses()
{
    pinMode(TOF_RIGHT_XSHUT, OUTPUT);
    pinMode(TOF_FRONT_XSHUT, OUTPUT);
    pinMode(TOF_LEFT_XSHUT, OUTPUT);

    digitalWrite(TOF_RIGHT_XSHUT, LOW);
    digitalWrite(TOF_FRONT_XSHUT, LOW);
    digitalWrite(TOF_LEFT_XSHUT, LOW);
    delay(20);

    digitalWrite(TOF_RIGHT_XSHUT, HIGH);
    delay(20);
    if (!loxRight.begin(LOX_RIGHT_ADDR))
    {
        while (1)
            ;
    }

    digitalWrite(TOF_FRONT_XSHUT, HIGH);
    delay(20);
    if (!loxFront.begin(LOX_FRONT_ADDR))
    {
        while (1)
            ;
    }

    digitalWrite(TOF_LEFT_XSHUT, HIGH);
    delay(20);
    if (!loxLeft.begin(LOX_LEFT_ADDR))
    {
        while (1)
            ;
    }
}

void readTof()
{
    loxRight.rangingTest(&measureRight, false);
    loxFront.rangingTest(&measureFront, false);
    loxLeft.rangingTest(&measureLeft, false);

    RightMM = (measureRight.RangeStatus != 4) ? measureRight.RangeMilliMeter : 8190;
    FrontMM = (measureFront.RangeStatus != 4) ? measureFront.RangeMilliMeter : 8190;
    LeftMM = (measureLeft.RangeStatus != 4) ? measureLeft.RangeMilliMeter : 8190;
}