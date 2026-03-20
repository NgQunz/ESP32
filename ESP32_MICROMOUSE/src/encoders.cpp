#include "encoders.h"
#include "config.h"

volatile long encoder1Count = 0;
volatile long encoder2Count = 0;

static void IRAM_ATTR enc1ISR()
{
    if (digitalRead(ENC1_B))
        encoder1Count++;
    else
        encoder1Count--;
}

static void IRAM_ATTR enc2ISR()
{
    if (digitalRead(ENC2_B))
        encoder2Count++;
    else
        encoder2Count--;
}

void initEncoders()
{
    pinMode(ENC1_A, INPUT_PULLUP);
    pinMode(ENC1_B, INPUT_PULLUP);
    pinMode(ENC2_A, INPUT_PULLUP);
    pinMode(ENC2_B, INPUT_PULLUP);

    attachInterrupt(digitalPinToInterrupt(ENC1_A), enc1ISR, RISING);
    attachInterrupt(digitalPinToInterrupt(ENC2_A), enc2ISR, RISING);
}

void resetEncoders()
{
    noInterrupts();
    encoder1Count = 0;
    encoder2Count = 0;
    interrupts();
}

long avgAbsEncoder()
{
    long e1 = abs(encoder1Count);
    long e2 = abs(encoder2Count);
    return (e1 + e2) / 2;
}

float encoderToMM(long pulse)
{
    return pulse * MM_PER_PULSE;
}