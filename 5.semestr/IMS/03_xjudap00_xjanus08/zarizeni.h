/**
 * Project: IMS - Model vyrobni a balici linky masla
 * @author: Petr Juda  (xjudap00), Filip Janus (xjanus08)
 * @file: zarizeni.h
 *
 * Deklarace globalnich objektu z knihovny SIMLIB.
 *
 */

#ifndef IMS_ZARIZENI_H
#define IMS_ZARIZENI_H

#include <simlib.h>

//! Zasobnik smetany
extern Store Smetana;
//! Vystup masla ze zmaselnovace
extern Store Maslo;
//! Zasobnikove silo masla
extern Store Silo;
//! Spatne zabalene kusy
extern Store Znehodnoceno;
//! Transportni pas
extern Store Balici_plocha;

//! Stroj balici maslo do kostek
extern Facility Balici_stroj;
//! Zmaselnovaci stroj
extern Facility Zmaselnovac;
//! Cerpadlo
extern Facility Cerpadlo;
//! Pracovnici balici linky
extern Facility* Pracovnici;

// Fronta do ktere se uspi cerpani pokud neni co precerpavat
extern Queue Cekani_na_maslo;
// Fronta do ktere se uspi zmaselnovani dokud neni dokoncena sanitace/udrzba
extern Queue Cekani_na_zmaselnovac;
// Fronta ve ktere ceka sanitace na probuzeni
extern Queue Cekani_na_sanitaci;
// Fronta ve ktere ceka udrzba na probuzeni
extern Queue Cekani_na_udrzbu;
// Fronta do ktere se uspi balici stroj prokud neni maslo
extern Queue Cekani_na_baleni;
// Fronta ve ktere ceka balici stroj na doplneni folie
extern Queue Cekani_na_doplneni_folie;
// Fronta ve ktere ceka proces baleni na uvolneni dopravnikoveho pasu
extern Queue Cekani_na_volne_misto;
// Fronta do ktere se radi procesy cekajici na obsluhu zamestnanci
extern Queue Fronta_na_pracovniky;

extern Histogram MasloCasBaleni;
extern Histogram CekaniNaMaslo;
extern Histogram CekaniNaVolnyPas;
extern Histogram Mnozstvicerpani;

//! Pocet zametnanych pracovniku
extern int pocet_pracovniku;

/**
 * @brief Nalezeni volneho pracovnika
 *
 * @return Index na volneho pracovnika do pole Pracovnici, jinak -1.
 *
 */
int volny_pracovnik();

//! Inicializacni struktura
typedef struct config{
    double cas;
    unsigned long kapacita;
    unsigned long smetana;
    unsigned long vstup_zmaselnovace;
    double i1;
    double i2;
    double cerpadlo;
    double balicka;
    unsigned krabice;
    unsigned obsluha;
    unsigned linka;
    unsigned long silo;
    unsigned long folie;
}TConfig;

//! Predani vstupnich parametru
extern TConfig config;

#endif //IMS_ZARIZENI_H
