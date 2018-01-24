/**
 * Project: IMS - Model vyrobni a balici linky masla
 * @author: Petr Juda  (xjudap00), Filip Janus (xjanus08)
 * @file: zarizeni.cc
 *
 * Deklarace globalnich objektu z knihovny SIMLIB.
 *
 */
#include "zarizeni.h"

Store Smetana("Zasobnik smetany",49000);
Store Maslo("Vystup masla ze zmaselnovace.",100000);
Store Silo("Zasobnikove silo masla",100000);
Store Znehodnoceno("Poskozene kostky masla", 10000);
Store Balici_plocha("Balici pas.", 500);

Facility Balici_stroj("Balici stroj.");
Facility Zmaselnovac ("Zmaselnovaci stroj.");
Facility Cerpadlo ("Cerpadlo masla.");

TConfig config;
Queue Fronta_na_pracovniky("Cekani na obslouzeni pracovnikem.");
Queue Cekani_na_maslo("Precerpani do skladu masla.");
Queue Cekani_na_zmaselnovac("Cekani na pripraveny zmaselnovac.");
Queue Cekani_na_sanitaci("Cekani na umyti zmaselnovace.");
Queue Cekani_na_udrzbu("Cekani na udrzbu.");
Queue Cekani_na_baleni("Cekani na baleni.");
Queue Cekani_na_doplneni_folie("Cekani na doplneni folie.");
Queue Cekani_na_volne_misto("Cekani na uvolneni mista na pase.");

Histogram MasloCasBaleni("Pocet zabalenych krabic masel za hodinu.", 0, 3600, 9);
Histogram Mnozstvicerpani("Pocet precerpaneho masla.", 0, 3600, 8);
Histogram CekaniNaMaslo("Pocet zastaveni baleni kvuli nedostatku masla.",0,3600,8);
Histogram CekaniNaVolnyPas("Pocet zastaveni baleni, kvuli obsazenemu pasu.", 0, 3600,8);


int pocet_pracovniku = 0;
Facility *Pracovnici;

int volny_pracovnik() {
    for (int i = 0; i < pocet_pracovniku; ++i) {
        if (!Pracovnici[i].Busy())
            return i;
    }
    return -1;
}
