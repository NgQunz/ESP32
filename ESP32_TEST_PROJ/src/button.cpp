#include <header.h>
int Button_Regime_Old = 1;
int Button_Regime_New = 1;
int Button_Led1_Old = 1;
int Button_Led1_New = 1;
int Button_Led2_Old = 1;
int Button_Led2_New = 1;
int Button_Led3_Old = 1;
int Button_Led3_New = 1;
int Button_Fan1_Old = 1;
int Button_Fan1_New = 1;
int Button_Fan2_Old = 1;
int Button_Fan2_New = 1;
int Button_Regime()
{
    Button_Regime_Old = Button_Regime_New;
    Button_Regime_New = digitalRead(BTN_REGIME);
    if (Button_Regime_New == 0 && Button_Regime_Old == 1)
    {
        return 1;
    }
    return 0;
}
int Button_Led1()
{
    Button_Led1_Old = Button_Led1_New;
    Button_Led1_New = digitalRead(BTN_FAN_1);
    if (Button_Led1_New == 0 && Button_Led1_Old == 1)
    {
        return 1;
    }
    return 0;
}
int Button_Led2()
{
    Button_Led2_Old = Button_Led2_New;
    Button_Led2_New = digitalRead(BTN_LED_2);
    if (Button_Led2_New == 0 && Button_Led2_Old == 1)
    {
        return 1;
    }
    return 0;
}
int Button_Led3()
{
    Button_Led3_Old = Button_Led3_New;
    Button_Led3_New = digitalRead(BTN_LED_3);
    if (Button_Led3_New == 0 && Button_Led3_Old == 1)
    {
        return 1;
    }
    return 0;
}
int Button_Fan1()
{
    Button_Fan1_Old = Button_Fan1_New;
    Button_Fan1_New = digitalRead(BTN_FAN_1);
    if (Button_Fan1_New == 0 && Button_Fan1_Old == 1)
    {
        return 1;
    }
    return 0;
}
int Button_Fan2()
{
    Button_Fan2_Old = Button_Fan2_New;
    Button_Fan2_New = digitalRead(BTN_FAN_2);
    if (Button_Fan2_New == 0 && Button_Fan2_Old == 1)
    {
        return 1;
    }
    return 0;
}