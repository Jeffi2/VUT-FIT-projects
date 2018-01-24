/*
 * 
 * @file     proj2.c
 * project   IOS_proj2
 * @autor    Petr Juda
 * @brief    2. Projekt do IOS, program resi synchronizaci roller coaster.
 * @date     10.04.2016
 * Copyright © 2016 Petr Juda. All rights reserved.
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include "proj2.h"

char *shmKEYs[POCET_PAMET] = {"/xjudap00_cislo_akce", "/xjudap00_pocet_nastoupenych", "/xjudap00_board_order", "/xjudap00_unboard_order", "/xjudap00_pocet_vystoupenych","/xjudap00_id_pass","/xjudap00_pocet_forku", "/xjuddap00_pid_car", "/xjudap00_navrat_kod","/xjudap00_pid_spawn"}; // Pojmenovani sdilene pameti
sem_t *semafory[POCET_SEM]; // Pole semaforu
int *shm[POCET_PAMET]; // Pole sdilene pameti
int shm_id[POCET_PAMET]; // ID pro otevreni sdilene pameti
FILE *vypis = NULL; // Soubor do ktereho se bude ukladat vypis programu
pid_t pid_car; // PID procesu car
pid_t pid_spaw; // PID procesu spawner

//--------------------------------------------------------------------------------------------------------//
/**
 *
 *	@brief Funkce pro zakladni test argumentu.
 * 	@param arg Text pro extrakci cisla.
 * 	@retun Cislo z arg nebo chybu a ukonceni.
 *
 */
int test_argument_number (const char * arg) {
    long test;
    int vysledek;
    char *endptr = NULL;
    test = (strtol (arg, &endptr, 10));  // 10 - base
      if (*endptr != '\0') { //Test na pritomnost stringu.
        fprintf(stderr,"Chyba prevodu argumentu.\n");
        fflush(stderr);
        exit(1);
      }
      else {
        vysledek = (unsigned int)test;
      }
  return vysledek;
}

//--------------------------------------------------------------------------------------------------------//
/**
 *
 *  @brief Funkce pro vytvoreni/inicializaci semaforu a sdilene pameti
 *  @retun Nic, pri chybe nastaci navrat. hodnotu.
 *
 */
void inicializace(int * navrat) {
  for (int i = 0; i < POCET_SEM; i++) {
    int hodnota = 0;
    if ((i == BOARD_CHECK) || (i == UNBOARD_SEM)) {
      hodnota = 1;
    }
    if ((semafory[i] = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0)) == MAP_FAILED) {
      *navrat = 1;
    }
    if ((sem_init(semafory[i], 1, hodnota)) == -1) {
      *navrat = 1;
    }
  }

  for(int i = 0; i < POCET_PAMET; i++) {
    if ((shm_id[i]=shm_open(shmKEYs[i], O_RDWR|O_CREAT, 0600)) == -1) {
      *navrat = 2;
    }
    if ((ftruncate(shm_id[i],sizeof(int))) == -1) {
      *navrat = 2;
    }
    if ((shm[i]= mmap(NULL,sizeof(int),PROT_READ|PROT_WRITE,MAP_SHARED,shm_id[i],0)) == MAP_FAILED) {
      *navrat = 2;
    }
    *shm[i]=0; 
  }   
}

//--------------------------------------------------------------------------------------------------------//
/**
 *
 *  @brief Funkce pro odinicializaci/odstraneni semaforu a sdilene pameti
 *  @retun Nic, pri chybe nastaci navrat. hodnotu.
 *
 */
void cisteni(int * navrat){
  for (int i = 0; i < POCET_SEM; i++) {
    if (sem_destroy(semafory[i]) == -1) {
      *navrat = 1;
    }
    if ((munmap(semafory[i], sizeof(sem_t))) == -1) {
      *navrat = 1;
    }
  }

  for(int i = 0; i < POCET_PAMET; i++)
  {
    if ((munmap(shm[i], sizeof(int)) == -1)) {
      *navrat = 2;
    }
    if ((shm_unlink(shmKEYs[i])) == -1) {
      *navrat = 2;
    }
    if ((close(shm_id[i])) == -1) {
      *navrat = 2;
    }
  }
}

//--------------------------------------------------------------------------------------------------------//  
/**
 *
 *  @brief Funkce pro rizeni procesu auta.
 *  @param arg Zpracovane argumenty.
 *  @retun Nic.
 *
 */
void car(argumenty arg) {
    sem_post(semafory[SPAWN]);
    int id_auto = 1;
    pid_car = getpid();
    *shm[PID_CAR] = pid_car;
    // vytvorene auto
    fprintf(vypis,"%d\t: %s %d\t: started\n",++(*shm[AKCE]),"C",id_auto);
    sem_post(semafory[VYPIS]);

    // v cyklu odvazime pasazery
    for (int i = 1; i <= (arg.P/arg.C); i++) {
     sem_wait(semafory[VYPIS]);
     fprintf(vypis,"%d\t: %s %d\t: load\n",++(*shm[AKCE]),"C",id_auto);
     sem_post(semafory[VYPIS]);

     // povoli nastoupeni C pasazeru
     for (int i = 0; i < arg.C; i++) {
       sem_post(semafory[BOARD_QUEUE]); 
     }

      //cekame nez vsichni nastoupi, po te jedeme
     sem_wait(semafory[ALL_BOARD]);
     sem_wait(semafory[VYPIS]);
     fprintf(vypis,"%d\t: %s %d\t: run\n",++(*shm[AKCE]),"C",id_auto);
     sem_post(semafory[VYPIS]);

     //uspani auta
     if (arg.RT != 0) {
        usleep((rand() % (arg.RT + 1)) * 1000);
      }

     //zahajeni vystupovani
     sem_wait(semafory[VYPIS]);
     fprintf(vypis,"%d\t: %s %d\t: unload\n",++(*shm[AKCE]),"C",id_auto);
     sem_post(semafory[VYPIS]);

     //povoli vystoupeni C pasazeru
     for (int i = 0; i < arg.C; i++) {
       sem_post(semafory[UNBOARD_QUEUE]); 
     }
    //cekame nez vsichni vystoupi, posilame sichnal k dalsimu vystoupeni/nastoupeni
    sem_wait(semafory[ALL_ASHORE]);
    sem_post(semafory[UNBOARD_SEM]);
    sem_post(semafory[BOARD_CHECK]); 
    }
  }

//--------------------------------------------------------------------------------------------------------//  
/**
 *
 *  @brief Funkce pro rizeni procesu pasazeru.
 *  @param arg Zpracovane argumenty.
 *  @retun Nic.
 *
 */
void passsanger(argumenty arg) {
  //ziskani id pasazera (abychom zacali od 1)
  ++(*shm[PASS_ID]);
  int id_pass = *shm[PASS_ID];
  sem_wait(semafory[VYPIS]);
  fprintf(vypis,"%d\t: %s %d\t: started\n",++(*shm[AKCE]),"P",id_pass);
  sem_post(semafory[VYPIS]);
        
  //Cekame na signal load
  sem_wait(semafory[BOARD_QUEUE]);
  sem_wait(semafory[BOARD_CHECK]);
  sem_wait(semafory[VYPIS]);
  fprintf(vypis,"%d\t: %s %d\t: board\n",++(*shm[AKCE]),"P",id_pass);
  sem_post(semafory[VYPIS]);
        
  //kontrolujeme pocet nastoupenych pasazeru a kapacitu auta
  ++(*shm[NASTOUPILO]);
  if ((*shm[NASTOUPILO] % arg.C) == 0) {
    sem_wait(semafory[VYPIS]);
    fprintf(vypis,"%d\t: %s %d\t: board last\n",++(*shm[AKCE]),"P",id_pass);
    sem_post(semafory[VYPIS]);
    //nastoupil posledni pasazer
    sem_post(semafory[ALL_BOARD]);
    *shm[BOARD_ORDER] = 0;
    *shm[NASTOUPILO] = 0;
  }
  else {
    sem_wait(semafory[VYPIS]);
    fprintf(vypis,"%d\t: %s %d\t: board order %d\n",++(*shm[AKCE]),"P",id_pass,++(*shm[BOARD_ORDER]));
    sem_post(semafory[VYPIS]);
    // posleme dalsiho pasazera
    sem_post(semafory[BOARD_CHECK]);
  }

  //ZACATEK VYSTUPOVANI, cekama na signal unboard
  sem_wait(semafory[UNBOARD_QUEUE]);
  sem_wait(semafory[UNBOARD_SEM]);
  sem_wait(semafory[VYPIS]);
  fprintf(vypis,"%d\t: %s %d\t: unboard\n",++(*shm[AKCE]),"P",id_pass);
  sem_post(semafory[VYPIS]);
     
  //kontrolujeme pocet vystoupenych pasazeru
  ++(*shm[VYSTOUPILO]);
  if ((*shm[VYSTOUPILO] % arg.C) == 0) {
    sem_wait(semafory[VYPIS]);
    fprintf(vypis,"%d\t: %s %d\t: unboard last\n",++(*shm[AKCE]),"P",id_pass);
    sem_post(semafory[VYPIS]);
    //vsichni vystoupili
    sem_post(semafory[ALL_ASHORE]);
    *shm[UNBOARD_ORDER] = 0;
    *shm[VYSTOUPILO] = 0;
  }
  else {
    sem_wait(semafory[VYPIS]);
    fprintf(vypis,"%d\t: %s %d\t: unboard order %d\n",++(*shm[AKCE]),"P",id_pass,++(*shm[UNBOARD_ORDER]));
    sem_post(semafory[VYPIS]);
    //posleme vystupovat dalsiho pasazera
    sem_post(semafory[UNBOARD_SEM]);
  }
  // pasazeri na sebe pred ukonecenim cekaji
  sem_wait(semafory[UKONCENI]);
  sem_wait(semafory[VYPIS]);
  fprintf(vypis,"%d\t: %s %d\t: finished\n",++(*shm[AKCE]),"P",id_pass);
  sem_post(semafory[VYPIS]);
  //ukoncime dalsi
  sem_post(semafory[UKONCENI]);
}

//--------------------------------------------------------------------------------------------------------//
/**
 *
 *  @brief Funkce pro rizeni procesu vytvareni pasazeru.
 *  @param arg Zpracovane argumenty.
 *  @retun 0 pri uspechu, 2 pri chybe.
 *
 */
int spawner(argumenty arg) {
    pid_spaw = getpid();
    *shm[PID_SPAWN] = pid_spaw;
    sem_post(semafory[SPAWN2]);
    sem_wait(semafory[SPAWN]);
    // alokovani pole pid pasazeru
    int * pasazer_id = malloc(sizeof(int)*arg.P);

    //generovani pasazeru
    for (int i = 0; i <arg.P ; i++) {
      if ((pasazer_id[i] = fork()) == 0) {
        ++(*shm[FORK]);
        passsanger(arg); //ridici kod pasazera
        exit(0);
      }
      else if (pasazer_id[i] < 0) {
        fprintf(stderr, "Nepodaril se fork pasazera!\n");
        fflush(stderr);
        for (int i = 0; i < (*shm[FORK]); i++)
        {
          kill(pasazer_id[i], SIGKILL);
        }
        free(pasazer_id);
        return 2;
      } 
      if (arg.PT != 0) {
        usleep((rand() % (arg.PT + 1)) * 1000);
      }
    }
    for(int i = 0; i < arg.P; i++) {
      waitpid(pasazer_id[i],NULL,0);
    } 
    free(pasazer_id); 
    return 0;
  }

//------------------------------------------------------------------------------------------------------------------//
/**
 *
 *	@brief Hlavni funkce.
 *	@para argc Pocet argumentu.
 *	@param argv Pole argumentu.
 * 	@return	Nula pri uspechu, nenulove cislo pri chybe.
 */
int main(int argc, const char * argv[]) {
  //Kontrola poctu argumentu
  if (argc != 5) {
    fprintf(stderr, "Spatny pocet argumentu!\n");
    fflush(stderr);
    exit(1);
  }
  //zpracovani argumentu
  argumenty arg;
  arg.P = test_argument_number(argv[1]);
  arg.C = test_argument_number(argv[2]);
  arg.PT = test_argument_number(argv[3]);
  arg.RT = test_argument_number(argv[4]);
  //Doplnujici podminky pro argumenty
  if ((arg.P <= 0) || (arg.C <= 0) || (arg.P <= arg.C) || (arg.P % arg.C != 0) || (arg.PT < 0) || (arg.PT > 5000) || (arg.RT < 0) || (arg.RT > 5000)) {
    fprintf(stderr, "Neplatny argument!\n");
    fflush(stderr);
    exit(1);
  }
  //otevreni a osetreni souboru
  vypis = fopen("proj2.out","w");
  if (vypis == NULL) {
    fprintf(stderr, "Nepodarilo se vytvorit soubor!\n");
    fflush(stderr);
    exit(1);
  }
  // inicializace semaforu a sdilene pameti
  int err_check = 0;
  inicializace(&err_check);
  if (err_check == 1) {
    fprintf(stderr, "Nepodarilo se vytvorit semafory!\n");
    fflush(stderr);
     exit(2);
  }
  if (err_check == 2) {
    fprintf(stderr, "Nepodarilo se vytvorit sdilenou pamet!\n");
    fflush(stderr);
     exit(2);
  }
//------------------------------------------------------------------------------------------------------------------//
  //nastavení bufferu pro vystup ve spravnem poradi
  setvbuf(vypis,NULL,_IONBF,0);
  setbuf(vypis,NULL);
  pid_t pid;
  pid_t consPID;
  pid_t prodPID;
  int chyba = 0;
    // systemove volani - vzdy je vhodne overit uspesnost!
   if ((pid = fork()) < 0) {
      fprintf(stderr, "Nepodaril se fork spawnera!\n");
      fflush(stderr);
      fclose(vypis);
      err_check = 0;
    cisteni(&err_check);
    if (err_check == 1) {
      fprintf(stderr, "Nepodarilo se odstranit semafory!\n");
      fflush(stderr);
      exit(2);
    }
    if (err_check == 2) {
      fprintf(stderr, "Nepodarilo se odstranit sdilenou pamet!\n");
      fflush(stderr);
      exit(2);
      }
      exit(2);
    }
    if (pid == 0) { // child spawner
        chyba = spawner(arg); // pomocni proces pro generovani
        if (chyba == 0)
        {
          *shm[CHYBA_KOD] = chyba;
          exit(0);
        }
        else {
          *shm[CHYBA_KOD] = chyba;
          fclose(vypis);
          kill(*shm[PID_CAR], SIGKILL);
          exit(2); 
        }
       
    } 
    else { // parent main
      consPID = pid;
      pid = fork();
      if (pid == 0) { // child car
        car(arg); // proces car
        //po vykonani vysech aktivit auto posila signak k ukonceni pasazeru
        sem_post(semafory[UKONCENI]);
        sem_wait(semafory[VYPIS]);
        fprintf(vypis,"%d\t: %s %d\t: finished\n",++(*shm[AKCE]),"C",1);
        sem_post(semafory[VYPIS]);
        exit(0); //ukonceni auta
      } 
      else if (pid < 0)
      {
        fprintf(stderr, "Nepodaril se fork auta!\n");
        fflush(stderr);
        *shm[CHYBA_KOD] = 2;
        sem_wait(semafory[SPAWN2]);
        if (*shm[PID_SPAWN] != 0) {
          kill(*shm[PID_SPAWN], SIGKILL);
        }
      } 
      else
      { 
        prodPID = pid;
      }
    }
    // cekani na ukonceni auta a spawnera
    waitpid(consPID, NULL, 0);
    waitpid(prodPID, NULL, 0); 
    
    //uzavreni souboru
    fclose(vypis);
    chyba = *shm[CHYBA_KOD];
    // uvolneni semaforu a sdilene pameti
    err_check = 0;
    cisteni(&err_check);
    if (err_check == 1) {
      fprintf(stderr, "Nepodarilo se odstranit semafory!\n");
      fflush(stderr);
      exit(2);
    }
    if (err_check == 2) {
      fprintf(stderr, "Nepodarilo se odstranit sdilenou pamet!\n");
      fflush(stderr);
      exit(2);
  }
    return chyba;
}

