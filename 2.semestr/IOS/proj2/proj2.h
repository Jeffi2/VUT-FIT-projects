/*
 * 
 * @file     proj2.h
 * project   IOS_proj2
 * @autor    Petr Juda
 * @brief    2. Projekt do IOS, program resi synchronizaci roller coaster.
 * @date     10.04.2016
 * Copyright © 2016 Petr Juda. All rights reserved.
 * 
 */

 /*
  * Vyctovy typ pro semafory
  */
enum semafory
{
  UKONCENI, // Vzajemne ukonceni
  BOARD_QUEUE, // Nastupovani (board)
  ALL_BOARD, // Auto je plne
  BOARD_CHECK, // Povoleni nastoupeni dalsimu
  UNBOARD_QUEUE, // Vystupovani (unboard)
  ALL_ASHORE, // Auto je prazdne
  BOARD_SEM, // Pusti nastoupit dalsiho pasazera
  UNBOARD_SEM, // Pusti vystoupit dalsiho pasazera
  VYPIS, // Osetreni pristupu k souboru
  SPAWN, // Osetreni spawnu a killu auta
  SPAWN2, // Osetreni spawnu a killu spawnera
  POCET_SEM // Pocet semaforu
};

/*
 * Vyctovy typ pro sdilenou pamet
 */
enum pamet
{
  AKCE, // Cislo akce
  NASTOUPILO, // Pocet nastoupenych pasazeru
  BOARD_ORDER, // Board order
  UNBOARD_ORDER, // Unboard order
  VYSTOUPILO, // Pocet pasazeru co vystoupilo
  PASS_ID, // ID pasazera
  FORK, // Pocet forknutych passazeru
  PID_CAR, //Pid procesu car
  CHYBA_KOD, //Navrat kod
  PID_SPAWN, //Navrat kod
  POCET_PAMET // Pocet oddelenych pameti.
};

/*
 * Struktura pro ulozeni argumentu
 */
typedef struct arg {
  int P;
  int C;
  int PT;
  int RT;
} argumenty;

/**
 *
 *	@brief Funkce pro zakladni test argumentu.
 * 	@param arg Text pro extrakci cisla.
 * 	@retun Cislo z arg nebo chybu a ukonceni.
 *
 */
int test_argument_number (const char * arg);

/**
 *
 *  @brief Funkce pro vytvoreni/inicializaci semaforu a sdilene pameti
 *  @retun Nic, pri chybe nastaci navrat. hodnotu.
 *
 */
void inicializace(int * navrat);

/**
 *
 *  @brief Funkce pro odinicializaci/odstraneni semaforu a sdilene pameti
 *  @retun Nic, pri chybe nastaci navrat. hodnotu.
 *
 */
void cisteni(int * navrat);

/**
 *
 *  @brief Funkce pro rizeni procesu auta.
 *  @param arg Zpracovane argumenty.
 *  @retun Nic.
 *
 */
void car(argumenty arg);

/**
 *
 *  @brief Funkce pro rizeni procesu pasazeru.
 *  @param arg Zpracovane argumenty.
 *  @retun Nic.
 *
 */
void passsanger(argumenty arg);

/**
 *
 *  @brief Funkce pro rizeni procesu vytvareni pasazeru.
 *  @param arg Zpracovane argumenty.
 *  @retun 0 pri uspechu, 2 pri chybe.
 *
 */
int spawner(argumenty arg);

/**
 *
 *	@brief Hlavni funkce.
 *	@para argc Pocet argumentu.
 *	@param argv Pole argumentu.
 * 	@return	Nula pri uspechu, nenulove cislo pri chybe.
 */
int main(int argc, const char * argv[]);
