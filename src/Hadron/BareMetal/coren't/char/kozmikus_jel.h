/*
   Created by ervin on [2026. 09. 29.].
*/

/* kozmikus_jel.h - Szigoru C90-es fejlécfile */
#ifndef HADRON_KOZMIKUS_JEL_H
#define HADRON_KOZMIKUS_JEL_H

#define BUFFER_SIZE 8
#define BUFFER_MASK 7

typedef enum {
   VONAL     = 0,
   KOR       = 1,
   HAROMSZOG = 2,
   NEGYSZOG  = 3
} AlakzatTipus;

typedef struct {
   AlakzatTipus tipus;
   int x; /* 0-3 koordinata */
   int y; /* 0-3 koordinata */
   int meret; /* 1: kicsi, 2: nagy */
} TisztaAlakzat;

typedef struct {
   unsigned long jel_id;
   TisztaAlakzat alakzat;
} TisztaJel;

/* Interfészek: A Puffer és a Gyártó külön él! */
void puffer_inicializal(void);
void puffer_mentes(TisztaJel kesz_jel);
int puffer_kiolvasas(TisztaJel *kimenet);

TisztaJel jel_gyarto(unsigned long id, AlakzatTipus tipus, int x, int y, int meret);

#endif /* HADRON_KOZMIKUS_JEL_H */
