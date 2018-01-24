//
//  main.c
//  FIT_Projekt2
//
//  Created by Petr Juda on 01.11.15.
//  Created in Xcode.
//  Copyright © 2015 Petr Juda. All rights reserved.
//


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>

//DEKLARACE FUNKCI

// FUNKCE FA POCITA HODNOTY CITATELE CFRAC_LOG.
double fa(double z, unsigned int n) {
  double citatel;
  z = (z - 1) / (z + 1); // UPRAVENI VSTUPNI HODNOTY Z(X) PODLE VZORCE.
  citatel = z * z * n * n; // VYPOCET HODNOTY CITATELE.
  return citatel;
}

// FUNKCE FB POCITA HODNOTY JMENOVATELE CFRAC_LOG. (LICHA CISLA)
int fb(unsigned int n) {
  int licha = 1;
  for (unsigned int i = 1; i <= n; i++) { // VYPOCET LICHYCH CISEL PODLE CISLA ITERACE.
    licha = licha + 2;
  }
  return licha;
}

// FUNKCE ZRETEZENYCH ZLOMKU.
double cfrac_log(double x, unsigned int n) {
  //VNORENA PODMINKA HLIDA DEF. OBOR A KONSTANTY INF A NAN
  if (x < 0 ) {
    return NAN;
  }
  else {
    if (x == 0) {
      return -INFINITY;
    }
    else {
      if (isinf(x)) {
        return INFINITY;
      }
      else {
        if (isnan(x)) {
          return NAN;
        }
      }
    }
  }
  //PODMINKA HLIDA VSTUP NULOVEHO POCTU ITERACI
  if (n == 0) {
    printf("Zadan nulovy pocet iteraci.\n");
    return 1;
  }
  
  n--; // DEKREMENTACE POCTU ITERACI, KVULI POSLEDNI ITERACI PROVADENE MIMO CYKLUS.
  double cf = 0.0, vysledek;
  while (n >= 1) { // DOKUD JE N VETSI JAK 1, POCITEJ ZRETEZENY ZLOMEK.
    cf = fa(x, n) / ( fb(n) - cf ); // VOLA FUNKCE FA A FB A POCITA ZRETEZENY ZLOMEK.
    n--;
  }
  vysledek = (2 * ((x - 1)/(x + 1))) / (fb(0) - cf); // DO VYSLEDKU ULOZI HODNOTU LOG. PROVADI SE ZDE I POSLEDNI ITERACE.
  return vysledek;
}

// ABSOLUTNI HODNOTA.
double ABS(double x) {
  if (x < 0) {
    x *= -1;
  }
  return x;
}

// FUNKCE PRO VYPOCET TAYLOR_LOG.
double taylor_log(double x, unsigned int n) {
  //VNORENA PODMINKA HLIDA DEF. OBOR A KONSTANTY INF A NAN
  if (x < 0 ) {
   return NAN;
  }
  else {
    if (x == 0) {
      return -INFINITY;
    }
    else {
      if (isinf(x)) {
        return INFINITY;
      }
      else {
        if (isnan(x)) {
          return NAN;
        }
      }
    }
  }
  //PODMINKA HLIDA VSTUP NULOVEHO POCTU ITERACI
  if (n == 0) {
    printf("Zadan nulovy pocet iteraci.\n");
    return 1;
  }
  
  double vysledek = 0;
  if (x <= 1.0) { // POKUD JE ZADANA HODNOTA MENSI NEZ 1.0, POCITA PRVNIM VZORCEM.
    unsigned int i = 1;
    double mocnina = 1;
    while (i <= n) { // OD PRVNIHO CLENU PO N TY CLEN PROVADI VYPOCET.
      mocnina *= (1 - x);
      
    vysledek = (vysledek - (mocnina / i)); // DO VYSLEDKU ULOZI VYSLEDEK - AKTUALNI CLEN.
    i++;
    }
  }
  else if (x > 1.0) { // POKUD JE ZADANA HODNOTA VETSI NEZ 1.0, POCITEJ DRUHYM VZORCEM.
    unsigned int i = 1;
    double mocnina = 1;
    while (i <= n) {// OD PRVNIHO CLENU PO N TY CLEN PROVADI VYPOCET.
      
      mocnina = mocnina * ((x - 1) / x);
      vysledek += (mocnina / i);  // DO VYSLEDKU ULOZI VYSLEDEK + AKTUALNI CLEN.
      i++;
    }
  }
  return vysledek;
}

// FUNKCE HLEDA HORNI HRANICI INTERVALU PRO VYPOCET POCTU ITERACI.
unsigned int test_iter(double min, double eps, double (*funkce)(double, unsigned int)){
  unsigned int iterace_pocet = 1;
  while ((ABS(log(min)) - ABS((funkce(min, iterace_pocet)))) >= (eps)) {
    //PODMINKA HLIDA NEJVYSSI INTERVAL
    if (iterace_pocet == 100000000) {
      iterace_pocet = UINT_MAX;
    }
    //NASTRELUJE NASOBKY 10 JAKO VRCHNI INTERVAL
    iterace_pocet *= 10;
  }
  return iterace_pocet;
}

// FUNKCE PROVADI BISEKCI
unsigned int bisekce(double x, double eps, unsigned int max, double (*funkce)(double, unsigned int) ){
  unsigned int median = max/2, min = max / 10;
  if (max == UINT_MAX) {
    min = 100000000;
  }
  //POKUD JE HORNI MEZ 1, STACI 1 ITERACE
  if (max == 1) {
    return max;
  }
  //PROVADI PULENI INTERVALU NEZ TO JSOU DVE PO SOBE JDOUCI HODNOTY, POTOM UKONCI CYKLUS.
  while (max - min != 1) {
      if ((ABS(log(x)) - ABS(funkce(x, median))) >= eps) {
      min = median;
      median = (min + max)/2;
    }
    else {
      max = median;
      median = min + (max - min)/2;
    }
  }
  //Z DVOU PO SOBE JDOUCICH HODNOT VYBERE VHODNY POCET ITERACI
  if ((ABS(log(x)) - ABS(funkce(x, min))) <= eps) {
    median = min;
  }
  else {
    median = max;
  }
  return median;
}

//FUNKCE MAIN

int main(int argc, char * argv[]) {
  char *endptr = NULL;
  if (argc > 1) { // POKUD JE POCET ARGUMENTU VETSI NEZ JEDNA.
    
    double x; // PROMENA X, PRO VSTUP CISLA LOGARITMU.
    unsigned int n; // PROMENA N, PRO VSTUP POCTU ITERACI.
    
  //SPOUSTENI --LOG
    if ((strcmp(argv[1], "--log") == 0) && (argc == 4)) { // KDYZ JE PRVNI ARGUMENT --LOG A POCET ARGUMENTU JE 4.
      
  //PREVOD 2.ARGUMENTU
      x = strtod (argv[2], &endptr); // PREVOD 2.ARGUMENTU NA DOUBLE.
        if (*endptr != '\0') {
          printf("Chyba prevodu argumentu.\n");
          return 2;
        }
        
  //PREVOD 3.ARGUMENTU
        long test;
        if (((test = (strtol (argv[3], &endptr, 10)
                      )) != 0) && (test != -1)) { // BASE ZNAMENA, V JAKE SOUSTAVE OCEKAVAM VSTUP.
          if (*endptr != '\0') {
            printf("Chyba prevodu argumentu.\n");
            return 1;
          }
          if (test < 1) {
            printf("Zadan zaporny pocet iteraci!\n");
            return 1;
          }
          if (test > UINT_MAX) {
            printf("Zadany pocet iteraci je vetsi jak rozsah UINTMAX!");
            return 2;
          }
          else {
            n = (unsigned int)test;
          }
          printf("log(%g) = %.12g\n", x, log(x));
          printf("cf_log(%g) = %.12g\n", x, cfrac_log(x, n));
          printf("taylor_log(%g) = %.12g\n", x, taylor_log(x, n));
        }
    else {
        printf("Chyba vstupu! (treti argument)\n"); // KDYŽ JE SE NEPOVEDE PREVOD DRUHEHO ARGUMENTU NA DOUBLE.
        return 1;
    }
  }
  else {
  //SPOUSTENI --ITER
      if ((strcmp(argv[1], "--iter") == 0) && (argc == 5)) { // KDYZ JE PRVNI ARGUMENT --ITER A POCET ARGUMENTU JE 4.
        
        double min, max, eps;
        
        //PREVOD DRUHEHO ARGUMENTU
        min = strtod (argv[2], &endptr);
        if (*endptr != '\0') {
          printf("Chyba prevodu argumentu.\n");
          return 2;
        }
          //PREVOD TRETIHO ARGUMENTU
        max = strtod (argv[3], &endptr);
        if (*endptr != '\0') {
          printf("Chyba prevodu argumentu.\n");
          return 2;
        }
          if (min > max) {
          printf("Chyba vstupu! (MIN je vetsi jak MAX.)\n"); // KDYŽ JE MIN > MAX.
          return 1;
        }
        //KONTROLA CTVRTEHO ARGUMENTU
        if ((eps = strtod (argv[4], &endptr)) != 0.0) {
          if (*endptr != '\0') {
            printf("Chyba prevodu argumentu.\n");
            return 2;
          }
          if (eps < 1e-12) {
            printf("Zadane eps je prilis male, zadej eps vetsi nez 1e-12.\n");
            return 3;
          }
          if ((isnan(eps) != 0) || (isinf(eps) == 1) || (isinf(eps) == -1)) {
            printf("Neplatny vstup!\n"); // NEPLATNA HODNOTA NA VSTUPU (INF NEBO NAN).
            return 1;
          }
          
          if (min < 0 || min == 0 || isnan(min) || isinf(min) || max < 0 || max == 0 || isnan(max) || isinf(max)) {
            printf("MIN nebo MAX je mimo def obor, nelze hledat iterace.\n");
            return 2;
          }
          
          //ZAVOLA FUNKCE PRO --ITER
          unsigned int iteraceT_pocet; // PROMENA PRO POCET ITERACI TAYLOR.
          unsigned int iteraceC_pocet; // PROMENA PRO POCET ITERACI ZRETEZENE ZLOMKY.
  
          //OFICIALNI TISK
          printf("log(%g) = %.12g\n", min, log(min));
          printf("log(%g) = %.12g\n", max, log(max));
          
          //NASTREL POCTU ITERACI PRO CF LOG
          unsigned int nastrel_min = test_iter(max, eps, cfrac_log);
          unsigned int nastrel_max = test_iter(min, eps, cfrac_log);
          
          if (nastrel_max > nastrel_min) {
            iteraceC_pocet = nastrel_max;
          }
          else {
            iteraceC_pocet = nastrel_min;
          }
          
          //VYBER VETSIHO POCTU ITERACI PRO CF LOG
          unsigned int iteraceC_bisekce;
          unsigned int bisekce_min = bisekce(min, eps, iteraceC_pocet, cfrac_log);
          unsigned int bisekce_max = bisekce(max, eps, iteraceC_pocet, cfrac_log);
          
          if (bisekce_max > bisekce_min) {
            iteraceC_bisekce = bisekce_max;
          }
          else {
            iteraceC_bisekce = bisekce_min;
          }
          
          printf("continued fraction iterations = %u\n", iteraceC_bisekce);
          printf("cf_log(%g) = %.12g\n", min, cfrac_log(min, iteraceC_bisekce));
          printf("cf_log(%g) = %.12g\n", max, cfrac_log(max, iteraceC_bisekce));
              
          //NASTREL POCTU ITERACI PRO TAYLOR LOG
          nastrel_min = test_iter(max, eps, taylor_log);
          nastrel_max = test_iter(min, eps, taylor_log);
          
          if (nastrel_max > nastrel_min) {
            iteraceT_pocet = nastrel_max;
          }
          else {
            iteraceT_pocet = nastrel_min;
          }
          //VYBER VETSIHO POCTU ITERACI PRO TAYLOR LOG
          unsigned int iteraceT_bisekce;
          bisekce_min = bisekce(min, eps, iteraceT_pocet, taylor_log);
          bisekce_max = bisekce(max, eps, iteraceT_pocet, taylor_log);
          
          if (bisekce_max > bisekce_min) {
            iteraceT_bisekce = bisekce_max;
          }
          else {
            iteraceT_bisekce = bisekce_min;
          }
          
          printf("taylor polynomial iterations = %u\n", iteraceT_bisekce);
          printf("taylor_log(%g) = %.12g\n", min, taylor_log(min, iteraceT_bisekce));
          printf("taylor_log(%g) = %.12g\n", max, taylor_log(max, iteraceT_bisekce));
        }
        else {
          printf("Chyba vstupu! (ctvrty argument)\n"); // KDYZ JE SE NEPOVEDE PREVOD CTVRTEHO ARGUMENTU NA DOUBLE.
          return 1;
        }
      }
      else { // KDYZ JE POCET ARGUMENTU MENSI, VETSI NEZ 4, A NENI --ITER.
        printf("Chyba vstupu!\nProgram pocita hodnoty prirozeneho logaritmu.\nSpousti se s argumenty:\n--log x n\n--iter min max eps.\nMIM musi byt mensi nez MAX, nejmensi povolena EPS je 1e-12.\nAutor: Petr Juda(xjudap00)\n");
        return 1;
      }
    }
  }
  else { // POKUD JE PROGRAM SPUSTEN BEZ ARGUMENTU.
    printf("Program je spusten bez argumentu!\n");
    return 1;
  }
  return 0;
}





