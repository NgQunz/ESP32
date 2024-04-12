#include <header.h>
int cnt=0,gas,cnt_check_btn=0,old_cnt=0,new_cnt=1;
int count_che_do=0;
double temp;
const double VCC = 3.3; 
const double R2 = 10000;
const double adc_resolution = 4095; 
int ButtonState_Old = 1;
int ButtonState_New = 1;
const double A = 0.001129148;
const double B = 0.000234125;
const double C = 0.0000000876741; 
uint8_t cnt_false = 0;
uint8_t cnt_delay = 0;
uint8_t Card_check = 0;
int cnt_e3f=0;
int cnt_f1=0,cnt_f2=0,cnt_l2=0,cnt_l3=0,cnt_mqtt=0,cnt_temp=0,cnt_gas=0, old_gas=0, old_temp=0,count_e3f=0,e3f_old;
Servo SG90_1;
Servo SG90_2;
byte nuidPICC[4];
byte targetUID[4] = {0xA4, 0x3D, 0x1C, 0xB8};
MFRC522 rfid(SS_PIN, RST_PIN);
LiquidCrystal_I2C lcd(0x27, 16, 2);
void SET_ALL()
{
  pinMode(BTN_REGIME, INPUT);
  pinMode(BTN_LED_2, INPUT);
  pinMode(BTN_LED_3, INPUT);
  pinMode(BTN_FAN_1, INPUT);
  pinMode(BTN_FAN_2, INPUT);
  pinMode(PIN_NTC, INPUT);
  pinMode(PIN_AS, INPUT);
  pinMode(PIN_MQ2, INPUT);
  pinMode(RELAY_LED_1, OUTPUT);
  pinMode(RELAY_LED_2, OUTPUT);
  pinMode(RELAY_LED_3, OUTPUT);
  pinMode(RELAY_FAN_1, OUTPUT);
  pinMode(RELAY_FAN_2, OUTPUT);
  rfid.PCD_Init();
  SG90_1.attach(SERVO1);
  SG90_2.attach(SERVO2);
  lcd.init();
  lcd.backlight();
}
Flag flag;
void Flag_Status()
{
  flag.flag_mqtt = 0;
  flag.flag_start = 1;
  flag.flag_check_id = 0;
  flag.flag_delay = 0;
  flag.flag_Gas = 0;
  flag.flag_Temp = 0;
  flag.flag_che_do = 0;
  flag.flag_mqtt_control = 0;
  flag.flag_auto_all = 0;
  flag.flag_open_door = 0;
  flag.flag_but_ctrl = 0;
  flag.flag_but_read = 0;
  flag.flag_lcd = 0;
  flag.flag_led_e3f = 0;
  flag.flag_cb_as = 0;
}
void MQTT_to_browser()
{
  if (flag.flag_mqtt)
  {
    cnt_mqtt++;
    MQTT_Pub_Sub();
    flag.flag_mqtt = 0;
  }
  if (cnt_mqtt==50)
  {
    To_mqtt_browser();
    cnt_mqtt=0;
  }
}
void Wifi_Connect()
{
  WiFi.begin(SSID1, PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
}
void Hien_thi_LCD()
{
  if (flag.flag_lcd)
  {
    flag.flag_lcd=0;
    lcd.setCursor(0, 0);
    lcd.println("Temp:");
    lcd.setCursor(5, 0);
    if (flag.flag_Temp)
    {
      cnt_temp++;
      if (cnt_temp==20)
      {
        lcd.println(Temp_Read(), 1);
        cnt_temp=0;
      }
      flag.flag_Temp=0;
    }
    lcd.setCursor(0, 1);
    lcd.println("Gas:");
    lcd.setCursor(5, 1);
    if (flag.flag_Gas)
    {
      cnt_gas++;
      if (cnt_gas==20)
      {
        lcd.println(Gas_Read(), 1);
        cnt_gas=0;
      }
      flag.flag_Gas=0;
    }
    lcd.setCursor(12, 1);
    if (count_che_do == 0)
    {
      lcd.println("AUTO");
    }
    if (count_che_do == 1)
    {
      lcd.println("BTN");
    }
    if (count_che_do == 2)
    {
      lcd.println("MQTT");
    }
  }
}
void CHE_DO()
{
  // if (flag.flag_che_do)
  // {
    Hien_thi_LCD();
    MQTT_to_browser();
    Stay_Connect_MQTT();
    Use_Button_to_Control();
    if (count_che_do == 0)
    {
      AUTO_ALL();
      //Use_Button_to_Control();
    }
    else if (count_che_do == 1) // 2 LÀ NÚT NHẤN
    {
      //Use_Button_to_Control();
    }
    else if (count_che_do == 2)
    {
      MQTT_Control();
    }
   // flag.flag_che_do=0;
  //}
}

void MQTT_Control()
{
  if (flag.flag_mqtt_control)
  {
    flag.flag_mqtt_control=0;
    Led_1(Led1);
    Led_2(Led2);
    Led_3(Led3);
    Fan_1(Fan1);
    Fan_2(Fan2);
  }
}
void Use_Button_to_Control()
{
  if (Read_Button(BTN_REGIME) == 1)
  {
    count_che_do++;
    count_che_do%=3;
    Serial.printf("cnt_che_do: %d",count_che_do);
  }
  if (Read_Button(BTN_FAN_1) ==  1)
  {
    cnt_f1++;
    count_che_do=1;
    Serial.println("cnt_f1: " + String(cnt_f1));
    cnt_f1%=2;
    Fan_1(cnt_f1);
  }
  if (Read_Button(BTN_FAN_2) ==  1)
  {
    cnt_f2++;
    count_che_do=1;
    Serial.println("cnt_f2: " + String(cnt_f2));
    cnt_f2%=2;
    Fan_2(cnt_f2);
  }
  if (Read_Button(BTN_LED_2) ==  1)
  {
    cnt_l2++;
    count_che_do=1;
    Serial.println("cnt_l2: " + String(cnt_l2));
    cnt_l2%=2;
    Led_2(cnt_l2);
  }
  if (Read_Button(BTN_LED_3) ==  1)
  {
    cnt_l3++;
    count_che_do=1;
    Serial.println("cnt_l3: " + String(cnt_l3));
    cnt_l3%=2;
    Led_3(cnt_l3);
  }
}
void AUTO_ALL()
{
  if (flag.flag_auto_all)
  {
    temp = Temp_Read();
    //1 là chế độ tự động
    if (temp > 30)
    {
      Fan_1(1);
    }
    else if (temp < 30)
    {
      Fan_1(0);
    }
    if (Gas_Read()>20 )
    {
      Fan_2(1);
      digitalWrite(PEPPER,1);
    }
    else if (Gas_Read()<5 )
    {
      Fan_2(0);
      digitalWrite(PEPPER,0);
    }
    LED_E3F();
    LED_CB_AS();
    Check_ID();
    flag.flag_auto_all=0;
  }
}
void LED_E3F()
{
  if(flag.flag_led_e3f)
  {
    int Led_E3F=digitalRead(PIN_E3F);
    flag.flag_led_e3f=0;
    if (Led_E3F == 0)
    {
      Led_1(1);
    }
    if (Led_E3F == 1)
    {
      cnt_e3f++;
      if (cnt_e3f%50==0)
      {
        Led_1(0);
        cnt_e3f=0;
      }
    }
  }
}
void LED_CB_AS()
{
  if (flag.flag_cb_as)
  {
    int value=digitalRead(PIN_AS);
    if (value == 1)
    {
      Led_1(1);
    }
    else
    {
      Led_1(0);
    }
    flag.flag_cb_as=0;
  }
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
      Open_The_Door();
    }
    else
    {
      cnt_false++;
      Card_check++;
      Check_id_false();
    }
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
void Open_The_Door()
{
  if (flag.flag_open_door)
  {
    for (int i = 0; i <= 90; i++)
    {
      SG90_1.write(i);
    }
    for (int i = 90; i >= 0; i--)
    {
      SG90_2.write(i);
    }
    if (flag.flag_open_door)
    {
      for (int i = 0; i <= 90; i++)
      {
        SG90_2.write(i);
      }
      for (int i = 90; i >= 0; i--)
      {
        SG90_1.write(i);
      }
    }
    flag.flag_open_door=0;
  }
}
int cnt_btn = 0;
int Read_Button(int Button)
{
  if (flag.flag_but_read)
  {
    flag.flag_but_read=0;
    ButtonState_Old = ButtonState_New;
    ButtonState_New = digitalRead(Button);
    if (ButtonState_New == 0 && ButtonState_Old == 1)
    {
        // Serial.println("cnt btn: " + String(cnt_btn++));
        // old_cnt=new_cnt;
        return 1;
    }
    return 0;
  }
}
void Led_2(int STT)
{
  digitalWrite(RELAY_LED_2,STT);
}
void Led_1(int STT)
{
  digitalWrite(RELAY_LED_1,STT);
}
void Led_3(int STT)
{
  digitalWrite(RELAY_LED_3,STT);
}
void Fan_1(int STT)
{
  digitalWrite(RELAY_FAN_1,STT);
}
void Fan_2(int STT)
{
  digitalWrite(RELAY_FAN_2,STT);
}
float Temp_Read()
{
  double Vout, Rth, temp, adc_value; 
  adc_value = analogRead(PIN_NTC);
  Vout = (adc_value * VCC) / adc_resolution;
  Rth = (VCC * R2 / Vout) - R2;
  temp = (1 / (A + (B * log(Rth)) + (C * pow((log(Rth)),3))));  
  temp = temp - 273.15;  // temp in degree celsius
  return temp;
}
int Gas_Read()
{
  int x = analogRead(PIN_MQ2) - 2095; // 2095 ~ 4095
  if ( x <= 0)
  {
    gas = 0;
  }
  else 
  {
    gas = x / 20;
  }
  return gas;
}
void Xu_ly_den(int L1)
{
  if (L1 == 1)
  {
    Serial.println("_Bật đèn");
  }
  else if (L1 == 0)
  {
    Serial.println("_Tắt đèn");
  }
}
void Xu_ly_quat(int F1)
{
  if (F1 == 1)
  {
    Serial.println("_Bật quạt");
  }
  else if (F1 == 0)
  {
    Serial.println( "_Tắt quạt");
  }
}