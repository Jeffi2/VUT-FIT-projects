/**
 * Project: IMS - Model vyrobni a balici linky masla
 * @author: Petr Juda  (xjudap00), Filip Janus (xjanus08)
 * @file: baleni.h
 *
 * Procesy(tridy) odpovidajici balici lince masla.
 *
 */

#ifndef IMS_BALENI_H
#define IMS_BALENI_H
#include <iostream>
#include <simlib.h>
#include <ctime>
#include <cstdlib>

//! True, pokud balici stroj potrebuje maslo ze Sila.
extern bool potreba_maslo;

//! Pocet vyrobenych kostek masla.
extern int vyrobene_kostky;

//! Pocet kg zabaleneho masla v krabicich.
extern int kg_maslo_v_krabici;


class baleni :public Process{
public:
    /**
     * @brief Nastaveni casu potrebneho na zabaleni jednoho masla.
     *
     * @param cas Cas baleni jedne kostky masla.
     *
     */
    void cas(double cas);

    /**
     * @brief Baleni masla do kostek.
     *
     */
    void Behavior();

private:
    //! Cas baleni kostky masla
    double cas_baleni;
};

class doplneni_folie :public Process {
public:
    /**
     * @brief Doplneni balici folie.
     *
     */
    void Behavior();
};

class pauza :public Process {
public:
    /**
     * @brief Nastaveni prvni pauzy.
     *
     * @param cislo_pauzy Nastaveni prvni pauzy.
     *
     */
    explicit pauza(int cislo_pauzy);

    /**
     * @brief Pauza pracovniku.
     *
     */
    void Behavior();
private:
    //! Oznacuje jak dlouho bude trvat pauza.
    int pocitadlo_pauza;
};

class maslo :public Process{
public:
    /**
     * @brief Baleni masla do krabice.
     *
     */
    void Behavior();
};
#endif //IMS_BALENI_H
