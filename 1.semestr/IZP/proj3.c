/**
 * Kostra programu pro 3. projekt IZP 2015/16
 *
 * Jednoducha shlukova analyza: 2D nejblizsi soused.
 * Single linkage
 * http://is.muni.cz/th/172767/fi_b/5739129/web/web/slsrov.html
 * Created by Petr Juda on 29.11.15.
 * Copyright © 2015 Petr Juda. All rights reserved.
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h> // sqrtf
#include <limits.h> // INT_MAX
#include <string.h>

/*****************************************************************
 * Ladici makra. Vypnout jejich efekt lze definici makra
 * NDEBUG, napr.:
 *   a) pri prekladu argumentem prekladaci -DNDEBUG
 *   b) v souboru (na radek pred #include <assert.h>
 *      #define NDEBUG
 */
#ifdef NDEBUG
#define debug(s)
#define dfmt(s, ...)
#define dint(i)
#define dfloat(f)
#else

// vypise ladici retezec
#define debug(s) printf("- %s\n", s)

// vypise formatovany ladici vystup - pouziti podobne jako printf
#define dfmt(s, ...) printf(" - "__FILE__":%u: "s"\n",__LINE__,__VA_ARGS__)

// vypise ladici informaci o promenne - pouziti dint(identifikator_promenne)
#define dint(i) printf(" - " __FILE__ ":%u: " #i " = %d\n", __LINE__, i)

// vypise ladici informaci o promenne typu float - pouziti
// dfloat(identifikator_promenne)
#define dfloat(f) printf(" - " __FILE__ ":%u: " #f " = %g\n", __LINE__, f)

#endif

/*****************************************************************
 * Deklarace potrebnych datovych typu:
 *
 * TYTO DEKLARACE NEMENTE
 *
 *   struct objt_t - struktura objektu: identifikator a souradnice
 *   struct cluster_t - shluk objektu:
 *      pocet objektu ve shluku,
 *      kapacita shluku (pocet objektu, pro ktere je rezervovano
 *          misto v poli),
 *      ukazatel na pole shluku.
 */

struct obj_t {
    int id;
    float x;
    float y;
};

struct cluster_t {
    int size;
    int capacity;
    struct obj_t *obj;
};

/*****************************************************************
 * Deklarace potrebnych funkci.
 *
 * PROTOTYPY FUNKCI NEMENTE
 *
 * IMPLEMENTUJTE POUZE FUNKCE NA MISTECH OZNACENYCH 'TODO'
 *
 */

/*
 Inicializace shluku 'c'. Alokuje pamet pro cap objektu (kapacitu).
 Ukazatel NULL u pole objektu znamena kapacitu 0.
 */
void init_cluster(struct cluster_t *c, int cap)
{
    assert(c != NULL);
    assert(cap >= 0);
    // TODO
    c->capacity = cap;
    //OBJ ALOKUJE KAPACITU PRO OBJEKTY
    c->obj = malloc(cap*sizeof(struct obj_t));
    if (c->obj == NULL) {
        fputs("Nepovedlo se alokovat pole objektu.\n", stderr);
        c->capacity = 0;
    }
    
}

/*
 Odstraneni vsech objektu shluku a inicializace na prazdny shluk.
 */
void clear_cluster(struct cluster_t *c)
{
    // TODO
    c->capacity = 0;
    c->size = 0;
    free(c->obj);
}

/// Chunk of cluster objects. Value recommended for reallocation.
const int CLUSTER_CHUNK = 10;

/*
 Zmena kapacity shluku 'c' na kapacitu 'new_cap'.
 */
struct cluster_t *resize_cluster(struct cluster_t *c, int new_cap)
{
    // TUTO FUNKCI NEMENTE
    assert(c);
    assert(c->capacity >= 0);
    assert(new_cap >= 0);
    
    if (c->capacity >= new_cap)
        return c;
    
    size_t size = sizeof(struct obj_t) * new_cap;
    
    void *arr = realloc(c->obj, size);
    if (arr == NULL)
        return NULL;
    
    c->obj = arr;
    c->capacity = new_cap;
    return c;
}

/*
 Prida objekt 'obj' na konec shluku 'c'. Rozsiri shluk, pokud se do nej objekt
 nevejde.
 */
void append_cluster(struct cluster_t *c, struct obj_t obj)
{
    // TODO
    //POKUD KAPACITA NEDOSTACUJE, ROZSIRI CLUSTER
    if (c->size == c->capacity) {
        resize_cluster(c, c->capacity * CLUSTER_CHUNK);
    }
    //NA INDEX SIZE PRIRADI HODNOTY OBJEKTU
    c->obj[c->size].id = obj.id;
    c->obj[c->size].x = obj.x;
    c->obj[c->size].y = obj.y;
    c->size++;
}

/*
 Seradi objekty ve shluku 'c' vzestupne podle jejich identifikacniho cisla.
 */
void sort_cluster(struct cluster_t *c);

/*
 Do shluku 'c1' prida objekty 'c2'. Shluk 'c1' bude v pripade nutnosti rozsiren.
 Objekty ve shluku 'c1' budou serazny vzestupne podle identifikacniho cisla.
 Shluk 'c2' bude nezmenen.
 */
void merge_clusters(struct cluster_t *c1, struct cluster_t *c2)
{
    assert(c1 != NULL);
    assert(c2 != NULL);
    
    // TODO
    //OBJEKTY C2 PRIRAD DO C1
    for (int i = 0; i < c2->size; i++) {
        append_cluster(c1, c2->obj[i]);
    }
    //SERADI OBJEKTY PODLE ID
    sort_cluster(c1);
}

/**********************************************************************/
/* Prace s polem shluku */

/*
 Odstrani shluk z pole shluku 'carr'. Pole shluku obsahuje 'narr' polozek
 (shluku). Shluk pro odstraneni se nachazi na indexu 'idx'. Funkce vraci novy
 pocet shluku v poli.
 */
int remove_cluster(struct cluster_t *carr, int narr, int idx)
{
    assert(idx < narr);
    assert(narr > 0);
    
    // TODO
    //UVOLNI OBJETY CLUSTRU S INDEXEM IDX
    clear_cluster(&carr[idx]);
    //POSUNE ZBYTEK CLUSTERU DOLEVA
    for (int i = idx +1, y = idx; i<narr; i++, y++) {
        carr[y].size = carr[i].size;
        carr[y].capacity = carr[i].capacity;
        carr[y].obj = carr[i].obj;
    }
    //POKUD SE NEJEDNALO O POSLEDNI PRVEK V POLI, VYNULUJE HO
    if (idx+1 != narr) {
        carr[narr-1].capacity = 0;
        carr[narr-1].size = 0;
        carr[narr-1].obj = NULL;
    }
    return narr-1;
}

/*
 Pocita Euklidovskou vzdalenost mezi dvema objekty.
 */
float obj_distance(struct obj_t *o1, struct obj_t *o2)
{
    assert(o1 != NULL);
    assert(o2 != NULL);
    
    // TODO
    float vzdalenost;
    //VOREC NA VYPOCET VZDALENOSTI VEKTORU
    vzdalenost = sqrtf((o2->x - o1->x)*(o2->x - o1->x) + (o2->y - o1->y)*(o2->y - o1->y));
    return vzdalenost;
}

/*
 Pocita vzdalenost dvou shluku. Vzdalenost je vypoctena na zaklade nejblizsiho
 souseda.
 */
float cluster_distance(struct cluster_t *c1, struct cluster_t *c2)
{
    assert(c1 != NULL);
    assert(c1->size > 0);
    assert(c2 != NULL);
    assert(c2->size > 0);
    
    // TODO
    //DO POMOCNE PRIRADI VZDALENOST PRVNICH OBJEKTU
    float pomocna = obj_distance(&c1->obj[0], &c2->obj[0]);
    //POROVNA OBJEKTY C1 S C2 A VYBERE NEJMENE VZDALENE PRVKY
    for (int i = 0; i < c1->size; i++) {
        for (int j=0; j < c2->size; j++) {
            if ((obj_distance(&c1->obj[i], &c2->obj[j])) < pomocna) {
                pomocna = obj_distance(&c1->obj[i], &c2->obj[j]);
            }
        }
    }
    return pomocna;
}

/*
 Funkce najde dva nejblizsi shluky. V poli shluku 'carr' o velikosti 'narr'
 hleda dva nejblizsi shluky (podle nejblizsiho souseda). Nalezene shluky
 identifikuje jejich indexy v poli 'carr'. Funkce nalezene shluky (indexy do
 pole 'carr') uklada do pameti na adresu 'c1' resp. 'c2'.
 */
void find_neighbours(struct cluster_t *carr, int narr, int *c1, int *c2)
{
    assert(narr > 0);
    
    // TODO
    //DO POMOCNE PRIRADI VZDALENOST PRVNICH CLUSTERU
    float pomocna = cluster_distance(&carr[0], &carr[1]);
    //NASTAV INDEXI NA POCATECNI HODNOTU
    *c1 = 0;
    *c2 = 1;
    //HLEDA NEJBIZSI CLUSTERY (POUZE JEDNOTNE KOMBINACE)
    for (int i = 0; i<narr-1; i++) {
        for (int j = i+1; j<narr; j++) {
            if (cluster_distance(&carr[i], &carr[j]) < pomocna) {
                *c1 = i;
                *c2 = j;
                pomocna = cluster_distance(&carr[i], &carr[j]);
            }
        }
    }
}

// pomocna funkce pro razeni shluku
static int obj_sort_compar(const void *a, const void *b)
{
    // TUTO FUNKCI NEMENTE
    const struct obj_t *o1 = a;
    const struct obj_t *o2 = b;
    if (o1->id < o2->id) return -1;
    if (o1->id > o2->id) return 1;
    return 0;
}

/*
 Razeni objektu ve shluku vzestupne podle jejich identifikatoru.
 */
void sort_cluster(struct cluster_t *c)
{
    // TUTO FUNKCI NEMENTE
    qsort(c->obj, c->size, sizeof(struct obj_t), &obj_sort_compar);
}

/*
 Tisk shluku 'c' na stdout.
 */
void print_cluster(struct cluster_t *c)
{
    // TUTO FUNKCI NEMENTE
    for (int i = 0; i < c->size; i++)
    {
        if (i) putchar(' ');
        printf("%d[%g,%g]", c->obj[i].id, c->obj[i].x, c->obj[i].y);
    }
    putchar('\n');
}

/*
 Ze souboru 'filename' nacte objekty. Pro kazdy objekt vytvori shluk a ulozi
 jej do pole shluku. Alokuje prostor pro pole vsech shluku a ukazatel na prvni
 polozku pole (ukalazatel na prvni shluk v alokovanem poli) ulozi do pameti,
 kam se odkazuje parametr 'arr'. Funkce vraci pocet nactenych objektu (shluku).
 V pripade nejake chyby uklada do pameti, kam se odkazuje 'arr', hodnotu NULL.
 */
int load_clusters(char *filename, struct cluster_t **arr)
{
    assert(arr != NULL);
    // TODO
    //OTEVRENI SOUBORU
    FILE *f;
    if ((f = fopen(filename, "r")) == NULL) {
        fputs("Soubor se nepodarilo otevrit.\n", stderr);
        return 2;
    }
    //OVERENI PRVNIHO RADKU A NACTENI A OVERENI COUNT
    char text[7];
    int pocet;
    fgets(text, 7, f);
    if (strcmp(text, "count=") != 0) {
        fputs("Chyba na prvnim radku souboru.\n", stderr);
        fclose(f);
        return 0;
    }
    if (fscanf(f, "%d", &pocet) != 1) {
        fputs("Nepodarilo se nacist count.\n", stderr);
        fclose(f);
        return 0;
    }
    if (pocet < 1) {
        fputs("Count je mensi nez 1!\n", stderr);
        fclose(f);
        return 0;
    }
    //ALOKOVANI POLE CLUSTERU
    int objid, x, y;
    *arr = malloc(pocet*sizeof(struct cluster_t));
    //NACITANI OBJEKTU
    int pocitani = 0;
    for (int i=0; i < pocet; i++) {
        
        if (fscanf(f, "%d%d%d", &objid, &x, &y) != 3) {
            fputs("Nepodarilo se nacist objekt.\n", stderr);
            fclose(f);
            return -pocitani;
        }
        
        if (((x < 0) || (x > 1000)) || ((y < 0) || (y > 1000))) {
            fputs("Hodnota souradnic je mimo rozsah.\n", stderr);
            fclose(f);
            return -pocitani;
        }
        //INICIALIZACE CLUSTERU A ULOZENI OBJEKTU
        init_cluster(&(*arr)[i], 1);
        (*arr)[i].size = 1;
        (*arr)[i].obj[0].id = objid;
        (*arr)[i].obj[0].x = x;
        (*arr)[i].obj[0].y = y;
        pocitani++;
    }
    fclose(f);
    return pocet;
}

/*
 Tisk pole shluku. Parametr 'carr' je ukazatel na prvni polozku (shluk).
 Tiskne se prvnich 'narr' shluku.
 */
void print_clusters(struct cluster_t *carr, int narr)
{
    printf("Clusters:\n");
    for (int i = 0; i < narr; i++)
    {
        printf("cluster %d: ", i);
        print_cluster(&carr[i]);
    }
}

int main(int argc, char *argv[])
{
    struct cluster_t *clusters;
    
    if (argc > 1) { // POKUD JE POCET ARGUMENTU VETSI NEZ JEDNA.
        int pocet_shluku =1 ;
        char *filename = NULL, *endptr = NULL;
        if ((argc == 2) || (argc == 3)) { //HLIDANI POCTU ARGUMENTU
            
            if (argc == 2) { //NACTENI JMENA SOUBORU
                filename = argv[1];
            }
            if (argc == 3) {
                filename = argv[1];
                long test;
                if (((test = (strtol (argv[2], &endptr, 10)
                              )) != 0) && (test != -1)) { //NACTENI A KONTROLA N
                    if (*endptr != '\0') {
                        fputs("Chyba prevodu 2. argumentu.\n", stderr);
                        return 1;
                    }
                    if (test < 1) {
                        fputs("Zadany pocet N musi byt > 0!\n", stderr);
                        return 1;
                    }
                    if (test > INT_MAX) {
                        fputs("Zadany pocet iteraci je vetsi jak rozsah INTMAX!\n", stderr);
                        return 2;
                    }
                    else {
                        pocet_shluku = (unsigned int)test;
                    }
                }
                else {
                    fputs("Chyba vstupu! (druhy argument)\n", stderr);
                    return 1;
                }
            }
            // TODO
            /// PREVEDENE A ZKONTROLOVANE ARGUMENTY
            int pocet_cluster, c1, c2;
            //NACTENI SOUBORU
            pocet_cluster= load_clusters(filename, &clusters);
            //OSETRENI NACTENI
            if (pocet_cluster == 0) {
                return 2;
            }
            if (pocet_cluster < 1) {
                fputs("Nepodarilo se nacist data ze souboru.\n", stderr);
                for (int i=0; i < pocet_cluster*(-1); i++) {//PRI CHYBE VRACI ZAPORNY POCET RADKU, KTERE ODALOKUJE
                    clear_cluster(&clusters[i]);
                }
                free(&clusters[0]);
                return 2;
            }
            if (pocet_shluku > pocet_cluster) {
                fputs("Pocet pozadovanych shluku je vetsi nez pocet objektu.\n", stderr);
                for (int i=0; i < pocet_cluster; i++) {
                    clear_cluster(&clusters[i]);
                }
                free(&clusters[0]);
                return 2;
            }
            //ALGORITMUS PRO SHLUKOVANI OBJEKTU
            while (pocet_cluster > pocet_shluku) {
                find_neighbours(clusters, pocet_cluster, &c1, &c2);
                merge_clusters(&clusters[c1], &clusters[c2]);
                pocet_cluster = remove_cluster(clusters, pocet_cluster, c2);
            }
            print_clusters(clusters, pocet_cluster);
            //VYCISTENI PAMETI
            for (int i = 0; i != pocet_cluster; i++) {
                clear_cluster(&clusters[i]);
            }
            free(&clusters[0]);
            
        }
        else {
            // KDYZ JE POCET ARGUMENTU VETSI NEZ 3
            fputs("Chyba vstupu! (Nespravny pocet argumentu!)\n", stderr);
            return 1;
        }
    }
    else { // POKUD JE PROGRAM SPUSTEN BEZ ARGUMENTU.
        fputs("Program je spusten bez argumentu!\n", stderr);
        return 1;
    }
    return 0;
}