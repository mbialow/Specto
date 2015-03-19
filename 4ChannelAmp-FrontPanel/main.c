/*
 * main.c
 *
 *  Created on: 9 lut 2015
 *      Author: mbialowa
 */

#define BAUD 4800

#define MY_UBBR ((F_CPU / (BAUD * 16L)) - 1)

#include <inttypes.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include "keyb.h"


//ON/OFF
#define RELAY0 ( 1 << PB4 )
#define ON_OFF_RELAY RELAY0
//SUB
#define RELAY1 ( 1 << PB5 )
#define SUB_RELAY RELAY1
//SUM/SUB
#define RELAY2 ( 1 << PB6 )
#define SUM_RELAY RELAY2
//CD/AUX
#define RELAY3 ( 1 << PB7 )
#define INPUT_SOURCE_RELAY RELAY3

#define SUB_SPEEKER_RELAY ( 1 << PD2 )
#define CENT_REAR_SPEEKER_RELAY ( 1 << PD3 )

void send_uart(uint8_t c);

volatile static uint8_t rx_data = 0;
volatile static uint8_t rx_data_flag = 0;
volatile static uint8_t time_counter = 0;
static uint8_t subDelayFlag = 0;//1 - glowny przekaznik
static uint8_t centerRearDelayFlag = 0; //2 - center i tylne


static inline void init_uart(void) {
  // set baud rate
  UBRRH = (uint8_t)(MY_UBBR >> 8);
  UBRRL = (uint8_t)(MY_UBBR);
  // enable receive and transmit
  UCSRB = (1 << RXEN) | (1 << TXEN) | (1 << RXCIE);
  // set frame format
  UCSRC = (1 << USBS) | (3 << UCSZ0);	// asynchron 8n1
}

static inline void init_timers(void){

  cli();

  TCCR0A |= (1 << WGM01); // Ustawia timer w tryb CTC
  //Timer0 dla przyciskow
  TCCR0B |= (1<<CS02) | (1<<CS00);  //Ustawia timer z preskalerem Fcpu/1024
  TIMSK |= (1 << OCIE0A);// Zezwolenie na przerwania dla CTC
  OCR0A = 10; //po osiagnieciu wartosci nastapi wygenerowanie przerwania

  //timer1 dla opoznienia zalaczenia kolumn
  TCCR1B |= (1 << WGM12); // Ustawia timer w tryb CTC
  OCR1A = 15625; // Ustawia wartość pożądaną na 1Hz dla preskalera 64
  TCCR1B |= ((1 << CS10) | (1 << CS11)); // Ustawia timer z preskalerem Fcpu/64
  TIMSK |= (1 << OCIE1A); // Zezwolenie na przerwania dla CTC

  sei();            // zezwolenie na przerwania
}
 // void init_io(void) __attribute__ ((section (".init1")));
  static inline void init_io(void) {

    DDRB |= (RELAY0 | RELAY1 | RELAY2 | RELAY3);//piny jako wyjscia
    PORTB &= ~(RELAY0 | RELAY1 | RELAY2 | RELAY3);//ustaw 0 na pinach

    //ustawienie klawiszy
    KEY_DDR &= ~(KEY0 | KEY1 | KEY2 | KEY3);  //piny jako wejscia
    KEY_PORT |= (KEY0 | KEY1 | KEY2 | KEY3);  //wlacz rezystor podciagajacy linie
    //ustawienie klawiszy
    DDRD &= ~(KEY4);//pin jako wejscie
    PORTD |= (KEY4);//wlacz rezystor podciagajacy linie

    //ustawienie przekaznikow opoznionego zalaczenia glosnikow
    DDRD |= (SUB_SPEEKER_RELAY | CENT_REAR_SPEEKER_RELAY);//piny jako wyjscia
    PORTD &= ~(SUB_SPEEKER_RELAY | CENT_REAR_SPEEKER_RELAY);//ustaw 0 na pinach
}


/*
 * send_uart
 * Wyslji znak przez UART bez ISR
 */
void send_uart(uint8_t c) {
     //poczekaj na wysylke poprzedniego znaku
     while (!(UCSRA & (1<<UDRE)));
     //wyslji znak
     UDR = c;
}

ISR(TIMER0_COMPA_vect){
    KeybProc();
}

ISR(TIMER1_COMPA_vect){
    time_counter++;
}

ISR(USART_RX_vect) {
    rx_data = UDR;
    rx_data_flag = 1;
}

void powerOn(void){

     if(bit_is_set(PORTB, PB4)){
         PORTD &= ~(SUB_SPEEKER_RELAY | CENT_REAR_SPEEKER_RELAY);//odlaczenie glosnikow
         PORTB &= ~(RELAY1);//odlaczenie pozostalych przekaznikow
     }else{
         time_counter = 0;
         subDelayFlag = 1;
     }

     PORTB ^= RELAY0;
}

void subCentRearOn(void){
    if(bit_is_set(PORTB, PB5)){
        PORTD &= ~(CENT_REAR_SPEEKER_RELAY);
    }else{
        centerRearDelayFlag = 1;
        time_counter = 0;
    }

    PORTB ^= RELAY1;
}
//void main(void) __attribute__ ((noreturn));

//volatile char mcusr /*__attribute__ ((section (".noinit")))*/;

static inline void handleSerial(){
    if(rx_data_flag){
               rx_data_flag = 0;

               //do_command[rx_data-97](0);

               switch(rx_data){
                    case 'w':
                         powerOn();
                    break;

                    case 's':
                        subCentRearOn();
                    break;

                    case 'i':
                         PORTB ^= RELAY2;
                    break;

                    case 'd':
                         PORTB &= ~(RELAY3);
                    break;

                    case 'a':
                         PORTB |= RELAY3;
                    break;
               }

           }
}

static inline void handleKeys(){
    switch(GetKeys()){

                case KEY_ON_OFF:
                     ClrKeyb(KBD_LOCK);
                     powerOn();
                     send_uart('w');
                break;

                case KEY_SUB:
                     ClrKeyb(KBD_LOCK);
                     if(bit_is_set(PORTB, PB4)){
                         subCentRearOn();
                         send_uart('s');
                     }
                break;

                case KEY_SUM_FRONT_RL:
                     ClrKeyb(KBD_LOCK);
                     if(bit_is_set(PORTB, PB4)){
                         PORTB ^= RELAY2;
                         send_uart('i');
                     }
                break;

                case KEY_AUX:
                     ClrKeyb(KBD_LOCK);
                     if(bit_is_set(PORTB, PB4)){
                         PORTB &= ~(RELAY3);
                         send_uart('a');
                     }
                break;

                case KEY_CD:
                     ClrKeyb(KBD_LOCK);
                     if(bit_is_set(PORTB, PB4)){
                         PORTB |= RELAY3;
                         send_uart('d');
                     }
                break;

           }
}

static inline void handleDelaySpeekerOn(){
    //jesli wzmacniacz jest wlaczony i minal czas 4s
           if(bit_is_set(PORTB, PB4) && time_counter > 4){
               time_counter = 0;

               //zalacz glosniki
               if(centerRearDelayFlag){
                   PORTD |= (CENT_REAR_SPEEKER_RELAY);
                   centerRearDelayFlag = 0;
               }
               if(subDelayFlag){
                   PORTD |= (SUB_SPEEKER_RELAY);
                   subDelayFlag = 0;
               }
           }
}
int main(void){

    init_io();        //skonfiguruj piny
    init_timers();    //skonfiguruj liczniki
    init_uart();
  /*  mcusr=MCUSR;

      if(mcusr&0x01)send_uart("W");
      if(mcusr&0x02)send_uart("E");
      if(mcusr&0x04)send_uart("B");
      if(mcusr&0x08)send_uart("P");
*/
    while(1){

       handleKeys();
       handleSerial();
       handleDelaySpeekerOn();

    }
    return 0;
}
