#define ledA GPIOC, GPIO_PIN_5
#define ledA GPIOC, GPIO_PIN_5
#define ledA GPIOC, GPIO_PIN_5
#define ledA GPIOC, GPIO_PIN_5
#define ledA GPIOC, GPIO_PIN_5
#define ledA GPIOC, GPIO_PIN_5
#define ledA GPIOC, GPIO_PIN_5
int A=11,F=10,B=7,G=2,C=3,D=5,E=6,led_1=12,led_2=9,led_3=8, led_4=13;
unsigned char led7Hex[]={0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF0, 0x80, 0x90};
void INIT_led()
{
    GPIO_Init(GPIOD, GPIO_PIN_2, GPIO_MODE_OUT_PP_LOW_FAST); //
}
void LED7_writeByte (unsigned char b){
 if(((b>>0)& 0x01)==1)
 {
   digitalWrite(A,HIGH);
 }
 else{
   digitalWrite(A,LOW);
 }
 if(((b>>1)& 0x01)==1)
 {
   digitalWrite(B,HIGH);
 }
 else{
   digitalWrite(B,LOW);
 }
 if(((b>>2)& 0x01)==1)
 {
   digitalWrite(C,HIGH);
 }
 else{
   digitalWrite(C,LOW);
 }
 if(((b>>3)& 0x01)==1)
 {
   digitalWrite(D,HIGH);
 }
 else{
   digitalWrite(D,LOW);
 }
 if(((b>>4)& 0x01)==1)
 {
   digitalWrite(E,HIGH);
 }
 else{
   digitalWrite(E,LOW);
 }
 if(((b>>5)& 0x01)==1)
 {
   digitalWrite(F,HIGH);
 }
 else{
   digitalWrite(F,LOW);
 }
 if(((b>>6)& 0x01)==1)
 {
   digitalWrite(G,HIGH);
 }
 else{
   digitalWrite(G,LOW);
 }
}
void setup() {
 pinMode(A,OUTPUT);
 pinMode(B,OUTPUT);
 pinMode(C,OUTPUT);
 pinMode(D,OUTPUT);
 pinMode(E,OUTPUT);
 pinMode(F,OUTPUT);
 pinMode(G,OUTPUT);
 pinMode(led_1,OUTPUT);
 pinMode(led_2,OUTPUT);
 pinMode(led_3,OUTPUT);
 pinMode(led_4,OUTPUT);

}

void loop() {
 for (int n=0;n<1000;n++)
 {
   for (int i=0;i<25;i++)
   {
     digitalWrite(led_1,HIGH);
     LED7_writeByte(led7Hex[n/1000]);
     delay(10);
     digitalWrite(led_1,LOW);

     digitalWrite(led_2,HIGH);
     LED7_writeByte(led7Hex[n/100%10]);
     delay(10);
     digitalWrite(led_2,LOW);

     digitalWrite(led_3,HIGH);
     LED7_writeByte(led7Hex[n/10%10]);
     delay(10);
     digitalWrite(led_3,LOW);

     digitalWrite(led_4,HIGH);
     LED7_writeByte(led7Hex[n%10]);
     delay(10);
     digitalWrite(led_4,LOW);
   }
   
 }
}