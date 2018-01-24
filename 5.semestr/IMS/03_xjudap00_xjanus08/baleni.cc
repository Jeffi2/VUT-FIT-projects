/**
 * Project: IMS - Model vyrobni a balici linky masla
 * @author: Petr Juda  (xjudap00), Filip Janus (xjanus08)
 * @file: baleni.cc
 *
 * Procesy(tridy) odpovidajici balici lince masla.
 *
 */

#include "baleni.h"
#include "zarizeni.h"

// Inicializace globalnich promenych
bool potreba_maslo = true;
int vyrobene_kostky = 0;
int kg_maslo_v_krabici = 0;

void doplneni_folie::Behavior() {
    Priority = 2;
    int zam_ukazatel = -1;
    back:
    // Nalezeni volneho pracovnika
    if ((zam_ukazatel = volny_pracovnik()) == -1) {
        // Cekame na pracovnika s vyssi prioritou nez maslo k baleni
        Fronta_na_pracovniky.Insert(this);
        Passivate();
        goto back;
    }
    Seize(Pracovnici[zam_ukazatel]);
    Seize(Balici_stroj, 1);
    // Doplneni folie do baliciho stroje
    Wait(Uniform(2*60, 5*60));
    Release(Balici_stroj);
    Release(Pracovnici[zam_ukazatel]);
    // Zapne balici stroj
    (Cekani_na_doplneni_folie.GetFirst())->Activate();
}

void baleni::cas(double cas) {
    this->cas_baleni=cas;
}

void baleni::Behavior() {
    back:
        Seize(Balici_stroj);
        if (vyrobene_kostky == config.folie ) {
            // Dosla balici folie
            Release(Balici_stroj);
            (new doplneni_folie)->Activate();
            // Stroj ceka na dokonceni plneni
            Cekani_na_doplneni_folie.Insert(this);
            Passivate();
            vyrobene_kostky = 0;
            goto back;
        }
        if (potreba_maslo)
            if (Silo.Used()>=1) {
                // Kilo masla do baliciho stroje
                Leave(Silo,1);
                potreba_maslo = false;
            }
            else{
                CekaniNaMaslo(Time);
                // Zasobnikove silo je prazdne
                Cekani_na_baleni.Insert(this);
                Release(Balici_stroj);
                double cas_cekani=Time;
                Passivate();
                goto back;
            }
        Wait(this->cas_baleni);
        ++vyrobene_kostky;
        if ((vyrobene_kostky % 4) == 0) {
            // Z 1kg vyrobime 4 kostky masla
            potreba_maslo = true;
        }

        if (Random() <= 0.02) {
            // Spatne zabalene maslo
            Enter(Znehodnoceno, 1);
        }
        else {
            // Kostka masla na pas
            Enter(Balici_plocha, 1);
        }
        Release(Balici_stroj);

        back2:
        // 20 kostek na pase
        if (Balici_plocha.Used() == 20) {
            // kapacita pasu
            if(Fronta_na_pracovniky.size() >= 10){
                // pas je plny
                Cekani_na_volne_misto.Insert(this);
                CekaniNaVolnyPas(Time);
                Passivate();
                goto back2;
            }
            else {
                // zabaleni masel do krabice
                (new maslo)->Activate();
                Leave(Balici_plocha,20);
            }
        }
    goto back;
}

pauza::pauza(int cislo_pauzy) {
    this->pocitadlo_pauza = cislo_pauzy;
    Priority = 3;
}

void pauza::Behavior() {
    back:
    pocitadlo_pauza++;
    Seize(Balici_stroj);
    for (int i = 0; i < pocet_pracovniku; ++i) {
        Seize(Pracovnici[i]);
    }

    if (pocitadlo_pauza == 1) {
        // prvni pauza
        Wait(10*60);
        Release(Balici_stroj);
        for (int i = 0; i < pocet_pracovniku; ++i) {
            Release(Pracovnici[i]);
        }
        if (!Fronta_na_pracovniky.Empty()) {
            (Fronta_na_pracovniky.GetFirst())->Activate();
        }
        if (!Cekani_na_volne_misto.Empty()) {
            (Cekani_na_volne_misto.GetFirst())->Activate();
        }
        Activate(Time+120*60);
    }
    else if (pocitadlo_pauza == 2) {
        // obedova pauza
        Wait(30*60);
        Release(Balici_stroj);
        for (int i = 0; i < pocet_pracovniku; ++i) {
            Release(Pracovnici[i]);
        }
        if (!Fronta_na_pracovniky.Empty()) {
            (Fronta_na_pracovniky.GetFirst())->Activate();
        }
        if (!Cekani_na_volne_misto.Empty()) {
            (Cekani_na_volne_misto.GetFirst())->Activate();
        }
        Activate(Time+110*60);
    }
    else {
        // posledni pauza
        Wait(10*60);
        Release(Balici_stroj);
        for (int i = 0; i < pocet_pracovniku; ++i) {
            Release(Pracovnici[i]);
        }
        if (!Fronta_na_pracovniky.Empty()) {
            (Fronta_na_pracovniky.GetFirst())->Activate();
        }
        if (!Cekani_na_volne_misto.Empty()) {
            (Cekani_na_volne_misto.GetFirst())->Activate();
        }
        return;
    }

    goto back;
}


void maslo::Behavior() {
    int zam_ukazatel = -1;

back:
    if ((zam_ukazatel = volny_pracovnik()) == -1) {
        Fronta_na_pracovniky.Insert(this);
        Passivate();
        goto back;
    }
    Seize(Pracovnici[zam_ukazatel]);
    Wait(Uniform(10, 15));
    Release(Pracovnici[zam_ukazatel]);
    kg_maslo_v_krabici += 5;
    MasloCasBaleni(Time);
    if (!Fronta_na_pracovniky.Empty()) {
        (Fronta_na_pracovniky.GetFirst())->Activate();
    }
    if (!Cekani_na_volne_misto.Empty()) {
        (Cekani_na_volne_misto.GetFirst())->Activate();
    }
}
