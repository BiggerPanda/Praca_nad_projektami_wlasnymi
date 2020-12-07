/**************************************/
/*              ARE 2008              */
/*      e-mail: biuro@are.net.pl      */
/*      www   : are.net.pl            */
/**************************************/

//Michalski Jakub
//Janiszyn Amadeusz

#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>
#include <string.h>

#define F_CPU 8000000UL  // 1 MHz
#define RS 0
#define RW 1
#define E  2

void delay_ms(int ms){
	volatile long unsigned int i;
	for(i=0;i<ms;i++)
		_delay_ms(1);
	}
	
void delay_us(int us){
	volatile long unsigned int i;
	for(i=0;i<us;i++)
		_delay_us(1);
	}


void LCD2x16_init(void){
PORTC &= ~(1<<RS);
PORTC &= ~(1<<RW);

PORTC |= (1<<E);
PORTD = 0x38;   // dwie linie, 5x7 punktow
PORTC &=~(1<<E);
_delay_us(120);

PORTC |= (1<<E);
PORTD = 0x0e;   // wlacz wyswietlacz, kursor, miganie
PORTC &=~(1<<E);
_delay_us(120);

PORTC |= (1<<E);
PORTD = 0x06;
PORTC &=~(1<<E);
_delay_us(120);
}

void LCD2x16_clear(void){
PORTC &= ~(1<<RS);
PORTC &= ~(1<<RW);

PORTC |= (1<<E);
PORTD = 0x01;
PORTC &=~(1<<E);
delay_ms(120);
}

void LCD2x16_putchar(int data){
PORTC |= (1<<RS);
PORTC &= ~(1<<RW);

PORTC |= (1<<E);
PORTD = data;
PORTC &=~(1<<E);
_delay_us(120);
}

void LCD2x16_pos(int wiersz, int kolumna){
PORTC &= ~(1<<RS);
PORTC &= ~(1<<RW);

PORTC |= (1<<E);
delay_ms(1);
PORTD = 0x80+(wiersz-1)*0x40+(kolumna-1);
delay_ms(1);
PORTC &=~(1<<E);
_delay_us(120);
}

int _sp = 50;
int _h = 4;

int _e;
int _dece;

float _pv;
int _ipv;
int _decpv;


int main(void){
  char tmp[16];
  int i;

  DDRD = 0xff;
  PORTD = 0x00;
  DDRC = 0xff;
  PORTC = 0xff;
  DDRB = 0x00;
  PORTB = 0xff;

  _delay_ms(500);

  LCD2x16_init();
  LCD2x16_clear();

  ADMUX = 0x40;
  ADCSRA = 0xe0;


  while(1){
    ADCSRA = ADCSRA | (1 << ADSC);
    while (ADCSRA & (1 << ADSC));

      _pv = ADC;
      _ipv = (_pv / 1023) * 100;
      _e = _sp - _ipv;

      _decpv = (((_pv / 1023) * 100) - _ipv) * 10;
      _dece = (10 - _decpv) % 10;
      
      //zapalanie diody CV1
      if(_e > 2*_h){
        PORTC=~(0x01<<3);
      }
      //gaszenie diody 
      if(_e < _h && _e > -_h){
        PORTC=(0xff<<3);
      }
      
      // zapalanie diody CV2
      if(_e < -_h*2){
        PORTC=~(0x01<<4);
      }

      
      
      if(!(PINB &(8<<PB0))){
        _sp=50;
      }
      if(!(PINB &(4<<PB0))){
        _sp=40;
      }
      if(!(PINB &(2<<PB0))){
        _h=8;
      }
      if(!(PINB &(1<<PB0))){
        _h=10;
      }
      
      LCD2x16_pos(1,1);
      sprintf(tmp,"SP=%2d PV=%3d.%1d%%",_sp, _ipv,_decpv);
      for(i=0;i < 16;i++){
        LCD2x16_putchar(tmp[i]);
      }
        
      LCD2x16_pos(2,1);
      sprintf(tmp,"H =%2d E =%3d.%1d%",_h, _e, _dece);
      for(i=0;i < 16;i++){
        LCD2x16_putchar(tmp[i]);
      }
      delay_ms(500);
    }
  return 0;
  }