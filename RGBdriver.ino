//// Piny PWM /////////////////////////
const int pwm1 = 0;
const int pwm2 = 1;
const int pwm3 = 4;

//definicje godziny wschodu, podane w sekundach mierzonych od polnocy
//np. wschod o 6.00 to 6*3600 + 0*60 = 21600
const int wschod1 = 0;
const int wschod2 = 20;
const int wschod3 = 15;

//definicje godziny poranka, podane w sekundach mierzonych od polnocy
//np. dzien o 6.15 to 6*3600 + 15*60 = 22500
const int dzien1  = 0;
const int dzien2  = 35;
const int dzien3  = 30;

//definicje godziny zachodu, podane w sekundach mierzonych od polnocy
//np. zachod o 20.00 to 22*3600 + 0*60 = 72000
const int zachod1 = 15;
const int zachod2 = 50;
const int zachod3 = 45;

//definicje godziny poranka, podane w sekundach mierzonych od polnocy
//np. noc o 20.15 to 22*3600 + 15*60 = 72900
const int noc1 = 30;
const int noc2 = 65;
const int noc3 = 60;

//// Maksymalna moc kanalu [%] //////
const int maxPower1 = 30;
const int maxPower2 = 30;
const int maxPower3 = 30;

struct czas{
  int milisekundy;
  int sekundy;
};
struct czas aktualny;

int rozciagniecie;

void setup() {
  // put your setup code here, to run once:
  pinMode(pwm1, OUTPUT);
  pinMode(pwm2, OUTPUT);
  pinMode(pwm3, OUTPUT);
}

void loop() {   
  zegar(&aktualny);
  pora(&aktualny, pwm1, maxPower1, wschod1, dzien1, zachod1, noc1);
  pora(&aktualny, pwm2, maxPower2, wschod2, dzien2, zachod2, noc2);
  pora(&aktualny, pwm3, maxPower3, wschod3, dzien3, zachod3, noc3);
}

bool stan = LOW;
void zegar(struct czas *t){
  delay(1);
  t->milisekundy++;
  if(t->milisekundy==999){
    t->milisekundy=0;
    t->sekundy++;
  }
  if(aktualny.sekundy == 86400){           //doba 86400 sekund
    aktualny.sekundy = 0;
  }
}

void pora(struct czas *t, int pwm, int maxPower, int wschod, int dzien, int zachod, int noc){
  if(t->sekundy < wschod) {
    wschodzi(&aktualny, pwm, maxPower, wschod, dzien);
  }  
  if( dzien < t->sekundy) {
    analogWrite(pwm, maxPower);
  }
  if(zachod < t->sekundy) {
    zachodzi(&aktualny, pwm, maxPower, zachod, noc);
  }

  //to przypisane czasu potrzebne jest tylko do testu, zeby skrocic petle
  if(noc < t->sekundy) {
    digitalWrite(pwm, LOW);
 //   aktualny.sekundy = 0; //--------------------//
  }  
}

void wschodzi(struct czas *t, int kanal, int maxPower, int wschod, int dzien) {
  int kierunek = HIGH;
  if (t->milisekundy == 0)
    rozciagniecie++; 
  int tmp = (maxPower / (dzien-wschod))*rozciagniecie;
  softPWM(kanal, tmp, kierunek);
}

void zachodzi(struct czas *t, int kanal, int maxPower, int noc, int zachod){
  int kierunek = LOW;
  if(t->milisekundy == 0)
    rozciagniecie++; 
  int tmp = (maxPower / (noc-zachod))*rozciagniecie;
  softPWM(kanal, tmp, kierunek);
}

void softPWM(int kanal, int wypelnienie, bool kierunek){
    for( int i = 0 ; i < 1000 ; i++) {
      if( i < wypelnienie)
        digitalWrite(kanal,kierunek);
      else 
        digitalWrite(kanal,!kierunek);
    }
}
