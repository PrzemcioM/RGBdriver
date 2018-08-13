//// Piny PWM /////////////////////////
#define pwm1 3//0xEF;	// PB4
#define pwm2 4//0xF7;	// PB3
#define pwm3 5//0xFB;	// PB2
//#define sensor 6;
//#define prog 3;

//definicje godziny wschodu, podane w sekundach mierzonych od polnocy
//np. wschod o 6.00 to 6*3600 + 0*60 = 21600
//wschod to start sterownika
#define wschod1 15;
#define wschod2 15;
#define wschod3 15;

//definicje godziny poranka, podane w sekundach mierzonych od polnocy
//np. dzien o 6.15 to 6*3600 + 15*60 = 22500
//dzien1 to sekunda od ktorej zaczyna sie dzien
#define dzien1 30;
#define dzien2 35;
#define dzien3 40;

//definicje godziny zachodu, podane w sekundach mierzonych od polnocy
//np. zachod o 20.00 to 22*3600 + 0*60 = 72000
//zachod to sekunda od ktorej zaczyna sie zachod
#define zachod1 60;
#define zachod2 65;
#define zachod3 70;

//definicje godziny poranka, podane w sekundach mierzonych od polnocy
//np. noc o 20.15 to 22*3600 + 15*60 = 72900
//noc to sekunda od ktorej zaczyna sie noc
#define noc1 90;
#define noc2 95;
#define noc3 100;

//// Maksymalna moc kanalu [%] //////
#define maxPower1 5;
#define maxPower2 5;
#define maxPower3 15;

struct czas {
  int milisekundy;
  int sekundy;
};
struct czas aktualny;
int rozciagniecie;

#ifndef F_CPU
#define F_CPU 8000000UL // 8 MHz clock speed
#endif

#include <avr/io.h>
#include <util/delay.h>
#include <stdbool.h>

void softPWM(int kanal, int wypelnienie, bool kierunek) {
  for ( int i = 0 ; i < 1000 ; i++) {
    if ( i < wypelnienie)
		DDRB |= (1<<kanal);
    else
		PORTB &= ~(1 << kanal);
  }
}

void wschodzi(struct czas *t, int kanal, int maxPower, int wschod, int dzien) {
  float mocPrzeliczona = maxPower * (256 / 1024);
  bool kierunek = true;
  int tmp;
  if (t->milisekundy == 0)
    tmp = mocPrzeliczona * rozciagniecie;
  softPWM(kanal, tmp, kierunek);
  if (rozciagniecie == mocPrzeliczona)
    rozciagniecie = mocPrzeliczona - 1;
}

void zachodzi(struct czas *t, int kanal, int maxPower, int noc, int zachod) {
  bool kierunek = false;
  if (t->milisekundy == 0)
    rozciagniecie++;
  float wspolczynnikSoft = (maxPower / (noc - zachod));
  int tmp = wspolczynnikSoft * rozciagniecie;
  softPWM(kanal, tmp, kierunek);
  if (rozciagniecie == 0)
    rozciagniecie = 1;
}

void pora(struct czas *t, int pwm, int maxPower, int wschod, int dzien, int zachod, int noc) {
  if (t->sekundy < wschod)
	PORTB |= (1<<pwm);
  if ((t->sekundy >= wschod) & (t->sekundy < dzien)) {
    wschodzi(&aktualny, pwm, maxPower, wschod, dzien);
  }
  if ((t->sekundy >= dzien) & (t->sekundy < zachod)) {
    OCR0A = maxPower;
  }
  if ((t->sekundy >= zachod) & (t->sekundy < noc)) {
    zachodzi(&aktualny, pwm, maxPower, zachod, noc);
  }

  if (t->sekundy >= noc) {
    PORTB |= (0<<pwm);
  }

  //oswietlenie(&aktualny, pwm, maxPower, zachod, noc);

}

/*
void wschodzi(struct czas *t, int kanal, int maxPower, int minPower, int wschod, int dzien) {
  //Serial.println(" i wschodzi z moca: ");
  bool kierunek = TRUE;
  if (t->milisekundy == 0)
    rozciagniecie++;
  int tmp = (maxPower / (dzien - wschod)) * rozciagniecie;
  softPWM(kanal, tmp, kierunek);
  //  Serial.println(tmp);
  }

  void zachodzi(struct czas *t, int kanal, int maxPower, int minPower, int noc, int zachod) {
    //Serial.println(" i zachodzi z moca: ");
  bool kierunek = FALSE;
  if (t->milisekundy == 0)
    rozciagniecie++;
  int tmp = (maxPower / (noc - zachod)) * rozciagniecie;
  softPWM(kanal, tmp, kierunek);
  //  Serial.println(tmp);
  }
*/

/*
void oswietlenie(struct czas *t, int kanal, int maxPower, int noc, int zachod) {
  const int minPower = 10;        //do tej wartosci zmniejszy sie oswietlenie
  int liczbaWywolan;
  bool stanPoprzedni;             //kontroluje czy nastapilo zewnetrzne wlaczenie czy wylaczenie oswietlenia. jesli 1 to nastapilo wylaczenie, jesli 0 nastapilo wlaczenie
  if (digitalRead(sensor) == true)
    liczbaWywolan++;
  if (liczbaWywolan >= prog and stanPoprzedni) {                 //prog jest const i wynosi 3
    liczbaWywolan = 0;
    if (stanPoprzedni)
      zachodzi(&aktualny, kanal, maxPower, t->sekundy, t->sekundy + 10);
    else
      wschodzi(&aktualny, kanal, maxPower,  t->sekundy, t->sekundy + 10);
    stanPoprzedni = !stanPoprzedni;

  }
} */

void zegar(struct czas *t) {
  _delay_ms(1);
  t->milisekundy++;
  if (t->milisekundy == 999) {
    t->milisekundy = 0;
    t->sekundy++;
  }
  if (aktualny.sekundy == 86400) {         //doba 86400 sekund
    aktualny.sekundy = 0;
  }
}

int main(void)
{

  DDRB = 0xFF;	//port B as output
  TCCR0A = (1 << COM0A1) | (1 << WGM00);  // phase correct PWM mode
  OCR0A  = 0x10;                          // initial PWM pulse width
    
  TCCR0B = (1 << CS01);   // clock source = CLK/8, start PWM
  zegar(&aktualny);
  //pora(&aktualny, pwm1, maxPower1, wschod1, dzien1, zachod1, noc1);
  //	pora(&aktualny, pwm2, maxPower2, wschod2, dzien2, zachod2, noc2);
  //	pora(&aktualny, pwm3, maxPower3, wschod3, dzien3, zachod3, noc3);

	PORTB |=(1<<6);
	PORTB |=(1<<7);
	while(1) {
	for ( int i = 0; i < 1000; i++){
		if ( i < 50)
			PORTB &= ~(1<<5);
		else	
			PORTB |=(1<<5);
	}
}	

}
