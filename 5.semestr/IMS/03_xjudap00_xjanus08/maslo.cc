/**
 * Project: IMS - Model vyrobni a balici linky masla
 * @author: Petr Juda  (xjudap00), Filip Janus (xjanus08)
 * @file: maslo.cc
 *
 * Procesy(tridy) odpovidajici vyrobni lince masla.
 *
 */

#include "maslo.h"
#include "zarizeni.h"

unsigned pocet_zmaselneni=0;

void sanitace::Behavior() {
    while(1) {
        Wait(Uniform(17*60,29*60));
        if (pocet_zmaselneni == 3) {
            // Udrzba zmaselnovaciho stroje
            (Cekani_na_udrzbu.GetFirst())->Activate();
        } else {
            // Nove zmaselnovani smetany
            (Cekani_na_zmaselnovac.GetFirst())->Activate();
        }
        Cekani_na_sanitaci.Insert(this);
        Passivate();
    }
}

void udrzba::Behavior() {
    while(1) {
        Wait(Uniform(15*60,25*60));
        Cekani_na_udrzbu.Insert(this);
        pocet_zmaselneni = 0;
        // Nove zmaselnovani smetany
        (Cekani_na_zmaselnovac.GetFirst())->Activate();
        Passivate();
    }

}

void zmaselneni::Behavior() {
    while(1) {
        unsigned long pouzita_smetana = 0;
        if (!Smetana.Empty()) {
            //
            if (Smetana.Used() >= this->smetana) {
                // Naplneni plneho zmaselnovace
                pouzita_smetana = this->smetana;
            }
            else {
                // Pouziti pouze zbytku smetany v zasobniku
                pouzita_smetana = Smetana.Used();
            }
            // Precerpani smetany do stroje (20.5l za 1s) 5 cerpadel https://www.sigmashop.cz/potravinarska-cerpadla/50-npb-180-10-yc-04-400v
            Wait(pouzita_smetana / 20.5);

            // Naplneni zmaselnovaciho stroje
            Leave(Smetana, pouzita_smetana);
        } else {
            //Dosla smetana neni co zmaselnovat
            return;
        }
        Seize(Zmaselnovac);

        // Vyroba masla
        Wait(Uniform(this->interval1*3600,this->interval2*3600));
        Release(Zmaselnovac);
        Enter(Maslo, pouzita_smetana/2);
        pocet_zmaselneni++;

        if (Cekani_na_maslo.Length()>0)
            // Pokud nebezi precerpavani masla, aktivuje me jej
            (Cekani_na_maslo.GetFirst())->Activate();

        if(!Cekani_na_sanitaci.Empty())
            // Sanitace zmaselnovaciho stroje
            (Cekani_na_sanitaci.GetFirst())->Activate();

        // Vyckani na dokonceni predchozich procesu
        Cekani_na_zmaselnovac.Insert(this);
        Passivate();
    }
}

void zmaselneni::cas(double i1, double i2) {
    this->interval1=i1;
    this->interval2=i2;
}

void zmaselneni::mnozstvi_masla(unsigned long mnozstvi) {
    this->smetana=mnozstvi;
}

void cerpani::Behavior() {
    double cas=Time;
    while (1) {
        if (!Maslo.Empty()) {
            Seize(Cerpadlo);
            Leave(Maslo,1);
            // Precerpani 1 kg masla do Sila
            Wait(this->cas_cerpani);
            Enter(Silo, 1);
            Release(Cerpadlo);
            Mnozstvicerpani(Time-cas);
            if(!Cekani_na_baleni.Empty())
                (Cekani_na_baleni.GetFirst())->Activate();
        }else{
            // Vsechno maslo je precerpano do Sila
            Cekani_na_maslo.Insert(this);
            Passivate();
        }
    }
}
void cerpani::cas(double cas) {
    this->cas_cerpani=cas;
}

plneni::plneni(unsigned long mnozstvi, unsigned long silo) {
    this->mnozstvi=mnozstvi;
    this->silo=silo;
}

void plneni::Behavior() {
    Enter(Smetana,mnozstvi);
    Enter(Silo,silo);
    if (!Cekani_na_baleni.Empty() && silo > 0)
        (Cekani_na_baleni.GetFirst())->Activate();
}