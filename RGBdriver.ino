//// Piny PWM /////////////////////////
#define pwm1 0                   
#define pwm2 1               
#define pwm3 4   

//definicje godziny wschodu, podane w sekundach mierzonych od polnocy
//np. wschod o 6.00 to 6*3600 + 0*60 = 21600
#define wschod1 21600
#define wschod2 21600
#define wschod3 21600

//definicje godziny poranka, podane w sekundach mierzonych od polnocy
//np. dzien o 6.15 to 6*3600 + 15*60 = 22500
#define dzien1  22500
#define dzien2  22500
#define dzien3  22500

//definicje godziny zachodu, podane w sekundach mierzonych od polnocy
//np. zachod o 20.00 to 22*3600 + 0*60 = 72000
#define zachod1 72000
#define zachod2 72000
#define zachod3 72000

//definicje godziny poranka, podane w sekundach mierzonych od polnocy
//np. noc o 20.15 to 22*3600 + 15*60 = 72900
#define noc1 72900
#define noc2 72900
#define noc3 72900

//// Maksymalna moc kanalu [%] //////
#define maxPower1 30
#define maxPower2 35
#define maxPower3 30

struct czas{
  int milisekundy;
  int sekundy;
};
struct czas aktualny;

void setup() {
  // put your setup code here, to run once:
  pinMode(pwm1, OUTPUT);
  pinMode(pwm2, OUTPUT);
  pinMode(pwm3, OUTPUT);
}

void loop() {
  delay(1);   
  zegar(&aktualny);
  pora(&aktualny);  
  
}

//3 kanalowy softPWM. Przepisuje na 1 kanałowy
void softPWM(int kanal, int wypelnienie, bool kierunek){
    for( int i = 0 ; i < 1000 ; i++){
      if( i < wypelnienie)
        digitalWrite(kanal,kierunek);
      else 
        digitalWrite(kanal,!kierunek);
    }
}

void zegar(struct czas *t){
   t->milisekundy = (t->milisekundy)+1;
   
   if ( t->milisekundy == 999){         //sekunda ma 1000 milisekund
      t->sekundy++;
      t->milisekundy = 0;
   }
  
   if ( t->sekundy == 86400){           //doba 86400 sekund
    t->sekundy = 0;
   }
}

void pora(struct czas *t){
   switch(t->sekundy){
    case wschod1:
      wschodzi(&aktualny, pwm1);
    break;
    
    case dzien1:
      analogWrite(pwm1,maxPower1);
    break;
    
    case zachod1: 
      //zachodzi(&aktualny, pwm1);
    break;
    
    case noc1:
      digitalWrite(pwm1,LOW);
    break;
    
    default:
      digitalWrite(pwm1,LOW);
   }   
}

void wschodzi(struct czas *t, int kanal){
  int kierunek = HIGH;
  int rozciagniecie;
  if (t->milisekundy == 0)
    rozciagniecie++;
  //int temp = map(rozciagniecie, 0, wschod1, 0, maxPower1);
  softPWM(kanal, rozciagniecie, kierunek);
}

void zachodzi(struct czas *t, int kanal){
  int kierunek = LOW;
  int rozciagniecie;
  if(t->milisekundy == 0)
    rozciagniecie++;
 // int temp = map(rozciagniecie, 0, zachod1, 0, maxPower1);    //rozciagniecie przeskalowuje sie automatycznie od nocy (tj. 0) do dnia (tj. maxPower) 
 // softPWM(kanal, temp, kierunek);
}
