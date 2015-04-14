/*
 * menu.h
 *
 *  Created on: 18 mar 2015
 *  Author: Marcin Bialowas -  mbialow@gmail.com
 */

#ifndef MENU_H_
#define MENU_H_


#define POMPA_PORT PORTB
#define POMPA_DDR DDRB
#define POMPA_PIN PB1

#define HISTEREZA 2

//odczyt bedzie przypisany jako poprzedni po przekroczeniu tej wartosci
#define LICZNIK_ODCZYTOW_TEMPERATURY 20

#define MAKSYMALNY_CZAS_PRACY_POMPY_SEKUNDY 60*5 //(60 s * 5 minut)


typedef enum tStan {
    PRACA_POMPY = 0x01,
    ODCZYTAJ_TEMEPERATURE,
    SPOCZYNEK

} Stan;

typedef struct tStanUkladu {
    uint8_t      poprzedniOdczytTemperatury;
    Stan         stan;
    uint16_t     czasPracyPompySekundy;
    uint8_t      licznikOdczytowTemperatury;

} StanUkladu ;


static void wylacz_pompe();
static void zalacz_pompe();

static void timer1_init();
static void timer1_stop();
static void timer1_start();
static void timer1_przerwanie_compare_match_A_start();
static void timer1_przerwanie_compare_match_A_stop();
static void timer1_przerwanie_compare_match_B_start();
static void timer1_przerwanie_compare_match_B_stop();

static void io_init();

#endif /* MENU_H_ */
