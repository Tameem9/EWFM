#include <SoftwareSerial.h>
SoftwareSerial ss(11,12);

#include <LiquidCrystal.h>
LiquidCrystal lcd(5,6,7,8,9,10);

#define BLYNK_PRINT Serial
#include <ESP8266_Lib.h>
#include <BlynkSimpleShieldEsp8266.h>
char auth[] = "nl2yt_gR9yx3Uj1RBFpX31c9xEDk0afN";
char ssid[] = "kh";
char pass[] = "123456789";
#define EspSerial Serial
#define ESP8266_BAUD 115200
ESP8266 wifi(&EspSerial);

byte sensorInterrupt = 0; 
byte sensorPin       = 2;
float calibrationFactor = 5.5;
volatile byte pulseCount;  
float flowRate;
unsigned int flowMilliLitres;
unsigned long totalMilliLitres;
float price;
unsigned long oldTime;
unsigned long sendTime;
float PPL=3;
void setup() {
  ss.begin(9600);
  EspSerial.begin(ESP8266_BAUD);delay(10);
  Blynk.begin(auth, wifi, ssid, pass);
pinMode (A4,OUTPUT);  digitalWrite(A4,HIGH);
pinMode(sensorPin, INPUT);digitalWrite(sensorPin, HIGH);

  pulseCount        = 0;
  flowRate          = 0.0;
  flowMilliLitres   = 0;
  totalMilliLitres  = 0;
  oldTime           = 0;
  sendTime          = 0;
  attachInterrupt(sensorInterrupt, pulseCounter, FALLING);

lcd.begin(16,4);
lcd.setCursor(0, 0); lcd.print("Electronic Water");
lcd.setCursor(0, 1); lcd.print("Flow Measurement");
lcd.setCursor(0, 2); lcd.print("----( 2021 )----");
delay(3000); lcd.clear();
lcd.setCursor(1, 0); lcd.print("-E.W.F.M-");
lcd.setCursor(0, 1); lcd.print("FlowR.:");lcd.setCursor(13, 1); lcd.print("L/M");
lcd.setCursor(0, 2); lcd.print("Total=");   lcd.setCursor(13, 2); lcd.print("mL");
lcd.setCursor(0, 3); lcd.print("PRICE=");   lcd.setCursor(13, 3); lcd.print("L.E");
}

void loop() {
  Blynk.run();
  Blynk.virtualWrite(V0,totalMilliLitres);
  Blynk.virtualWrite(V1,price);
  
  int stime=millis() / 1000; 
  lcd.setCursor(13, 0);lcd.print(stime);
  if((millis() - sendTime) > 60000){ 
                                       lcd.clear();
                                       lcd.setCursor(4, 1);lcd.print("send SMS.");
                                       sendSMS();
                                       lcd.clear();
                                       sendTime = millis();}
 if((millis() - oldTime) > 1000)   
  { 
    detachInterrupt(sensorInterrupt);
    flowRate = ((1000.0 / (millis() - oldTime)) * pulseCount) / calibrationFactor;
    oldTime = millis();
    flowMilliLitres = (flowRate / 60) * 1000;
    totalMilliLitres += flowMilliLitres;

    price =totalMilliLitres*(PPL/1000);  
 lcdprint();
    pulseCount = 0;
    attachInterrupt(sensorInterrupt, pulseCounter, FALLING);
  }
}

void lcdprint(){
  lcd.setCursor(1, 0); lcd.print("-E.W.F.M-");
  lcd.setCursor(0, 1); lcd.print("FlowR.:");lcd.setCursor(13, 1); lcd.print("L/M");
  lcd.setCursor(0, 2); lcd.print("Total=");   lcd.setCursor(13, 2); lcd.print("mL");
  lcd.setCursor(0, 3); lcd.print("PRICE=");   lcd.setCursor(13, 3); lcd.print("L.E");
  lcd.setCursor(8, 1); lcd.print(flowRate);           
  lcd.setCursor(8, 2); lcd.print(totalMilliLitres);
  lcd.setCursor(8, 3); lcd.print(price);
}
/////////////////////////////////////////////////////////////////
void pulseCounter() {pulseCount++;}
////////////////////////////////////////
void sendSMS(){
 ss.println("AT"); delay(500);
 ss.println("AT+CMGF=1"); delay(500);
 ss.println("AT+CMGS=\"+201062235122\""); delay(500);
 ss.print("consmption: ");delay(500);
 ss.println(totalMilliLitres);delay(500);
 ss.print("cost= ");delay(500);
 ss.print(price); delay(500);
 ss.write(26);}
