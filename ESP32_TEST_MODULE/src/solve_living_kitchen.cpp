#include "header.h"

int flag_auto = 1;
int cnt = 0;
int flag_living_fan = 0;
int flag_living_light = 0;
int flag_kitchen_fan = 0;
int flag_kitchen_light = 0;

int living_fan = 15;
int living_light = 4;
int kitchen_fan = 16;
int kitchen_light = 17;


void Declare_Device()
{
    pinMode( living_fan, OUTPUT);
    // digitalWrite( living_fan, HIGH);

    pinMode( living_light, OUTPUT);
    // digitalWrite( living_light, HIGH);

    pinMode( kitchen_fan, OUTPUT);
    // digitalWrite( kitchen_fan, HIGH);

    pinMode( kitchen_light, OUTPUT);
    // digitalWrite( kitchen_light, HIGH);
}

void Living_Auto()
{
    if ( Night() == 1)
    {
        flag_living_light = 1;
    }
    else 
    {
        flag_living_light = 0;
    }
    if ( t >= 30 )
    {
        flag_living_fan = 1;
    }
    else 
    {
        flag_living_fan = 0;
    }
}

void Kitchen_Auto()
{
    if ( gas >= 20 )
    {
        flag_kitchen_fan = 1;
    }
    if ( gas <= 5)
    {
        flag_kitchen_fan = 0;
    }

    if ( Night() == 1)
    {
        flag_kitchen_light = 1;
    }
    else 
    {
        flag_kitchen_light = 0;
    }
}

void Manual_Button()
{
    if ( Press_Button26() == 1)
    {
        flag_auto = 0;
        flag_living_fan = !flag_living_fan;        
    }
    if ( Press_Button32() == 1)
    {
        flag_auto = 0;
        flag_living_light = !flag_living_light;
        // flag_auto = 0;
    }
    if ( Press_Button33() == 1)
    {
        flag_auto = 0;
        flag_kitchen_fan = !flag_kitchen_fan;
        // flag_auto = 0;
    }
    if ( Press_Button34() == 1)
    {
        flag_auto = 0;
        flag_kitchen_light = !flag_kitchen_light;
        // flag_auto = 0;
    }
}

void Solve_Device()
{
    if ( flag_living_fan == 1 ) digitalWrite( living_fan, HIGH);
    else digitalWrite( living_fan, LOW);

    if ( flag_living_light == 1 ) digitalWrite( living_light, HIGH);
    else digitalWrite( living_light, LOW);

    if ( flag_kitchen_fan == 1 ) digitalWrite( kitchen_fan, HIGH);
    else digitalWrite( kitchen_fan, LOW);

    if ( flag_kitchen_light == 1 ) digitalWrite( kitchen_light, HIGH);
    else digitalWrite( kitchen_light, LOW);
}


