/**
 * Project: IMS - Model vyrobni a balici linky masla
 * @author: Petr Juda  (xjudap00), Filip Janus (xjanus08)
 * @file: main.cpp
 *
 * Hlavni rizeni programu.
 *
 */
#include <iostream>
#include <simlib.h>
#include <ctime>
#include "maslo.h"
#include "baleni.h"
#include "zarizeni.h"
#include <fstream>
#include <sstream>
#include <cstring>

/**
 * @brief Prevod hodin na sekundy
 */
#define  H_S(x) x*3600

/**
 * @brief Funkce nacitajici konfiguracni informace ze souboru
 *
 * @param cislo_souboru Cislo init souboru.
 *
 * @return TConfig strukturu s naplnenymi konfiguracnimi daty.
 *
 */
TConfig nacteniKonfigurace(char * cislo_souboru){
    std::ifstream soubor;
    std::string nazev = "init";
    nazev.append(cislo_souboru);
    soubor.open(nazev.c_str());
    if (!soubor.is_open()){
        std::cerr<<"Nelze otevřít inicializační soubor!" << std::endl;
        exit(1);
    }
    std::string line;
    int i=0;
    std::string x;
    TConfig config{};
    try {
    while(std::getline(soubor,line)) {
        std::istringstream radek (line);
        switch (i) {
            case 0:
                radek >>x >>config.cas;
                break;
            case 1:
                radek>>x >>config.kapacita;
                break;
            case 2:
                radek>>x >>config.smetana;
                break;
            case 3:
                radek>>x >>config.vstup_zmaselnovace;
                break;
            case 4:
                radek>>x >>config.i1>>config.i2;
                break;
            case 5:
                radek>>x >>config.cerpadlo;
                break;
            case 6:
                radek>>x >>config.balicka;
                break;
            case 7:
                radek>>x >>config.krabice;
                break;
            case 8:
                radek>>x >>config.obsluha;
                break;
            case 9:
                radek>>x>>config.linka;
                break;
            case 10:
                radek>>x>>config.silo;
                break;
            case 11:
                radek>>x>>config.folie;
                break;
            default:{}

        }
        i++;
    }
    } catch (...) {
        soubor.close();
        std::cerr << "Špatná struktura inicializačního souboru!" << std::endl;
        exit(1);
    }
    if (i != 12) {
        soubor.close();
        std::cerr << "Špatná struktura inicializačního souboru!" << std::endl;
        exit(1);
    }
    soubor.close();
    return config;
}

int main(int argc, char *argv[]) {
    if (argc == 2) {
        // Nacteni konfigurace z init souboru
        config = nacteniKonfigurace(argv[1]);
    } else if (argc == 1) {
        // Pouziti vychozi konfigurace
        config.vstup_zmaselnovace = 7000;
        config.kapacita = 49000;
        config.smetana = 49000;
        config.krabice = 20;
        config.balicka = 0.4;
        config.cas = 8.5;
        config.cerpadlo = 1;
        config.i2 = 1.38;
        config.i1 = 1.03;
        config.obsluha = 2;
        config.linka = 200;
        config.silo = 0;
        config.folie = 16000;
    } else {
        std::cerr << "Špatně zadané programové parametry." << std::endl << "Použití: ./ims [init-file number]"
                  << std::endl;
        exit(1);
    }

    // Nastaveni vystupniho souboru
    SetOutput("statistiky.txt");
    // Nastaveni simulacniho casu
    Init(0, H_S(config.cas));

    // Nastaveni pro nahodne generatory
    RandomSeed(std::time(0));

    // Inicializace kapacity Store objektu
    Smetana.SetCapacity(config.kapacita);
    Balici_plocha.SetCapacity(config.linka);

    // Vlozeni pocatecniho mnozstvi smetany a masla v silu
    (new plneni(config.smetana, config.silo))->Activate();

    // Naplanovani procesu zmaselneni smetany
    zmaselneni *tmp_zmaselneni = new zmaselneni;
    tmp_zmaselneni->cas(config.i1, config.i2);
    tmp_zmaselneni->mnozstvi_masla(config.vstup_zmaselnovace);
    tmp_zmaselneni->Activate();

    // Naplanovani procesu precerpavani smetany
    cerpani *tmp_cerpani = new cerpani;
    tmp_cerpani->cas(config.cerpadlo);
    tmp_cerpani->Activate();

    // Vytvoreni procesu sanitace
    sanitace *tmp_sanitace = new sanitace;
    tmp_sanitace->Passivate();
    // Vstoupi do fronty a ceka, nez bude zmaselnena smetana
    Cekani_na_sanitaci.Insert(tmp_sanitace);

    // Vytvoreni procesu udrzby zmaselnovaciho stroje
    udrzba *tmp_udrzba = new udrzba;
    tmp_udrzba->Passivate();
    // Vstoupi do fronty a ceka, nez bude potreba provest udrzbu
    Cekani_na_udrzbu.Insert(tmp_udrzba);

    // Vytvoreni procesu balici linky
    baleni *tmp_baleni = new baleni;
    // Nastavi rychlost baliciho stroje
    tmp_baleni->cas(config.balicka);
    tmp_baleni->Passivate();
    Cekani_na_baleni.Insert(tmp_baleni);

    // Vytvori pole pracovniku obsluhujicich balici linku
    Facility tmp[config.obsluha];
    Pracovnici = (*tmp);
    pocet_pracovniku = config.obsluha;

    // Naplanuje prvni pauzu pracovniku
    (new pauza(0))->Activate(110*60);

    // Spusteni simulace
    Run();

    // Vysledky simulace
    Smetana.Output();
    Zmaselnovac.Output();
    Maslo.Output();
    Silo.Output();
    Fronta_na_pracovniky.Output();
    Balici_stroj.Output();
    for (int j = 0; j < config.obsluha; ++j) {
        Pracovnici[j].Output();
    }
    MasloCasBaleni.Output();
    Mnozstvicerpani.Output();
    CekaniNaMaslo.Output();
    CekaniNaVolnyPas.Output();

    // Simlib print hack
    std::string s1 = "Celkem zabaleno: ";
    std::string s2 = " kg masla.\n";
    std::string s3 = "Celkem poškozeno: ";
    double poskozene = (Znehodnoceno.Used() / 4.0);
    Print(s1.data()); Print(kg_maslo_v_krabici); Print(s2.data());
    Print(s3.data()); Print(poskozene); Print(s2.data());

    std::cout << "Output uložen do souboru statistiky.txt" << std::endl;

    return 0;
}
