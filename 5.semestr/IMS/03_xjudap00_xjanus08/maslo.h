/**
 * Project: IMS - Model vyrobni a balici linky masla
 * @author: Petr Juda  (xjudap00), Filip Janus (xjanus08)
 * @file: maslo.h
 *
 * Procesy(tridy) odpovidajici vyrobni lince masla.
 *
 */
#ifndef IMS_MASLO_H
#define IMS_MASLO_H

#include <iostream>
#include <simlib.h>
#include <ctime>
#include <cstdlib>

// Pocet zmaselneni od posledni udrzby
extern unsigned pocet_zmaselneni;

class sanitace :public Process{
    /**
     * @brief Cisteni zmaslovaciho stroje.
     *
     */
    void Behavior();
};

class udrzba :public Process {
    /**
     * @brief Udrzba zmaslovaciho stroje.
     *
     */
    void Behavior();
};

class zmaselneni :public Process {
public:
    /**
     * @brief Nastaveni mnozstvi smetany do zmaselnovace.
     *
     * @param mnozstvi Pocet litru smetany v jednom cyklu zmaselneni.
     *
     */
    void mnozstvi_masla(unsigned long mnozstvi);

    /**
     * @brief Nastaveni intervalu doby zmaselnovani.
     *
     * @param i1 Spodni hranice intervalu.
     *
     * @param i2 Horni hranice intervalu.
     *
     */
    void cas(double i1, double i2);

    /**
     * @brief Premena smetany na maslo.
     *
     */
    void Behavior();
private:
    //! Mnozstvi smetany do zmaselnovaciho stroje.
    unsigned long smetana;
    //! Spodni hranice casu zmaselneni smetany.
    double interval1;
    //! Horni hranice casu zmaselneni smetany.
    double interval2;
};

class cerpani :public Process{
public:
    /**
     * @brief Nastaveni casu precerpani 1kg masla.
     *
     * @param cas Doba presunu 1kg masla.
     *
     */
    void cas(double cas);

    /**
     * @brief Precerpavani masla.
     *
     */
    void Behavior();

private:
    //! Cas precerpani 1kg.
    double cas_cerpani;
};

class plneni:public Process {
public:
    /**
     * @brief Inicializace atributu.
     *
     * @param mnozstvi Mnozstvi smetany na vstupu.
     *
     * @param silo Mnozstvi masla v silu.
     *
     */
    explicit plneni(unsigned long mnozstvi, unsigned long silo);

    /**
     * @brief Udrzba zmaslovaciho stroje.
     *
     */
    void Behavior();

private:
    //! Pocatecni mnozstvi smetany.
    unsigned long mnozstvi;
    //! Pocatecni mnozstvi masla v zasobnikovem silu.
    unsigned long silo;
};

#endif //IMS_MASLO_H
