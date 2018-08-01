//// Piny PWM /////////////////////////
const int pwm1 = 9;
const int pwm2 = 10;
const int pwm3 = 11;
const int sensor = 12;
const int prog = 3;

//definicje godziny wschodu, podane w sekundach mierzonych od polnocy
//np. wschod o 6.00 to 6*3600 + 0*60 = 21600
//wschod to start sterownika
const int wschod1 = 0;
const int wschod2 = 0;
const int wschod3 = 0;

//definicje godziny poranka, podane w sekundach mierzonych od polnocy
//np. dzien o 6.15 to 6*3600 + 15*60 = 22500
//dzien1 to sekunda od ktorej zaczyna sie dzien
const int dzien1  = 930;
const int dzien2  = 935;
const int dzien3  = 940;

//definicje godziny zachodu, podane w sekundach mierzonych od polnocy
//np. zachod o 20.00 to 22*3600 + 0*60 = 72000
//zachod to sekunda od ktorej zaczyna sie zachod
const int zachod1 = 1000;
const int zachod2 = 1005;
const int zachod3 = 1010;

//definicje godziny poranka, podane w sekundach mierzonych od polnocy
//np. noc o 20.15 to 22*3600 + 15*60 = 72900
//noc to sekunda od ktorej zaczyna sie noc
const int noc1 = 1500;
const int noc2 = 1505;
const int noc3 = 15010;

//// Maksymalna moc kanalu [%] //////
const int maxPower1 = 30;
const int maxPower2 = 30;
const int maxPower3 = 30;

struct czas {
  int milisekundy;
  int sekundy;
};
struct czas aktualny;

int rozciagniecie;
#include <Wire.h>
#include "RTClib.h"


RTC_Millis rtc;

void setup() {
  //deklaracje wejsc/wyjsc
  pinMode(pwm1, OUTPUT);
  pinMode(pwm2, OUTPUT);
  pinMode(pwm3, OUTPUT);
  pinMode(sensor, INPUT);

  //debugging z RTC
  Serial.begin(57600);
  rtc.adjust(DateTime(2014, 1, 21, 0, 0, 0));
  rtc.begin();
}

void loop() {

  //debugging z RTC
  DateTime now = rtc.now();
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.print(now.second(), DEC);
  aktualny.sekundy = now.minute() * 60 + now.second();


  //zegar(&aktualny);
  pora(&aktualny, pwm1, maxPower1, wschod1, dzien1, zachod1, noc1);
  pora(&aktualny, pwm2, maxPower2, wschod2, dzien2, zachod2, noc2);
  pora(&aktualny, pwm3, maxPower3, wschod3, dzien3, zachod3, noc3);
}

void pora(struct czas *t, int pwm, int maxPower, int wschod, int dzien, int zachod, int noc) {
  if (t->sekundy >= wschod and t->sekundy < dzien) {
    wschodzi(&aktualny, pwm, maxPower, wschod, dzien);
    Serial.println("jest wschod");
  }
  if (t->sekundy >= dzien and t->sekundy < zachod) {
    analogWrite(pwm, maxPower);
    Serial.println("jest dzien");
  }
  if (t->sekundy >= zachod and t->sekundy < noc) {
    zachodzi(&aktualny, pwm, maxPower, zachod, noc);
    Serial.println("jest zachod");
  }

  //to przypisane czasu potrzebne jest tylko do testu, zeby skrocic petle
  if (t->sekundy >= noc) {
    digitalWrite(pwm, LOW);
    Serial.println("jestnoc");
    //aktualny.sekundy = 0;
    //--------------------//
  }

  oswietlenie(&aktualny, pwm, maxPower, zachod, noc);
}

void wschodzi(struct czas *t, int kanal, int maxPower, int wschod, int dzien) {
  Serial.println(" i wschodzi z moca: ");
  float mocPrzeliczona = maxPower * (9256 / 1024);
  int kierunek = HIGH;
  if (t->milisekundy == 0)
    rozciagniecie++;
  int tmp = (mocPrzeliczona / (dzien - wschod)) * rozciagniecie;
  softPWM(kanal, tmp, kierunek);
  Serial.println(tmp);
}

void zachodzi(struct czas *t, int kanal, int maxPower, int noc, int zachod) {
  Serial.println(" i zachodzi z moca: ");
  int kierunek = LOW;
  if (t->milisekundy == 0)
    rozciagniecie++;
  float wspolczynnikSoft = (maxPower / (noc - zachod));
  int tmp = wspolczynnikSoft * rozciagniecie;
  softPWM(kanal, tmp, kierunek);
  Serial.println(tmp);
}

void softPWM(int kanal, int wypelnienie, bool kierunek) {
  for ( int i = 0 ; i < 1000 ; i++) {
    if ( i < wypelnienie)
      digitalWrite(kanal, kierunek);
    else
      digitalWrite(kanal, !kierunek);
  }
}

int buttonPushCounter = 0;   // counter for the number of button presses
int buttonState = 0;         // current state of the button
int lastButtonState = 0;     // previous state of the button

void oswietlenie(struct czas *t, int kanal, int maxPower, int noc, int zachod) {
  buttonState = digitalRead(sensor);
  if (buttonState != lastButtonState) {
    if (buttonState == HIGH) {
      buttonPushCounter++;
      Serial.println(buttonPushCounter);
    }
  }
  lastButtonState = buttonState;

  if (buttonPushCounter % 4 == 0) {
    wschodzi(&aktualny, kanal, maxPower, t->sekundy, t->sekundy+5);
  } 
  if (buttonPushCounter % 8 == 0) {
    zachodzi(&aktualny, kanal, maxPower, t->sekundy, t->sekundy+5);
  }
}

void zegar(struct czas *t) {
  delay(1);
  t->milisekundy++;
  if (t->milisekundy == 999) {
    t->milisekundy = 0;
    t->sekundy++;
  }
}
