#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// ================= BLUETOOTH =================
static const char *DEVICE_NAME = "Micromouse";

// ================= ENCODER PINS =================
#define ENC1_A 32
#define ENC1_B 33
#define ENC2_A 25
#define ENC2_B 26

// ================= MOTOR DRIVER PINS =================
#define M1_IN1 27
#define M1_IN2 17
#define M2_IN1 16
#define M2_IN2 13

#define PWM_FREQ 20000
#define PWM_RES 8

#define CH_M1_IN1 0
#define CH_M1_IN2 1
#define CH_M2_IN1 2
#define CH_M2_IN2 3

// ================= I2C / MPU6050 =================
#define I2C_SDA 21
#define I2C_SCL 22
#define MPU_ADDR 0x68

// ================= TOF XSHUT =================
// thứ tự: Right - Front - Left
#define TOF_RIGHT_XSHUT 14
#define TOF_FRONT_XSHUT 15
#define TOF_LEFT_XSHUT 23

#define LOX_RIGHT_ADDR 0x30
#define LOX_FRONT_ADDR 0x31
#define LOX_LEFT_ADDR 0x32

// ================= ROBOT CONSTANTS =================
static constexpr float MM_PER_PULSE = 252.0f / 833.0f; // đo thực nghiệm
static constexpr float CELL_MM = 180.0f;
static constexpr long CELL_PULSE = (long)(CELL_MM / MM_PER_PULSE + 0.5f);

static constexpr float TURN_TOLERANCE_DEG = 2.0f;

static constexpr int BASE_PWM_FORWARD = 150;
static constexpr int BASE_PWM_TURN = 135;
static constexpr int MIN_PWM_FORWARD = 80; // giảm xuống để correction âm có tác dụng
static constexpr int MAX_PWM_FORWARD = 220;

static constexpr int WALL_THRESHOLD_MM = 90;
static constexpr int SIDE_WALL_USE_MM = 160;

static constexpr float KP_ENC = 0.15f; // giảm mạnh: 0.55 → 0.15
static constexpr float KP_GYRO = 1.5f; // giảm: 3.2 → 1.5
static constexpr float KP_WALL = 0.3f; // giảm: 0.9 → 0.3

// ================= MAZE =================
#define LOOP_COST 20
#define TRAVEL_COST 5
#define BRANCH_REWARD 5

#endif