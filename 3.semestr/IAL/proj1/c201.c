
/* c201.c *********************************************************************}
{* Téma: Jednosměrný lineární seznam
**
**                     Návrh a referenční implementace: Petr Přikryl, říjen 1994
**                                          Úpravy: Andrea Němcová listopad 1996
**                                                   Petr Přikryl, listopad 1997
**                                Přepracované zadání: Petr Přikryl, březen 1998
**                                  Přepis do jazyka C: Martin Tuček, říjen 2004
**                                            Úpravy: Bohuslav Křena, říjen 2016
**
** Implementujte abstraktní datový typ jednosměrný lineární seznam.
** Užitečným obsahem prvku seznamu je celé číslo typu int.
** Seznam bude jako datová abstrakce reprezentován proměnnou typu tList.
** Definici konstant a typů naleznete v hlavičkovém souboru c201.h.
** 
** Vaším úkolem je implementovat následující operace, které spolu s výše
** uvedenou datovou částí abstrakce tvoří abstraktní datový typ tList:
**
**      InitList ...... inicializace seznamu před prvním použitím,
**      DisposeList ... zrušení všech prvků seznamu,
**      InsertFirst ... vložení prvku na začátek seznamu,
**      First ......... nastavení aktivity na první prvek,
**      CopyFirst ..... vrací hodnotu prvního prvku,
**      DeleteFirst ... zruší první prvek seznamu,
**      PostDelete .... ruší prvek za aktivním prvkem,
**      PostInsert .... vloží nový prvek za aktivní prvek seznamu,
**      Copy .......... vrací hodnotu aktivního prvku,
**      Actualize ..... přepíše obsah aktivního prvku novou hodnotou,
**      Succ .......... posune aktivitu na další prvek seznamu,
**      Active ........ zjišťuje aktivitu seznamu.
**
** Při implementaci funkcí nevolejte žádnou z funkcí implementovaných v rámci
** tohoto příkladu, není-li u dané funkce explicitně uvedeno něco jiného.
**
** Nemusíte ošetřovat situaci, kdy místo legálního ukazatele na seznam 
** předá někdo jako parametr hodnotu NULL.
**
** Svou implementaci vhodně komentujte!
**
** Terminologická poznámka: Jazyk C nepoužívá pojem procedura.
** Proto zde používáme pojem funkce i pro operace, které by byly
** v algoritmickém jazyce Pascalovského typu implemenovány jako
** procedury (v jazyce C procedurám odpovídají funkce vracející typ void).
**/

#include "c201.h"

int solved;
int errflg;

void Error() {
/*
** Vytiskne upozornění na to, že došlo k chybě.
** Tato funkce bude volána z některých dále implementovaných operací.
**/	
    printf ("*ERROR* The program has performed an illegal operation.\n");
    errflg = TRUE;                      /* globální proměnná -- příznak chyby */
}

void InitList (tList *L) {
/*
** Provede inicializaci seznamu L před jeho prvním použitím (tzn. žádná
** z následujících funkcí nebude volána nad neinicializovaným seznamem).
** Tato inicializace se nikdy nebude provádět nad již inicializovaným
** seznamem, a proto tuto možnost neošetřujte. Vždy předpokládejte,
** že neinicializované proměnné mají nedefinovanou hodnotu.
**/
    L->Act = NULL;
    L->First = NULL;

}

void DisposeList (tList *L) {
/*
** Zruší všechny prvky seznamu L a uvede seznam L do stavu, v jakém se nacházel
** po inicializaci. Veškerá paměť používaná prvky seznamu L bude korektně
** uvolněna voláním operace free.
***/
    while (L->First != NULL) { //Dokud neni seznam pouze inicializovany
        tElemPtr tmp_tElem; //Vytvorime pomocny prvek
        tmp_tElem = L->First; //Prvni budeme rusit
        L->First = L->First->ptr; //Posuneme ukazatel na dalsi
        free(tmp_tElem); //Zrusime prvek
    }
    L->Act = NULL; //Prazdy seznam bude neaktivni
}

void InsertFirst (tList *L, int val) {
/*
** Vloží prvek s hodnotou val na začátek seznamu L.
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** volá funkci Error().
**/
    tElemPtr tmp_tElem; //Vytvorime pomocny prvek
    tmp_tElem = malloc(sizeof(struct tElem)); //Alokujeme misto, pokud neni vyvolame chybu
    if (tmp_tElem == NULL) {
        Error();
    }
    else {
    tmp_tElem->data = val;
    tmp_tElem->ptr = L->First;
    L->First = tmp_tElem;
    }
}

void First (tList *L) {
/*
** Nastaví aktivitu seznamu L na jeho první prvek.
** Funkci implementujte jako jediný příkaz, aniž byste testovali,
** zda je seznam L prázdný.
**/
    L->Act = L->First; //Aktivuje prvni prvek
}

void CopyFirst (tList *L, int *val) {
/*
** Prostřednictvím parametru val vrátí hodnotu prvního prvku seznamu L.
** Pokud je seznam L prázdný, volá funkci Error().
**/
    if (L->First != NULL) { //Pokud neni prazdny
        *val = L->First->data; // Pomoci predani hodnotou, vrati data prvniho prvku.
    }
    else {
        Error();
    }
}

void DeleteFirst (tList *L) {
/*
** Zruší první prvek seznamu L a uvolní jím používanou paměť.
** Pokud byl rušený prvek aktivní, aktivita seznamu se ztrácí.
** Pokud byl seznam L prázdný, nic se neděje.
**/
    if (L->First != NULL) { //Pokud neni prazdny
        if (L->First == L->Act) { //Pokud prvni aktivni, ztrati se aktivita
            L->Act = NULL;
        }
            tElemPtr tmp_tElem; //Vytvorime pomocny prvek
            tmp_tElem = L->First; //Ukazuje na ruseny prvek
            L->First = L->First->ptr; //Druhy bude prvni
            free(tmp_tElem);
    }
}	

void PostDelete (tList *L) {
/* 
** Zruší prvek seznamu L za aktivním prvkem a uvolní jím používanou paměť.
** Pokud není seznam L aktivní nebo pokud je aktivní poslední prvek seznamu L,
** nic se neděje.
**/
    if ((L->Act != NULL) && (L->Act->ptr != NULL)) { //Pokud je aktivni
        tElemPtr tmp_tElem; //Vytvorime pomocny prvek
        tmp_tElem = L->Act->ptr; //Ukazuje na ruseny prvek
        L->Act->ptr = L->Act->ptr->ptr; //Aktivni bude ukazovat na nasledujici
        free(tmp_tElem); //Uvolni prvek
    }

}

void PostInsert (tList *L, int val) {
/*
** Vloží prvek s hodnotou val za aktivní prvek seznamu L.
** Pokud nebyl seznam L aktivní, nic se neděje!
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** zavolá funkci Error().
**/
    if (L->Act != NULL) { //Pokud je aktivni
        tElemPtr tmp_tElem; //Vytvorime pomocny prvek
        tmp_tElem = malloc(sizeof(struct tElem)); //Alokujeme misto, pokud neni vyvolame chybu
        if (tmp_tElem == NULL) {
            Error();
        }
        else {
            tmp_tElem->data = val;
            tmp_tElem->ptr = L->Act->ptr; //Ukazuje tam, kde je aktivni
            L->Act->ptr = tmp_tElem; //Aktivni bude ukazovat na nove pridany prvek
        }

    }
}

void Copy (tList *L, int *val) {
/*
** Prostřednictvím parametru val vrátí hodnotu aktivního prvku seznamu L.
** Pokud seznam není aktivní, zavolá funkci Error().
 **/
    if (L->Act != NULL) { //Pokud je aktivni
        *val = L->Act->data; // Pomoci predani hodnotou, vrati data aktivniho prvku.
    }
    else {
        Error();
    }
}

void Actualize (tList *L, int val) {
/*
** Přepíše data aktivního prvku seznamu L hodnotou val.
** Pokud seznam L není aktivní, nedělá nic!
**/
    if (L->Act != NULL) { //Pokud je aktivni
        L->Act->data = val;
    }
}

void Succ (tList *L) {
/*
** Posune aktivitu na následující prvek seznamu L.
** Všimněte si, že touto operací se může aktivní seznam stát neaktivním.
** Pokud není předaný seznam L aktivní, nedělá funkce nic.
**/
    if (L->Act != NULL) { //Pokud je aktivni
        if (L->Act->ptr == NULL) { //Posledni prvek, deaktivuje seznam
            L->Act = NULL;
        }
        else {
            L->Act = L->Act->ptr; //Posune ukazatel na nasledovnika
        }
    }
}

int Active (tList *L) {		
/*
** Je-li seznam L aktivní, vrací nenulovou hodnotu, jinak vrací 0.
** Tuto funkci je vhodné implementovat jedním příkazem return. 
**/
	return (L->Act != NULL); // Pokud neni prazdy, vrati true(1), jinak false(0)
}

/* Konec c201.c */
