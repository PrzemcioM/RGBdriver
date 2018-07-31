//// Piny PWM /////////////////////////
const int pwm1 = 0;
const int pwm2 = 1;
const int pwm3 = 4;
const int sensor = 6;
const int prog = 3;

//definicje godziny wschodu, podane w sekundach mierzonych od polnocy
//np. wschod o 6.00 to 6*3600 + 0*60 = 21600
//wschod to start sterownika
const int wschod1 = 0;
const int wschod2 = 20;
const int wschod3 = 15;

//definicje godziny poranka, podane w sekundach mierzonych od polnocy
//np. dzien o 6.15 to 6*3600 + 15*60 = 22500
//dzien1 to sekunda od ktorej zaczyna sie dzien
const int dzien1  = 15;
const int dzien2  = 35;
const int dzien3  = 30;

//definicje godziny zachodu, podane w sekundach mierzonych od polnocy
//np. zachod o 20.00 to 22*3600 + 0*60 = 72000
//zachod to sekunda od ktorej zaczyna sie zachod
const int zachod1 = 30;
const int zachod2 = 50;
const int zachod3 = 45;

//definicje godziny poranka, podane w sekundach mierzonych od polnocy
//np. noc o 20.15 to 22*3600 + 15*60 = 72900
//noc to sekunda od ktorej zaczyna sie noc
const int noc1 = 45;
const int noc2 = 65;
const int noc3 = 60;

//// Maksymalna moc kanalu [%] //////
const int maxPower1 = 30;
const int maxPower2 = 30;
const int maxPower3 = 30;

struct czas {
  int milisekundy;
  int sekundy;
};
struct czas aktualny;

void setup() {
  //deklaracje wejsc/wyjsc
  pinMode(pwm1, OUTPUT);
  pinMode(pwm2, OUTPUT);
  pinMode(pwm3, OUTPUT);
  //pinMode(sensor, INPUT);
}

void loop() {
  zegar(&aktualny);
  pora(&aktualny, pwm1, maxPower1, wschod1, dzien1, zachod1, noc1);
  //pora(&aktualny, pwm2, maxPower2, wschod2, dzien2, zachod2, noc2);
  //pora(&aktualny, pwm3, maxPower3, wschod3, dzien3, zachod3, noc3);
}

void pora(struct czas *t, int pwm, int maxPower, int wschod, int dzien, int zachod, int noc) {
  if (t->sekundy >= wschod and t->sekundy < dzien) {
    wschodzi(&aktualny, pwm, maxPower, 0, wschod, dzien);
    Serial.println("jest wschod");
  }
  if (t->sekundy >= dzien and t->sekundy < zachod) {
    analogWrite(pwm, maxPower);
    Serial.println("jest dzien");
  }
  if (t->sekundy >= zachod and t->sekundy < noc) {
    zachodzi(&aktualny, pwm, maxPower, 0, zachod, noc);
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

void wschodzi(struct czas *t, int kanal, int maxPower, int minPower, int wschod, int dzien) {
  int rozciagniecie;
  Serial.println(" i wschodzi z moca: ");
  int kierunek = HIGH;
  if (t->milisekundy == 0)
    rozciagniecie++;
  int tmp = (maxPower / (dzien - wschod)) * rozciagniecie;
  softPWM(kanal, tmp, kierunek);
  Serial.println(tmp);
}

void zachodzi(struct czas *t, int kanal, int maxPower, int minPower, int noc, int zachod) {
  int rozciagniecie;
  Serial.println(" i zachodzi z moca: ");
  int kierunek = LOW;
  if (t->milisekundy == 0)
    rozciagniecie++;
  int tmp = (maxPower / (noc - zachod)) * rozciagniecie;
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

void oswietlenie(struct czas *t, int kanal, int maxPower, int noc, int zachod) {
  const int minPower = 10;        //do tej wartosci zmniejszy sie oswietlenie
  int liczbaWywolan;
  bool stanPoprzedni;             //kontroluje czy nastapilo zewnetrzne wlaczenie czy wylaczenie oswietlenia. jesli 1 to nastapilo wylaczenie, jesli 0 nastapilo wlaczenie
  if (digitalRead(sensor))
    liczbaWywolan++;
  if (liczbaWywolan >= prog and stanPoprzedni) {                 //prog jest const i wynosi 3
    liczbaWywolan = 0;
    if (stanPoprzedni)
      zachodzi(&aktualny, kanal, maxPower, minPower,  t->sekundy, t->sekundy+10);
    else
      wschodzi(&aktualny, kanal, maxPower, minPower,  t->sekundy, t->sekundy+10);
    stanPoprzedni = !stanPoprzedni;
    
  }
}

void zegar(struct czas *t) {
  delay(1);
  t->milisekundy++;
  if (t->milisekundy == 999) {
    t->milisekundy = 0;
    t->sekundy++;
  }
  if (aktualny.sekundy == 86400) {         //doba 86400 sekund
    aktualny.sekundy = 0;
  }
}
