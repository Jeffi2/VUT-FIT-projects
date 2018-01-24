//
//  main.c
//  FIT_Projekt1
//
//  Created by Petr Juda on 11.10.15.
//  Created in Xcode.
//  Copyright © 2015 Petr Juda. All rights reserved.
//

#include <stdio.h>
#include <stdbool.h>
#include <limits.h>
#include <time.h>

struct tm time_str; /* Deklarace struktury pro mktime. */

//Deklarace funkci
void JeCislo(int i, char retezec[]);
void JePalindrom(int i, char retezec[]);
void JeDatum(int i, char retezec[]);


int main(int argc, char *argv[]) {
  if (argc > 1) { /* Rozhoduje podle argumentu, zda program vypise funkci nebo cte ze souboru. */
     printf("Program %s nacita text ze vstupu.\nPodle zadani vypisuje, zda se jedna o cislo, datum, slovo nebo palindrom.\nAutorem programu je: xjudap00.\nVytvoreno 11.10.2015.\n", argv[0]);
     return 0;
  }
  else
  {
      char retezec[101];
      int code;
      bool prazdny = true; /* Promena prazdny na true, kvuli hlidani prazdneho souboru. */
    
      while ((code = (scanf("%100s", retezec))) != EOF) /* Dokud neni konec souboru nacitej retezec. */
      {
       
        bool cislo = true;
        bool datum = false;
        prazdny = false; /* Pokud se provede nacist, prazdny se prehodi na false. */
  
        int i = 0;
        while (retezec[i] != '\0') { /* Dokud nejsi na konci retezce delej. */
          i++; /* Do promenne i se zapise delka retezce. */
          }
        
  //ZDE ZACINA TESTOVANI CISLA
        
        for (int l = 0; l != i; l++) {
         if ((retezec[l]) < '0' || retezec[l] > '9') { /* Porovnava hodnotu znaku s 0 a 9, urcuje zda se jedna o cislo. */
            cislo = false; /* Pokud se jedná o cislo, zustava true. */
         }
        }
  //ZDE KONCI TESTOVANI CISLA
        
  //ZDE ZACINA TESTOVANI FORMATU DATA
        if ((i == 10) && (retezec[0] > '0' && retezec[0] <= '9') && (retezec[1] >= '0' && retezec[1] <= '9') &&
            (retezec[2] >= '0' && retezec[2] <= '9') && (retezec[3] >= '0' && retezec[3] <= '9') && (retezec[4] == '-') &&
            (retezec[5] >= '0' && retezec[5] <= '9') && (retezec[6] >= '0' && retezec[6] <= '9') && (retezec[7] == '-') &&
            (retezec[8] >= '0' && retezec[8] <= '9') && (retezec[9] >= '0' && retezec[9] <= '9') ) {
          /* Podminka, ktera se chova jako maska. */
          datum = true; /* Pokud maska vyhovuje, datum se nastavi na true a spusti se test datumu. */
        }
        
        if (cislo == true) {
          JeCislo(i, retezec);
          }
        else {
          if (datum == true) {
            JeDatum(i, retezec);
          }
          else {
            JePalindrom(i, retezec);
          }
         }
        }
      if ((code != 1) && (prazdny == true)) { /* Hlida, zda se na vstupu neobjevi prazdny soubor, pokud ano vypise chybu. */
        printf("Prazdny soubor na vstupu!\n");
        return 1;
      }
    }
  
  
  return 0;
}
//KONEC MAIN

void JeCislo(int i, char retezec[]) {
  
  bool nula = true;
  bool prvocislo =true;
  int pocet_nul = 1;
  
  if (retezec[0] == '0') { /* Pokud cislo zacina nulou delej. */
    
    while (nula == true) {
      if (retezec[pocet_nul] == '0') { /* Cyklus pocita pocet nul na zacatku retezce. */
        pocet_nul++;
      }
      else {
        nula = false;
      }
    }

    int pocet_opakovani;
    int pocetnul_kopie = pocet_nul; /* Zalohuje pocet nul na zacatku retezce, kvuli prepisovani v cyklu. */
    for (pocet_opakovani = 0; pocet_opakovani < (i-pocet_nul); pocet_opakovani++) { /* Odstrani z cisla nuly na zacatku. */
      retezec[pocet_opakovani] = retezec[pocetnul_kopie];
      pocetnul_kopie++;
    }
    
    retezec[i-pocet_nul] = '\0'; /* Orizne retezec. */
    i = i - pocet_nul; /* Do i ulozi novou delku retezce. */
  }
  
  if (i <= 10) { /* Pokud je rozsah mensi jak 10 cislic proved. */
    long hodnota_cisla = 0;
    sscanf(retezec, "%li", &hodnota_cisla); /* Prevod string na long. */
    if (hodnota_cisla <= INT_MAX) { /* Pokud je cislo mensi jak INT_MAX otestuj prvocislo. */
      
      for (int cyklus_1 = 2; cyklus_1 < hodnota_cisla; cyklus_1++) {
        if ((hodnota_cisla % cyklus_1) == 0) { /* Pokud je modus 0, nejedna se o prvocislo. */
          prvocislo = false;
            break;
        }
      }
      if (hodnota_cisla == 0 || hodnota_cisla == 1) { /* Osetreni O a 1, které nejsou prvocisla. */
         printf("number: %li\n",hodnota_cisla);
      }
      else {
        if (prvocislo == true) { /* Pokud je prvocislo, vytiskne se jako prvocislo. */
          printf("number: %li (prime)\n",hodnota_cisla);
          }
        else { /* Pokud neni prvocislo, vytiskne se jako cislo. */
          printf("number: %li\n",hodnota_cisla);
        }
      }
    }
  else { /* Pokud ma 10 cifer, ale je vetsi jak int_max vypis cislo. */
    printf("number: %s\n",retezec);
    }
  }
  else { /* Pokud je cislo vestli jak 10 cifer vypis cislo. */
    printf("number: %s\n",retezec);
  }
}

void JePalindrom(int i, char retezec[]) {
  
  //ZDE ZACINA DETEKCE PALINDROMU
  int j = 0;
  int delka = i;
  char pozpatku[101];
  bool palindrom = true;

  
  while (j != i) {
    pozpatku[j] = retezec[delka-1]; /* Do promenne pozpatku prevrati znaky z promenne retezec. */
    delka--;
    j++;
  }
  
  int k;
  for (k = 0; k != i; k++) { /* Porovnava retezec a pozpatka a urcuje, zda jsou schodne. */
    if (pozpatku[k] != retezec[k]) {
      palindrom = false; /* Urcuje, zda se jedna o palindrom */
    }
  }
  
  if (palindrom == true) {
    printf("word: %s (palindrome)\n",retezec); /* Vypisuje, ze se jedna o palindrom. */
  }
  else {
    printf("word: %s\n",retezec); /* Vypisuje, ze se nejedna o palindrom. */
  }
  //ZDE KONCI DETEKCE PALINDROMU
}

void JeDatum(int i, char retezec[]) {
  int den_i, mesic_i, rok_i;
  char vysledek[20];
  bool datum_rozsah = false;

  sscanf(retezec, "%d%d%d", &rok_i, &mesic_i, &den_i); /* Z retezce vytahne rok, datum, den do int promennych. Mesic, den budou zaporne, kvuli pomlcce. */
  mesic_i *= -1;
  den_i *= -1;
  
  time_str.tm_year = rok_i - 1900;
  time_str.tm_mon = mesic_i - 1;
  time_str.tm_mday = den_i;
  
  if ((mesic_i >=1 && mesic_i <= 12) && (den_i >= 1 && den_i <= 31)) {
     datum_rozsah = true;                       /*Podminka hlida rozsah dnu a mesicu, kvuli strukture mktime) */
  }
  
  if((mktime(&time_str) == -1) || (datum_rozsah == false)) { /* Pokud se prevod nepovede nebo mesice a dny nesedi, je s datem zachazeno jako cislo a spusti test na palindrom. */
    JePalindrom(i, retezec);
  }
  else
  {
    strftime(vysledek, sizeof(vysledek), "%a %F", &time_str );
        /* Ze struktury time_str zpracuje datum a vysledek ulozi do vysledku. */
    printf("date: %s\n",vysledek); /* Vytiskne datum v pozadovanem formate. */
  }
}
