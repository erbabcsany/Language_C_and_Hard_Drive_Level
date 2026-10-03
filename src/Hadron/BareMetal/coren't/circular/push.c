/*
   Created by ervin on [2026. 09. 26.].
*/

#include "push.h"

#include <string.h> /* Szükséges az Amy-féle memmove függvényhez */

#include <stddef.h>
#include <string.h>

/* A puffer mérete itt is kötelezően 2 hatványa a bitművelet miatt */
#define BUFFER_SIZE 8
#define BUFFER_MASK (BUFFER_SIZE - 1)
#define TOMB_SIZE 4

static SzigoruTomb TOMB[MAX_SIZE];
static int INDEX = 0;

#include <stddef.h>
#include <string.h>

/* 1. INICIALIZÁLÁS: Itt kötjük ki a fix méretű típust és az irányt */
void tomb_letrehozas(SzigoruTomb *t, size_t szigorup_tipus_meret, CsusztatasIrany kivant_irany) {
    t->elem_meret = szigorup_tipus_meret;
    t->jelenlegi_darabszam = 0;
    t->irany = kivant_irany;
    memset(t->memoria, 0, sizeof(t->memoria));
}

/* 2. BESZÚRÁS ÉS CSÚSZTATÁS */
int tomb_uj_elem(SzigoruTomb *t, const void *uj_ertek) {
    unsigned char *tomb_eleje = t->memoria;

    if (t->irany == CSUSZTAT_BALRA) {
        /* Ha betelt a tömb, eltoljuk balra az elemeket (az első elem elvész) */
        if (t->jelenlegi_darabszam == ELEM_SZAM) {
            memmove(tomb_eleje, tomb_eleje + t->elem_meret, (ELEM_SZAM - 1) * t->elem_meret);
            t->jelenlegi_darabszam--;
        }
        /* Az új elemet a tömb aktuális végére másoljuk */
        memcpy(tomb_eleje + (t->jelenlegi_darabszam * t->elem_meret), uj_ertek, t->elem_meret);
        t->jelenlegi_darabszam++;
    }
    else if (t->irany == CSUSZTAT_JOBBRA) {
        /* Minden elemet eggyel jobbra tolunk, hogy felszabadítsuk a 0. indexet */
        size_t mozgathato_elemek = (t->jelenlegi_darabszam < ELEM_SZAM) ? t->jelenlegi_darabszam : (ELEM_SZAM - 1);

        if (mozgathato_elemek > 0) {
            memmove(tomb_eleje + t->elem_meret, tomb_eleje, mozgathato_elemek * t->elem_meret);
        }
        /* Az új elemet kényszerítve beírjuk a legelső (0.) helyre */
        memcpy(tomb_eleje, uj_ertek, t->elem_meret);

        if (t->jelenlegi_darabszam < ELEM_SZAM) {
            t->jelenlegi_darabszam++;
        }
    }
    return 1;
}

/* 3. SZIGORÚ, KÉNYSZERÍTETT ELVÉTEL TYPE-CHECKINGGEL
   Ha a hívó nem pontosan olyan méretű változót ad át, mint amire a tömböt hitelesítették,
   a függvény megtagadja a futást és 0-t ad vissza, megvédve a memóriát. */
int tomb_kenyszeritett_elvetel(SzigoruTomb *t, void *cel_valtozo, size_t ellenorzo_tipus_meret) {
    unsigned char *tomb_eleje = t->memoria;

    /* Szigorú típusellenőrzés: ha a méret nem egyezik, a tranzakció érvénytelen */
    if (ellenorzo_tipus_meret != t->elem_meret || t->jelenlegi_darabszam == 0) {
        return 0;
    }

    if (t->irany == CSUSZTAT_BALRA) {
        /* BALRA irány esetén a legrégebbi elem a 0. indexen van, azt vesszük ki */
        memcpy(cel_valtozo, tomb_eleje, t->elem_meret);
        /* A maradék elemeket előrecsúsztatjuk a felszabadult helyre */
        memmove(tomb_eleje, tomb_eleje + t->elem_meret, (t->jelenlegi_darabszam - 1) * t->elem_meret);
    }
    else {
        /* JOBBRA irány esetén az utolsó indexen lévő elemet vesszük ki */
        size_t utolso_index = t->jelenlegi_darabszam - 1;
        memcpy(cel_valtozo, tomb_eleje + (utolso_index * t->elem_meret), t->elem_meret);
    }

    t->jelenlegi_darabszam--;
    return 1; /* Sikeres, típusbiztos elvétel */
}

int pain(void) {
    SzigoruTomb adatok;
    int uj_adat = 550;
    int kimenet = 0;
    char rossz_tipusu_valtozo = 'A';

    /* Létrehozunk egy 1024 elemű int tömböt, ami beszúráskor BALRA csúszik */
    tomb_letrehozas(&adatok, sizeof(int), CSUSZTAT_BALRA);

    /* Elem hozzáadása */
    tomb_uj_elem(&adatok, &uj_adat);

    /* HIBÁS ELVÉTELI KÍSÉRLET: char-t passzolunk be egy int tömbhöz */
    /* A függvény 0-t ad vissza, mert a sizeof(char) != adatok.elem_meret */
    if (!tomb_kenyszeritett_elvetel(&adatok, &rossz_tipusu_valtozo, sizeof(char))) {
        /* Ide ugrik: A típus-kényszerítés megakadályozta a memóriaszemét keletkezését */
    }

    /* HELYES, SZIGORÍTOTT ELVÉTEL: int-et passzolunk be, a méretek egyeznek */
    if (tomb_kenyszeritett_elvetel(&adatok, &kimenet, sizeof(int))) {
        /* Sikerült! A kimenet értéke most már 550, és a tömb elemei elcsúsztak. */
    }

    return 0;
}

void push(void *value) {
   SzigoruTomb int_puffer;

   /* Inicializáláskor átadjuk a külső tömböt és annak típusméretét */
   tomb_letrehozas(&int_puffer, TOMB->elem_meret, sizeof(&value));

   /* Írás a pufferbe */
   tomb_uj_elem(&int_puffer, &value);
}

void* get(const int index) {
   return TOMB[index].memoria;
}
