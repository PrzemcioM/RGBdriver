//// Piny PWM /////////////////////////
#define pwmR 0                   
#define pwmG 1               
#define pwmB 4                               

//// Czas wschodu [s] /////////////////
#define sunrise1 360
#define sunrise2 720
#define sunrise3 1080

//// Czas swiecenia pelna moca [s] ///
#define lightingTime1 1080
#define lightingTime2 720
#define lightingTime3 360

//// Czas zachodu [s] ////////////////
#define sunset1 360
#define sunset2 360
#define sunset3 360

//// Maksymalna moc kanalu [%] //////
#define maxPower1 30
#define maxPower2 75
#define maxPower3 75

//// Odwrocenia dzialania PWMow [true/false]
#define pwm1Invert false
#define pwm2Invert false
#define pwm3Invert false

//// Opoznienie zalaczenia obwodu [s]
#define pwm1Delay 1
#define pwm2Delay 45
#define pwm3Delay 90

struct czas{
  int milisekundy;
  int sekundy;
  int minuty;
  int godziny;
};

#define LED_BUILTIN 1

void setup() {
  // put your setup code here, to run once:
  pinMode(pwmR, OUTPUT);
  pinMode(pwmG, OUTPUT);
  pinMode(pwmB, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  delay(1);
  struct czas aktualny;
  zegar(&aktualny);
  zapalanie(&aktualny);
   
}

void softPWM( int wypelnienieR, int wypelnienieG, int wypelnienieB){
    for( int i = 0 ; i < 1000 ; i++){
      if( i < wypelnienieR)
        digitalWrite(pwmR,HIGH);
      else 
        digitalWrite(pwmR,LOW);

      if( i < wypelnienieG)
        digitalWrite(pwmG,HIGH);
      else 
        digitalWrite(pwmG,LOW);

      if( i < wypelnienieB)
        digitalWrite(pwmB,HIGH);
      else 
        digitalWrite(pwmB,LOW);
   }
}

void zegar(struct czas *t){
   t->milisekundy = (t->milisekundy)+1;
   
   if ( t->milisekundy == 999){
      t->sekundy++;
      t->milisekundy = 0;
   }
   
   if ( t->sekundy == 59){
    t->sekundy = 0;
    t->minuty++;
   }
   
   if ( t->minuty == 59 ){
    t->minuty = 0;
    t->godziny++;
   }
}

void zapalanie(struct czas *t){
  for (int i=t->sekundy; i < 30; i++)
    softPWM(i, 0, 0);
}





