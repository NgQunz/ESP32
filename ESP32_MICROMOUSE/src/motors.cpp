#include "motors.h"
#include "config.h"

void initMotors()
{
    ledcSetup(CH_M1_IN1, PWM_FREQ, PWM_RES);
    ledcSetup(CH_M1_IN2, PWM_FREQ, PWM_RES);
    ledcSetup(CH_M2_IN1, PWM_FREQ, PWM_RES);
    ledcSetup(CH_M2_IN2, PWM_FREQ, PWM_RES);

    ledcAttachPin(M1_IN1, CH_M1_IN1);
    ledcAttachPin(M1_IN2, CH_M1_IN2);
    ledcAttachPin(M2_IN1, CH_M2_IN1);
    ledcAttachPin(M2_IN2, CH_M2_IN2);

    stopAllMotors();
}

void setMotor1(int pwm)
{
    pwm = constrain(pwm, -255, 255);

    // đã đảo chiều cho đúng robot của bạn
    if (pwm > 0)
    {
        ledcWrite(CH_M1_IN1, 0);
        ledcWrite(CH_M1_IN2, pwm);
    }
    else if (pwm < 0)
    {
        ledcWrite(CH_M1_IN1, -pwm);
        ledcWrite(CH_M1_IN2, 0);
    }
    else
    {
        ledcWrite(CH_M1_IN1, 0);
        ledcWrite(CH_M1_IN2, 0);
    }
}

void setMotor2(int pwm)
{
    pwm = constrain(pwm, -255, 255);

    // đã đảo chiều cho đúng robot của bạn
    if (pwm > 0)
    {
        ledcWrite(CH_M2_IN1, 0);
        ledcWrite(CH_M2_IN2, pwm);
    }
    else if (pwm < 0)
    {
        ledcWrite(CH_M2_IN1, -pwm);
        ledcWrite(CH_M2_IN2, 0);
    }
    else
    {
        ledcWrite(CH_M2_IN1, 0);
        ledcWrite(CH_M2_IN2, 0);
    }
}

void setMotor(int leftPWM, int rightPWM)
{
    setMotor1(leftPWM);
    setMotor2(rightPWM);
}

void stopAllMotors()
{
    setMotor(0, 0);
}

void brakeStop()
{
    stopAllMotors();
    delay(60);
}