// #include "MPU_6050.h"
// #include <Wire.h>

// static TwoWire *_wire = &Wire;
// static uint8_t _addr = 0x68;

// static MPU6050_Data _d{};
// static float _gox = 0, _goy = 0, _goz = 0;

// static bool write8(uint8_t reg, uint8_t val)
// {
//     _wire->beginTransmission(_addr);
//     _wire->write(reg);
//     _wire->write(val);
//     return _wire->endTransmission() == 0;
// }

// static bool readN(uint8_t reg, uint8_t *buf, size_t n)
// {
//     _wire->beginTransmission(_addr);
//     _wire->write(reg);
//     if (_wire->endTransmission(false) != 0)
//         return false; // repeated start
//     size_t got = _wire->requestFrom((int)_addr, (int)n, (int)true);
//     if (got != n)
//         return false;
//     for (size_t i = 0; i < n; i++)
//         buf[i] = _wire->read();
//     return true;
// }

// bool mpu_init(TwoWire &wire, uint8_t addr)
// {
//     _wire = &wire;
//     _addr = addr;

//     // I2C begin nếu bạn chưa begin ở main
//     // (Nếu main đã Wire.begin(SDA,SCL) thì bỏ cũng được)
//     _wire->begin();

//     // Wake up MPU6050: PWR_MGMT_1 (0x6B) = 0
//     if (!write8(0x6B, 0x00))
//         return false;

//     // Gyro full scale = ±250 deg/s : GYRO_CONFIG (0x1B) = 0x00
//     if (!write8(0x1B, 0x00))
//         return false;

//     // Accel full scale = ±2g : ACCEL_CONFIG (0x1C) = 0x00
//     if (!write8(0x1C, 0x00))
//         return false;

//     // DLPF config (tùy): CONFIG (0x1A)
//     // 0x03 ~ 44Hz accel / 42Hz gyro (ổn cho robot)
//     if (!write8(0x1A, 0x03))
//         return false;

//     return true;
// }

// bool mpu_update()
// {
//     // ACCEL_XOUT_H start at 0x3B, total 14 bytes
//     uint8_t b[14];
//     if (!readN(0x3B, b, sizeof(b)))
//         return false;

//     auto toInt16 = [&](int hi, int lo) -> int16_t
//     {
//         return (int16_t)((b[hi] << 8) | b[lo]);
//     };

//     int16_t ax = toInt16(0, 1);
//     int16_t ay = toInt16(2, 3);
//     int16_t az = toInt16(4, 5);
//     int16_t t = toInt16(6, 7);
//     int16_t gx = toInt16(8, 9);
//     int16_t gy = toInt16(10, 11);
//     int16_t gz = toInt16(12, 13);

//     // scale factors:
//     // accel ±2g => 16384 LSB/g
//     // gyro  ±250 => 131 LSB/(deg/s)
//     _d.ax = (float)ax / 16384.0f;
//     _d.ay = (float)ay / 16384.0f;
//     _d.az = (float)az / 16384.0f;

//     _d.tempC = (float)t / 340.0f + 36.53f;

//     _d.gx = (float)gx / 131.0f - _gox;
//     _d.gy = (float)gy / 131.0f - _goy;
//     _d.gz = (float)gz / 131.0f - _goz;

//     return true;
// }

// MPU6050_Data mpu_get() { return _d; }

// void mpu_set_gyro_offset(float ox, float oy, float oz)
// {
//     _gox = ox;
//     _goy = oy;
//     _goz = oz;
// }

// bool mpu_calibrate_gyro(uint16_t ms)
// {
//     uint32_t start = millis();
//     uint32_t n = 0;
//     double sx = 0, sy = 0, sz = 0;

//     while (millis() - start < ms)
//     {
//         if (mpu_update())
//         {
//             // tạm thời cộng raw gyro đã trừ offset (offset đang 0)
//             sx += _d.gx;
//             sy += _d.gy;
//             sz += _d.gz;
//             n++;
//         }
//         delay(2);
//     }
//     if (n < 10)
//         return false;

//     // offset = average drift
//     _gox = (float)(sx / n);
//     _goy = (float)(sy / n);
//     _goz = (float)(sz / n);
//     return true;
// }

// void mpu_print(Stream &out)
// {
//     out.print("A(g): ");
//     out.print(_d.ax, 3);
//     out.print(", ");
//     out.print(_d.ay, 3);
//     out.print(", ");
//     out.print(_d.az, 3);

//     out.print(" | G(dps): ");
//     out.print(_d.gx, 2);
//     out.print(", ");
//     out.print(_d.gy, 2);
//     out.print(", ");
//     out.print(_d.gz, 2);

//     out.print(" | T: ");
//     out.println(_d.tempC, 2);
// }