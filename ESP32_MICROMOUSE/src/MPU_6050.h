// #include <Arduino.h>
// #include <Wire.h>
// struct MPU6050_Data
// {
//     float ax, ay, az; // g
//     float gx, gy, gz; // deg/s
//     float tempC;      // °C
// };

// // init I2C + config MPU
// bool mpu_init(TwoWire &wire = Wire, uint8_t addr = 0x68);

// // đọc & cập nhật data mới nhất
// bool mpu_update();

// // lấy data hiện tại
// MPU6050_Data mpu_get();

// // đặt offset gyro (tự calib hoặc set tay)
// void mpu_set_gyro_offset(float ox, float oy, float oz);

// // calib gyro khi đứng yên (ms: thời gian lấy mẫu)
// bool mpu_calibrate_gyro(uint16_t ms = 1500);

// // tiện: in ra Serial (debug)
// void mpu_print(Stream &out = Serial);