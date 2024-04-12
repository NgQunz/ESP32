#include <header_DA.h>

uint8_t cnt_false = 0;
uint8_t cnt_delay = 0;
uint8_t Card_check = 0;

uint8_t Cambien = 2;
uint8_t Value = 0;

uint8_t But_Fan_livingroom = 1;
uint8_t But_Fan_livingroom_old = 1;
uint8_t Cnt_But_Fan_livingroom = 0;

uint8_t But_Fan_kitchen = 1;
uint8_t But_Fan_kitchen_old = 1;
uint8_t Cnt_But_Fan_kitchen = 0;

uint8_t But_Light_livingroom = 1;
uint8_t But_Light_livingroom_old = 1;
uint8_t Cnt_But_Light_livingroom = 0;

uint8_t But_Light_kitchen = 1;
uint8_t But_Light_kitchen_old = 1;
uint8_t Cnt_But_Light_kitchen = 0;

byte nuidPICC[4];
byte targetUID[4] = {0xA4, 0x3D, 0x1C, 0xB8};
MFRC522 rfid(SS_PIN, RST_PIN);
Servo sg90;
LiquidCrystal_I2C lcd(0x27, 16, 2);
DHT dht(DHTPIN, DHTTYPE);

Flag flag;

int check_gas = 0;
int cnt_check_gas = 0;

int cnt_mqtt = 0;

void Flag_Status()
{
    flag.flag_stt = 0;
    flag.flag_start = 0;
    flag.flag_check_id = 0;
    flag.flag_delay = 0;
    flag.flag_Attach_Fan_manual_livingroom = 0;
    flag.flag_Attach_Fan_manual_kitchen = 0;
    flag.flag_Attach_Light_manual_livingroom = 0;
    flag.flag_Attach_Light_manual_kitchen = 0;
    flag.flag_Check_Gas = 0;
    flag.flag_Temperature = 0;
}
void MQTT_to_browser()
{
    if (flag.flag_stt)
    {
        cnt_mqtt++;
        MQTT_Pub_Sub();
        flag.flag_stt = 0;
    }
    if (cnt_mqtt==50)
    {
        To_mqtt_browser();
        cnt_mqtt=0;
    }
}
void set_pin_mode()
{
    pinMode(relayPin_Fan_livingroom, OUTPUT);
    pinMode(relayPin_Light_livingroom, OUTPUT);
    pinMode(relayPin_Fan_kitchen, OUTPUT);
    pinMode(relayPin_Light_kitchen, OUTPUT);

    digitalWrite(relayPin_Light_livingroom, HIGH);
    digitalWrite(relayPin_Fan_kitchen, HIGH);
    digitalWrite(relayPin_Fan_livingroom, HIGH);
    digitalWrite(relayPin_Light_kitchen, HIGH);

    pinMode(Cambien, INPUT);

    pinMode(but_fan_living, INPUT);
    pinMode(but_light_living, INPUT);
    pinMode(but_fan_kitchen, INPUT);
    pinMode(but_light_kitchen, INPUT);
}

void Check_ID()
{
    if (flag.flag_check_id)
    {
        if (!rfid.PICC_IsNewCardPresent())
            return;
        if (!rfid.PICC_ReadCardSerial())
            return;

        if (memcmp(rfid.uid.uidByte, targetUID, rfid.uid.size) == 0)
        {
            Card_check = 0;
            Serial.println("OK");
            Open_door();
        }
        else
        {
            cnt_false++;
            Serial.println("NO !");
            Card_check++;
            Check_id_false();
        }

        Serial.println("Flag cnt_false: " + String(cnt_false));
        Serial.println("Flag Card_check: " + String(Card_check));
        Serial.println("Flag cnt_delay: " + String(cnt_delay));

        flag.flag_check_id = 0;

        rfid.PICC_HaltA();
        rfid.PCD_StopCrypto1();
    }
}

void Check_id_false()
{
    if (flag.flag_delay)
    {
        if ((cnt_false >= 3) && Card_check)
        {
            cnt_delay++;
            cnt_false = 0;
            Card_check = 0;
            if (cnt_delay == 100)
            {
                cnt_delay = 0;
            }
        }
        flag.flag_delay = 0;
    }
}

void Open_door()
{
    for (int i = 0; i <= 180; i++)
    {
        sg90.write(i);
    }
    delay(1000);
    for (int i = 180; i >= 0; i--)
    {
        sg90.write(i);
    }
}

void On_Light_auto_living()
{
    digitalWrite(relayPin_Light_livingroom, LOW);
}

void Off_Light_auto_living()
{
    digitalWrite(relayPin_Light_livingroom, HIGH);
}

void Dark_Light_Livingroom()
{
    Value = digitalRead(Cambien);
    Serial.println(Value);
    if (Value == 1)
    {
        On_Light_auto_living();
        delay(100);
    }
    else
    {
        Off_Light_auto_living();
        delay(100);
    }
}

void Attach_Fan_manual_livingroom()
{
    if (flag.flag_Attach_Fan_manual_livingroom)
    {
        But_Fan_livingroom_old = But_Fan_livingroom;
        But_Fan_livingroom = digitalRead(but_fan_living);

        Serial.println("Trang Thai: " + String(But_Fan_livingroom));

        if (But_Fan_livingroom_old == 1 && But_Fan_livingroom == 0)
        {
            Cnt_But_Fan_livingroom++;
        }

        Serial.println("ButFan: " + String(Cnt_But_Fan_livingroom));

        if (Cnt_But_Fan_livingroom % 2 == 1)
        {
            digitalWrite(relayPin_Fan_livingroom, LOW);
        }
        else
        {
            digitalWrite(relayPin_Fan_livingroom, HIGH);
        }
        flag.flag_Attach_Fan_manual_livingroom = 0;
    }
}

void Attach_Fan_manual_kitchen()
{
    if (flag.flag_Attach_Fan_manual_kitchen)
    {
        But_Fan_kitchen_old = But_Fan_kitchen;
        But_Fan_kitchen = digitalRead(but_fan_kitchen);

        if (But_Fan_kitchen_old == 1 && But_Fan_kitchen == 0)
        {
            Cnt_But_Fan_kitchen++;
            But_Fan_kitchen = 1;
        }

        if (Cnt_But_Fan_kitchen % 2 == 1)
        {
            digitalWrite(relayPin_Fan_kitchen, LOW);
        }
        else
        {
            digitalWrite(relayPin_Fan_kitchen, HIGH);
        }
        flag.flag_Attach_Fan_manual_kitchen = 0;
    }
}

void Attach_Light_manual_livingroom()
{
    if (flag.flag_Attach_Light_manual_livingroom)
    {
        But_Light_livingroom_old = But_Light_livingroom;
        But_Light_livingroom = digitalRead(but_light_living);

        Serial.println("Trang thai: " + String(But_Light_livingroom));

        if (But_Light_livingroom_old == 1 && But_Light_livingroom == 0)
        {
            Cnt_But_Light_livingroom++;
            Serial.println("cnt: " + String(Cnt_But_Light_livingroom));
        }

        Serial.println(Cnt_But_Light_livingroom);

        if (Cnt_But_Light_livingroom % 2 == 1)
        {
            digitalWrite(relayPin_Light_livingroom, LOW);
        }
        else
        {
            digitalWrite(relayPin_Light_livingroom, HIGH);
        }
        flag.flag_Attach_Light_manual_livingroom = 0;
    }
}

void Attach_Light_manual_kitchen()
{
    if (flag.flag_Attach_Light_manual_kitchen)
    {
        But_Light_kitchen_old = But_Light_kitchen;
        But_Light_kitchen = digitalRead(but_light_kitchen);
        Serial.println("Trang thai: " + String(But_Light_kitchen));

        if (But_Light_kitchen_old == 1 && But_Light_kitchen == 0)
        {
            Cnt_But_Light_kitchen++;
            But_Light_kitchen = 1;
        }

        Serial.println(Cnt_But_Light_kitchen);

        if (Cnt_But_Light_kitchen % 2 == 1)
        {
            digitalWrite(relayPin_Light_kitchen, LOW);
        }
        else
        {
            digitalWrite(relayPin_Light_kitchen, HIGH);
        }
        flag.flag_Attach_Light_manual_kitchen = 0;
    }
}

void On_Fan_auto_kitchen()
{
    digitalWrite(relayPin_Fan_kitchen, LOW);
}

void Off_Fan_auto_kitchen()
{
    digitalWrite(relayPin_Fan_kitchen, HIGH);
}

void On_Fan_auto_living()
{
    digitalWrite(relayPin_Fan_livingroom, LOW);
}

void Off_Fan_auto_living()
{
    digitalWrite(relayPin_Fan_livingroom, HIGH);
}

void Check_Gas()
{
    if (flag.flag_Check_Gas)
    {
        check_gas = analogRead(4);

        lcd.setCursor(0, 0);
        lcd.print("Gas: ");
        lcd.setCursor(5, 0);
        lcd.print(check_gas);

        while (check_gas > 1800)
        {
            Serial.println("Gas\t");
            On_Fan_auto_living();
            check_gas = analogRead(4);
        }
        flag.flag_Check_Gas = 0;
    }
}

void Temperature_and_Humidity()
{
    if (flag.flag_Temperature)
    {
        float t = dht.readTemperature();
        lcd.setCursor(0, 1);
        lcd.print("Nhiet Do: ");
        lcd.setCursor(9, 1);
        // lcd.print(t);

        while (t >= 30)
        {
            On_Fan_auto_living();
            t = dht.readTemperature();
        }

        Serial.print("Nhiet do: ");
        Serial.println(t);

        flag.flag_Temperature = 0;
    }
}

void RFID_Config()
{
    rfid.PCD_Init();
}

void Servo_set()
{
    sg90.attach(15);
}

void LCD_set()
{
    lcd.init();
    lcd.backlight();
}

void DHT_set()
{
    dht.begin();
}
