/*
 * menu.c
 *
 *  Created on: 18 mar 2015
 *  Author: Marcin Bialowas -  mbialow@gmail.com
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <avr/wdt.h>
#include <stdlib.h>


#include "ds18x20/onewire.h"
#include "ds18x20/ds18x20.h"
#include "main.h"
#include "debug.h"
#include <string.h>

volatile StanUkladu stanUkladu;

ISR(TIMER1_COMPA_vect){
    stanUkladu.stanCzujnika = ODCZYTAJ_TEMPERATURE;
    stanUkladu.licznikOdczytowTemperatury++;
    timer1_A_stop();
}

ISR(TIMER1_COMPB_vect){
    stanUkladu.czasPracyPompy++;
    if(stanUkladu.stanPompy == WLACZONA && stanUkladu.czasPracyPompy > MAKSYMALNY_CZAS_PRACY_POMPY_SEKUNDY){
        timer1_B_stop();
        wylacz_pompe();
    }
}


static void timer1_init(){

    // timer0 ustawienie  prescaler = 1024
    TCCR1B |= (1 << CS12)|(1 << CS10);
    //tryb CTC
    TCCR1B |= (1 << WGM12);

    //przerwanie co 1s
    OCR1A = 976;
    //dla 8MHZ - 8000
    //OCR1A = 8000;
}


static void timer1_A_start(){
    TIMSK |= (1 << OCIE1A);
}

static void timer1_A_stop(){
    TIMSK &= ~(1 << OCIE1A);
}


static void timer1_B_start(){
    TIMSK |= (1 << OCIE1B);
}

static void timer1_B_stop(){
    TIMSK &= ~(1 << OCIE1B);
}

static void io_init(){
//pin pompy jako wyjscie
    POMPA_DDR  |=  ( 1 << POMPA_PIN );
//stan niski na pompe
    POMPA_PORT &= ~( 1 << POMPA_PIN );
}

static void zalacz_pompe(){
    stanUkladu.stanPompy = WLACZONA;
    stanUkladu.czasPracyPompy = 0;
    //stan wysoki na pin
    POMPA_PORT |= ( 1 << POMPA_PIN );
}

static void wylacz_pompe(){
    stanUkladu.stanPompy = WYLACZONA;
    //0 na odpowiedni pin
    POMPA_PORT &= ~( 1 << POMPA_PIN );
}


int main(void){

//ustaw startowe parametry
    stanUkladu.poprzedniOdczytTemperatury = 0xFF;
    stanUkladu.stanPompy = WYLACZONA;

#ifdef DEBUG_USART

    init_uart();

#endif

    io_init();
    timer1_init();

    DS18X20_StartMeasurement();
    timer1_A_start();

    wdt_enable(WDTO_500MS);
    sei();

    uint8_t tempOdczytanaCzescCalkowita = 0x00;

    for(;;){

        wdt_reset();
        if(stanUkladu.stanCzujnika == ODCZYTAJ_TEMPERATURE && stanUkladu.stanPompy == WYLACZONA){
            stanUkladu.stanCzujnika = SPOCZYNEK;

            if(DS18X20_OK == DS18X20_ReadTemperature()){
                tempOdczytanaCzescCalkowita = (( temperature >> 4 ) & 0x7F );

#ifdef DEBUG_USART

            printTmep(tempOdczytanaCzescCalkowita);
#endif

                if(stanUkladu.stanPompy == WYLACZONA && (tempOdczytanaCzescCalkowita - stanUkladu.poprzedniOdczytTemperatury >= HISTEREZA)){
                    zalacz_pompe();
                    timer1_B_start();
                }

                if(stanUkladu.licznikOdczytowTemperatury > LICZNIK_ODCZYTOW_TEMPERATURY){
                    stanUkladu.poprzedniOdczytTemperatury = tempOdczytanaCzescCalkowita;
                    stanUkladu.licznikOdczytowTemperatury = 0;
                }
            }



        }

        if(!DS18X20_IsInProgress()){
            DS18X20_StartMeasurement();
            timer1_A_start();
        }

    }

   return 0;
}

