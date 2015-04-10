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

volatile StanUkladu stanUkladu;
//StanUkladu EEMEM ss;

ISR(TIMER1_COMPA_vect){
    stanUkladu.stanCzujnika = ODCZYTAJ_TEMPERATURE;
    stanUkladu.licznikOdczytowTemperatury++;
    timer1_przerwanie_compare_match_A_stop();
}

ISR(TIMER1_COMPB_vect){
    stanUkladu.czasPracyPompy++;
    if(stanUkladu.stanPompy == WLACZONA && stanUkladu.czasPracyPompy > MAKSYMALNY_CZAS_PRACY_POMPY_SEKUNDY){
        timer1_przerwanie_compare_match_B_stop();
        wylacz_pompe();
    }
}

static void timer1_stop(){
    TCCR1B &= ~(1 << CS12)|(1 << CS10);
}

static void timer1_start(){
    // timer1 ustawienie  prescaler = 1024
    TCCR1B |= (1 << CS12)|(1 << CS10);
}

static void timer1_init(){
    //tryb CTC
    TCCR1B |= (1 << WGM12);
    //przerwanie co 1s
    OCR1A = 976;
}


static void timer1_przerwanie_compare_match_A_start(){
    TIMSK |= (1 << OCIE1A);
    timer1_start();
}

static void timer1_przerwanie_compare_match_A_stop(){
    TIMSK &= ~(1 << OCIE1A);
    timer1_stop();
}


static void timer1_przerwanie_compare_match_B_start(){
    TIMSK |= (1 << OCIE1B);
    timer1_start();
}

static void timer1_przerwanie_compare_match_B_stop(){
    TIMSK &= ~(1 << OCIE1B);
    timer1_stop();
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

//ustaw domyslne parametry
    stanUkladu.poprzedniOdczytTemperatury = 0xFF;
    stanUkladu.licznikOdczytowTemperatury = LICZNIK_ODCZYTOW_TEMPERATURY;
    stanUkladu.stanPompy = WYLACZONA;
    stanUkladu.stanCzujnika = SPOCZYNEK;

#ifdef DEBUG_USART

    init_uart();

#endif

    io_init();
    timer1_init();

    //DS18X20_StartMeasurement();
    //timer1_przerwanie_compare_match_A_start();

    wdt_enable(WDTO_500MS);
    sei();

    uint8_t tempOdczytanaCzescCalkowita = 0x20;

    for(;;){

        wdt_reset();

        if (stanUkladu.stanPompy == WYLACZONA) {

            if (stanUkladu.stanCzujnika == ODCZYTAJ_TEMPERATURE) {
                stanUkladu.stanCzujnika = SPOCZYNEK;

                if (DS18X20_OK == DS18X20_ReadTemperature()) {
                    tempOdczytanaCzescCalkowita = (temperature >> 4);

#ifdef DEBUG_USART

                    printTemp(tempOdczytanaCzescCalkowita);
#endif

                    if (tempOdczytanaCzescCalkowita - stanUkladu.poprzedniOdczytTemperatury >= HISTEREZA) {
                        zalacz_pompe();
                        timer1_przerwanie_compare_match_B_start();
                        stanUkladu.licznikOdczytowTemperatury = LICZNIK_ODCZYTOW_TEMPERATURY;
                    }

                    if (stanUkladu.licznikOdczytowTemperatury >= LICZNIK_ODCZYTOW_TEMPERATURY) {
                        stanUkladu.poprzedniOdczytTemperatury = tempOdczytanaCzescCalkowita;
                        stanUkladu.licznikOdczytowTemperatury = 0;
                    }
                }

            }

            if (!DS18X20_IsInProgress()) {
                DS18X20_StartMeasurement();
                timer1_przerwanie_compare_match_A_start();
            }

        }

    }

   return 0;
}

