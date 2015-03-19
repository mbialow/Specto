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
#define POMPA_PIN PB2

#define HISTEREZA 2

//odczyt bedzie przypisany jako poprzedni po przekroczeniu tej wartosci
#define LICZNIK_ODCZYTOW_TEMPERATURY 3

#define MAKSYMALNY_CZAS_PRACY_POMPY_SEKUNDY 60*10 //(60 s * 10 minut)


typedef enum StanPompy {
    WLACZONA = 0x01,
    WYLACZONA

} StanPompy;

typedef enum tStanCzujnika {
    SPOCZYNEK = 0x00,
    ODCZYTAJ_TEMPERATURE

} StanCzujnika;

typedef struct tStanUkladu {
    uint8_t      poprzedniOdczytTemperatury;
    StanPompy    stanPompy;
    StanCzujnika stanCzujnika;
    uint16_t     czasPracyPompy;
    uint8_t      licznikOdczytowTemperatury;

} StanUkladu ;


static void wylacz_pompe();
static void zalacz_pompe();

static void timer1_init();
static void timer1_A_start();
static void timer1_A_stop();
static void timer1_B_start();
static void timer1_B_stop();

static void io_init();

#endif /* MENU_H_ */
