/*
   Created by ervin on [2026. 09. 29.].
*/

/* kozmikus_jel.c - Szigoru C90-es forrasfile */
#include "kozmikus_jel.h"

/* A PUFFER MODUL: Csak a tarolasert felel, a szintaxishoz semmi koze */
static TisztaJel TOMB[BUFFER_SIZE];
static int IRASI_INDEX = 0;
static int OLVASASI_INDEX = 0;
static int ELEMEK_SZAMA = 0;

void puffer_inicializal(void) {
    IRASI_INDEX = 0;
    OLVASASI_INDEX = 0;
    ELEMEK_SZAMA = 0;
}

void puffer_mentes(TisztaJel kesz_jel) {
    TOMB[IRASI_INDEX] = kesz_jel;
    IRASI_INDEX = (IRASI_INDEX + 1) & BUFFER_MASK;

    if (ELEMEK_SZAMA < BUFFER_SIZE) {
        ELEMEK_SZAMA++;
    } else {
        OLVASASI_INDEX = (OLVASASI_INDEX + 1) & BUFFER_MASK;
    }
}

int puffer_kiolvasas(TisztaJel *kimenet) {
    if (ELEMEK_SZAMA == 0) return 0;
    *kimenet = TOMB[OLVASASI_INDEX];
    OLVASASI_INDEX = (OLVASASI_INDEX + 1) & BUFFER_MASK;
    ELEMEK_SZAMA--;
    return 1;
}

/* A SZINTAXIS MODUL: Csak az adatok osszerakasaert felel, a pufferrol nem tud */
TisztaJel jel_gyarto(unsigned long id, AlakzatTipus tipus, int x, int y, int meret) {
    TisztaJel uj_jel;
    uj_jel.jel_id = id;
    uj_jel.alakzat.tipus = tipus;
    uj_jel.alakzat.x = x;
    uj_jel.alakzat.y = y;
    uj_jel.alakzat.meret = meret;
    return uj_jel;
}
