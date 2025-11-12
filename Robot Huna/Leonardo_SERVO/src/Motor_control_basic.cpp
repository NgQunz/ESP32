#include "Motor_control_basic.h"

const bool FORWARD_IS_HIGH = true;
inline uint8_t DIR_FWD() { return FORWARD_IS_HIGH ? HIGH : LOW; }
inline uint8_t DIR_REV() { return FORWARD_IS_HIGH ? LOW : HIGH; }

const int PWM_CAP = 140;
inline int capPWM(int v) { return constrain(v, 0, PWM_CAP); }

float GAIN_L = 0.78f, GAIN_R = 1.18f;
int BIAS_L = 0, BIAS_R = 0;

float CM_PER_SEC_BASE = 34.5f;
const int SPEED_CAL_PWM = 70;
float CM_SCALE = 0.909f;
float CM_OFFSET = 4.545f;
float NEAR_OFFSET_CM = 0.0f;
float NEAR_APPLY_UPTO = 20.0f;

const int BRAKE_BACK_PWM = 60;
const uint16_t BRAKE_MS = 120;

static int g_targetL = 0, g_targetR = 0;

static void setWheelSigned(int pwmSigned, uint8_t dirPin, uint8_t pwmCh);
static void applyTrimAndWrite(int leftSigned, int rightSigned);
static void stopActiveBrake();
static unsigned long msForDistance_linear(float cm_req, int pwmL, int pwmR);

void MotorInit()
{
    pinMode(ML_DIR, OUTPUT);
    pinMode(MR_DIR, OUTPUT);
    pinMode(ML_PWM, OUTPUT);
    pinMode(MR_PWM, OUTPUT);
    SoftPWMBegin();
    SoftPWMSet(ML_PWM, 0);
    SoftPWMSet(MR_PWM, 0);
}

void Speed(int left, int right)
{
    g_targetL = constrain(left, -255, 255);
    g_targetR = constrain(right, -255, 255);
}

void Distance(float cm)
{
    if (cm <= 0 || (g_targetL == 0 && g_targetR == 0))
        return;
    applyTrimAndWrite(g_targetL, g_targetR);

    unsigned long hold_us = msForDistance_linear(cm, g_targetL, g_targetR) * 1000UL;
    unsigned long t0 = micros();
    while ((unsigned long)(micros() - t0) < hold_us)
        ;
    stopActiveBrake();
}

void turn_Left(int degree)
{
    Speed(-73, 70);
    Distance(32 + degree);
    Speed(0, 0);
}

void turn_Right(int degree)
{
    Speed(70, -73);
    Distance(35 + degree);
    Speed(0, 0);
}

void runWithDS(int distance)
{
    Speed(73, 73);
    Distance(distance * 1.2);
    Speed(0, 0);
}

static void setWheelSigned(int pwmSigned, uint8_t dirPin, uint8_t pwmCh)
{
    pwmSigned = constrain(pwmSigned, -255, 255);
    if (pwmSigned > 0)
    {
        digitalWrite(dirPin, DIR_FWD());
        SoftPWMSet(pwmCh, capPWM(pwmSigned));
    }
    else if (pwmSigned < 0)
    {
        digitalWrite(dirPin, DIR_REV());
        SoftPWMSet(pwmCh, capPWM(-pwmSigned));
    }
    else
    {
        SoftPWMSet(pwmCh, 0);
    }
}

static void applyTrimAndWrite(int leftSigned, int rightSigned)
{
    int sL = (leftSigned >= 0) ? +1 : -1;
    int sR = (rightSigned >= 0) ? +1 : -1;
    int mL = abs(leftSigned);
    int mR = abs(rightSigned);
    long l = lround(mL * GAIN_L) + BIAS_L;
    long r = lround(mR * GAIN_R) + BIAS_R;
    setWheelSigned(sL * capPWM((int)l), ML_DIR, ML_PWM);
    setWheelSigned(sR * capPWM((int)r), MR_DIR, MR_PWM);
}

static void stopActiveBrake()
{
    applyTrimAndWrite(-BRAKE_BACK_PWM, -BRAKE_BACK_PWM);
    delay(BRAKE_MS);
    SoftPWMSet(ML_PWM, 0);
    SoftPWMSet(MR_PWM, 0);
}

static unsigned long msForDistance_linear(float cm_req, int pwmL, int pwmR)
{
    float cm_eff = CM_SCALE * cm_req + CM_OFFSET;
    if (cm_req <= NEAR_APPLY_UPTO)
        cm_eff += NEAR_OFFSET_CM;
    int pwmMag = (abs(pwmL) + abs(pwmR)) / 2;
    float v_cm_s = CM_PER_SEC_BASE * ((float)pwmMag / (float)SPEED_CAL_PWM);
    float t_ms = (cm_eff / v_cm_s) * 1000.0f;
    return (unsigned long)(t_ms + 0.5f);
}
