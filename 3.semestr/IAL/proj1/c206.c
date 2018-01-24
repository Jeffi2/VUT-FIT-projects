	
/* c206.c **********************************************************}
{* Téma: Dvousměrně vázaný lineární seznam
**
**                   Návrh a referenční implementace: Bohuslav Křena, říjen 2001
**                            Přepracované do jazyka C: Martin Tuček, říjen 2004
**                                            Úpravy: Bohuslav Křena, říjen 2016
**
** Implementujte abstraktní datový typ dvousměrně vázaný lineární seznam.
** Užitečným obsahem prvku seznamu je hodnota typu int.
** Seznam bude jako datová abstrakce reprezentován proměnnou
** typu tDLList (DL znamená Double-Linked a slouží pro odlišení
** jmen konstant, typů a funkcí od jmen u jednosměrně vázaného lineárního
** seznamu). Definici konstant a typů naleznete v hlavičkovém souboru c206.h.
**
** Vaším úkolem je implementovat následující operace, které spolu
** s výše uvedenou datovou částí abstrakce tvoří abstraktní datový typ
** obousměrně vázaný lineární seznam:
**
**      DLInitList ...... inicializace seznamu před prvním použitím,
**      DLDisposeList ... zrušení všech prvků seznamu,
**      DLInsertFirst ... vložení prvku na začátek seznamu,
**      DLInsertLast .... vložení prvku na konec seznamu, 
**      DLFirst ......... nastavení aktivity na první prvek,
**      DLLast .......... nastavení aktivity na poslední prvek, 
**      DLCopyFirst ..... vrací hodnotu prvního prvku,
**      DLCopyLast ...... vrací hodnotu posledního prvku, 
**      DLDeleteFirst ... zruší první prvek seznamu,
**      DLDeleteLast .... zruší poslední prvek seznamu, 
**      DLPostDelete .... ruší prvek za aktivním prvkem,
**      DLPreDelete ..... ruší prvek před aktivním prvkem, 
**      DLPostInsert .... vloží nový prvek za aktivní prvek seznamu,
**      DLPreInsert ..... vloží nový prvek před aktivní prvek seznamu,
**      DLCopy .......... vrací hodnotu aktivního prvku,
**      DLActualize ..... přepíše obsah aktivního prvku novou hodnotou,
**      DLSucc .......... posune aktivitu na další prvek seznamu,
**      DLPred .......... posune aktivitu na předchozí prvek seznamu, 
**      DLActive ........ zjišťuje aktivitu seznamu.
**
** Při implementaci jednotlivých funkcí nevolejte žádnou z funkcí
** implementovaných v rámci tohoto příkladu, není-li u funkce
** explicitně uvedeno něco jiného.
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

#include "c206.h"

int solved;
int errflg;

void DLError() {
/*
** Vytiskne upozornění na to, že došlo k chybě.
** Tato funkce bude volána z některých dále implementovaných operací.
**/	
    printf ("*ERROR* The program has performed an illegal operation.\n");
    errflg = TRUE;             /* globální proměnná -- příznak ošetření chyby */
    return;
}

void DLInitList (tDLList *L) {
/*
** Provede inicializaci seznamu L před jeho prvním použitím (tzn. žádná
** z následujících funkcí nebude volána nad neinicializovaným seznamem).
** Tato inicializace se nikdy nebude provádět nad již inicializovaným
** seznamem, a proto tuto možnost neošetřujte. Vždy předpokládejte,
** že neinicializované proměnné mají nedefinovanou hodnotu.
**/
    L->Act = NULL; //Inicializace pointeru na null
    L->First = NULL;
    L->Last =NULL;
}

void DLDisposeList (tDLList *L) {
/*
** Zruší všechny prvky seznamu L a uvede seznam do stavu, v jakém
** se nacházel po inicializaci. Rušené prvky seznamu budou korektně
** uvolněny voláním operace free. 
**/
    while (L->First != NULL) { //Dokud neni seznam pouze inicializovany
        tDLElemPtr tmp_Elem; //Vytvorime pomocny prvek
        tmp_Elem = L->First; //Prvni budeme rusit
        L->First = L->First->rptr; //Posuneme ukazatel na dalsi
        free(tmp_Elem); //Zrusime prvek
    }
    L->Act = NULL; //Prazdy seznam bude neaktivni
    L->Last = NULL; //Prazdny seznam nema posledni
}

void DLInsertFirst (tDLList *L, int val) {
/*
** Vloží nový prvek na začátek seznamu L.
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** volá funkci DLError().
**/
    tDLElemPtr tmp_Elem; //Vytvorime pomocny prvek
    tmp_Elem = malloc(sizeof(struct tDLElem)); //Alokujeme misto, pokud neni vyvolame chybu
    if (tmp_Elem == NULL) {
        DLError();
    }
    tmp_Elem->data = val; //Nahrajeme data do noveho prvku
    tmp_Elem->lptr = NULL; //Prvek nema predchudce
    tmp_Elem->rptr = L->First; //Pravy bude ukazovat na dosud prvni prvek
    if (L->Last == NULL) { //Pokud je posledni null, vkladam prvni prvek
        L->Last = tmp_Elem;
    }
    else {
        L->First->lptr = tmp_Elem; //Doposud prvni prvek bude mit levy ukazatel na nove vkladany
    }
    L->First = tmp_Elem; //Vlozeny prvek bude prvni
}

void DLInsertLast(tDLList *L, int val) {
/*
** Vloží nový prvek na konec seznamu L (symetrická operace k DLInsertFirst).
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** volá funkci DLError().
**/ 	
    
    tDLElemPtr tmp_Elem; //Vytvorime pomocny prvek
    tmp_Elem = malloc(sizeof(struct tDLElem)); //Alokujeme misto, pokud neni vyvolame chybu
    if (tmp_Elem == NULL) {
        DLError();
    }
    tmp_Elem->data = val; //Nahrajeme data do noveho prvku
    tmp_Elem->rptr = NULL; //Prvek nema nasledovnika
    tmp_Elem->lptr = L->Last; //Levy bude ukazovat na dosud posledni prvek
    if (L->Last == NULL) { //Pokud je posledni null, vkladam prvni prvek
        L->First = tmp_Elem;
    }
    else {
        L->Last->rptr = tmp_Elem; //Doposud posledni prvek bude mit pravy ukazatel na nove vkladany
    }
    L->Last = tmp_Elem; //Vlozeny prvek bude posledni

}

void DLFirst (tDLList *L) {
/*
** Nastaví aktivitu na první prvek seznamu L.
** Funkci implementujte jako jediný příkaz (nepočítáme-li return),
** aniž byste testovali, zda je seznam L prázdný.
**/
    L->Act = L->First; //Aktivuje prvni
}

void DLLast (tDLList *L) {
/*
** Nastaví aktivitu na poslední prvek seznamu L.
** Funkci implementujte jako jediný příkaz (nepočítáme-li return),
** aniž byste testovali, zda je seznam L prázdný.
**/
    L->Act = L->Last; //Aktivuje posledni
}

void DLCopyFirst (tDLList *L, int *val) {
/*
** Prostřednictvím parametru val vrátí hodnotu prvního prvku seznamu L.
** Pokud je seznam L prázdný, volá funkci DLError().
**/
    if (L->First == NULL) { //Seznam je prazdny
        DLError();
    }
    else {
        *val = L->First->data; //Neni prazdny vrati data
    }
}

void DLCopyLast (tDLList *L, int *val) {
/*
** Prostřednictvím parametru val vrátí hodnotu posledního prvku seznamu L.
** Pokud je seznam L prázdný, volá funkci DLError().
**/
    if (L->First == NULL) { //Seznam je prazdny
        DLError();
    }
    else {
        *val = L->Last->data; //Neni prazdny vrati data
    }
}

void DLDeleteFirst (tDLList *L) {
/*
** Zruší první prvek seznamu L. Pokud byl první prvek aktivní, aktivita 
** se ztrácí. Pokud byl seznam L prázdný, nic se neděje.
**/
    if (L->First != NULL) { //Pokud seznam neni prazdny
        tDLElemPtr tmp_Elem = L->First; //Prvni prvek budu uvolnovat
        if (L->Last == L->First) { //Pokud je to posledni prvek inicializuji seznam
            L->Last = NULL;
            L->Act = NULL;
            L->First = NULL;
        }
        else { //Nebyl posledni
            if (L->Act == L->First) { //Pokud je prvni aktivni, ztrati se aktivita
                L->Act = NULL;
            }
            L->First = L->First->rptr; //Druhy se stane prvnim
            L->First->lptr = NULL; //Prestane ukazovat na zruseny prvek
        }
        free(tmp_Elem);
    }
}	

void DLDeleteLast (tDLList *L) {
/*
** Zruší poslední prvek seznamu L. Pokud byl poslední prvek aktivní,
** aktivita seznamu se ztrácí. Pokud byl seznam L prázdný, nic se neděje.
**/ 
    if (L->First != NULL) { //Pokud seznam neni prazdny
        tDLElemPtr tmp_Elem = L->Last; //Posledni prvek budu uvolnovat
        if (L->Last == L->First) { //Pokud je to posledni prvek inicializuji seznam
            L->Last = NULL;
            L->Act = NULL;
            L->First = NULL;
        }
        else { //Nebyl posledni
            if (L->Act == L->Last) { //Pokud je posledni aktivni, ztrati se aktivita
                L->Act = NULL;
            }
            L->Last = L->Last->lptr; //Predposledni se stane poslednim
            L->Last->rptr = NULL; //Prestane ukazovat na zruseny prvek
        }
        free(tmp_Elem);
    }
}

void DLPostDelete (tDLList *L) {
/*
** Zruší prvek seznamu L za aktivním prvkem.
** Pokud je seznam L neaktivní nebo pokud je aktivní prvek
** posledním prvkem seznamu, nic se neděje.
**/
    if (L->Act != NULL) { //Seznam je aktivni
        if (L->Act != L->Last) { //Neni posledni prvek seznamu
            tDLElemPtr tmp_Elem = L->Act->rptr; //Prvek za aktivnim  budu uvolnovat
            if (L->Last == tmp_Elem) { //Pokud rusim posledni, aktivni se stane poslednim
                L->Last = L->Act;
                L->Last->rptr = NULL; 
            }
            else {
                tmp_Elem->lptr->rptr = tmp_Elem->rptr; //Prvek je uptrostred seznamu, upravime pravy a levy ukazatel
                tmp_Elem->rptr->lptr = tmp_Elem->lptr;
            }
        free(tmp_Elem);
        }
    }
}

void DLPreDelete (tDLList *L) {
/*
** Zruší prvek před aktivním prvkem seznamu L .
** Pokud je seznam L neaktivní nebo pokud je aktivní prvek
** prvním prvkem seznamu, nic se neděje.
**/
    if (L->Act != NULL) { //Seznam je aktivni
        if (L->First != L->Act) { //Neni posledni prvek
            tDLElemPtr tmp_Elem = L->Act->lptr; //Prvek pred aktivnim  budu uvolnovat
            if (L->First == tmp_Elem) { //Pokud rusim prvni, aktivni se stane prvnim
                L->First = L->Act;
                L->First->lptr = NULL;
            }
            else {
                tmp_Elem->lptr->rptr = tmp_Elem->rptr; //Prvek je uptrostred seznamu, upravime pravy a levy ukazatel
                tmp_Elem->rptr->lptr = tmp_Elem->lptr;
            }
            free(tmp_Elem);
        }
    }
}

void DLPostInsert (tDLList *L, int val) {
/*
** Vloží prvek za aktivní prvek seznamu L.
** Pokud nebyl seznam L aktivní, nic se neděje.
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** volá funkci DLError().
**/
    if (L->Act != NULL) { //Seznam je aktivni
        tDLElemPtr tmp_Elem; //Vytvorime pomocny prvek
        tmp_Elem = malloc(sizeof(struct tDLElem)); //Alokujeme misto, pokud neni vyvolame chybu
        if (tmp_Elem == NULL) {
            DLError();
        }
        else {
            tmp_Elem->data = val; //Nahrajeme data do noveho prvku
            tmp_Elem->lptr = L->Act; //Prvek bude vlevo ukazovat na aktivni
            tmp_Elem->rptr = L->Act->rptr; //Prvek bude vpravo ukazovat na prvek za aktivnim
            L->Act->rptr = tmp_Elem; //Aktivni bude vpravo ukazovat na novy
            if (L->Act == L->Last) { //Pokud je nove vlozeny posledni
                L->Last = tmp_Elem;
                }
            else {
                tmp_Elem->rptr->lptr = tmp_Elem; //Pokud neni posleni, prvek za aktivnim bude vlevo ukazovat na nove vlozeny
            }
        }
    }
}

void DLPreInsert (tDLList *L, int val) {
/*
** Vloží prvek před aktivní prvek seznamu L.
** Pokud nebyl seznam L aktivní, nic se neděje.
** V případě, že není dostatek paměti pro nový prvek při operaci malloc,
** volá funkci DLError().
**/
    if (L->Act != NULL) { //Seznam je aktivni
        tDLElemPtr tmp_Elem; //Vytvorime pomocny prvek
        tmp_Elem = malloc(sizeof(struct tDLElem)); //Alokujeme misto, pokud neni vyvolame chybu
        if (tmp_Elem == NULL) {
            DLError();
        }
        else {
            tmp_Elem->data = val; //Nahrajeme data do noveho prvku
            tmp_Elem->rptr = L->Act; //Prvek bude vpravo ukazovat na aktivni
            tmp_Elem->lptr = L->Act->lptr; //Prvek bude vlevo ukazovat na prvek pred aktivnim
            L->Act->lptr = tmp_Elem; //Aktivni bude vlevo ukazovat na novy
            if (L->Act == L->First) { //Pokud je nove vlozeny prvni
                L->First = tmp_Elem;
            }
            else {
                tmp_Elem->lptr->rptr = tmp_Elem; //Pokud neni prvni, prvek pred aktivnim bude vpravo ukazovat na nove vlozeny
            }
        }
    }
}

void DLCopy (tDLList *L, int *val) {
/*
** Prostřednictvím parametru val vrátí hodnotu aktivního prvku seznamu L.
** Pokud seznam L není aktivní, volá funkci DLError ().
**/
    if (L->Act == NULL) {
        DLError(); //Pokud neni aktivni vola error
    }
    else {
        *val = L->Act->data;
    }
}

void DLActualize (tDLList *L, int val) {
/*
** Přepíše obsah aktivního prvku seznamu L.
** Pokud seznam L není aktivní, nedělá nic.
**/
    if (L->Act != NULL) { //Pokud je seznam aktivni
        L->Act->data = val;
    }
}

void DLSucc (tDLList *L) {
/*
** Posune aktivitu na následující prvek seznamu L.
** Není-li seznam aktivní, nedělá nic.
** Všimněte si, že při aktivitě na posledním prvku se seznam stane neaktivním.
**/
    if (L->Act != NULL) {//Pokud je seznam aktivni
        if (L->Act == L->Last) {
            L->Act = NULL; //Pokud je posledni prvek aktivni, stane se nekativni
        }
        else {
            L->Act = L->Act->rptr;
        }
    }
}


void DLPred (tDLList *L) {
/*
** Posune aktivitu na předchozí prvek seznamu L.
** Není-li seznam aktivní, nedělá nic.
** Všimněte si, že při aktivitě na prvním prvku se seznam stane neaktivním.
**/
    if (L->Act != NULL) {//Pokud je seznam aktivni
        if (L->Act == L->First) {
            L->Act = NULL; //Pokud je prvni prvek aktivni, stane se nekativni
        }
        else {
            L->Act = L->Act->lptr;
        }
    }
}

int DLActive (tDLList *L) {
/*
** Je-li seznam L aktivní, vrací nenulovou hodnotu, jinak vrací 0.
** Funkci je vhodné implementovat jedním příkazem return.
**/
	return (L->Act != NULL); // Pokud neni prazdy, vrati true(1), jinak false(0)
}

/* Konec c206.c*/
