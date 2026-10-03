/*
    Created by ervin on [2026. 09. 25.].
*/

#ifndef HADRON_PUSH_H
#define HADRON_PUSH_H
#include "../type.h"
#define MAX_SIZE 1024

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ELEM_SZAM 1024

/* Csúsztatási irányok típusdefiníciója */
typedef enum {
    CSUSZTAT_BALRA,  /* Új elem a végére jön, a régi elemek balra (eleje felé) csúsznak */
    CSUSZTAT_JOBBRA  /* Új elem az elejére jön, a régi elemek jobbra (vége felé) csúsznak */
} CsusztatasIrany;

/* Szigorú, típusbiztos és irányított tömbkonténer */
typedef struct {
    unsigned char memoria[ELEM_SZAM * 16]; /* Fix helyfoglalás (pl. max 16 bájtos elemeknek) */
    size_t elem_meret;                     /* A kényszerített alaptípus mérete (sizeof) */
    size_t jelenlegi_darabszam;            /* Hány valós elem van most a tömbben */
    CsusztatasIrany irany;                 /* Merre csússzanak az elemek beszúráskor */
} SzigoruTomb;

typedef struct {
    void *data;          /* Most már hivatalosan is tisztán void*! Bármit elnyel. */
    size_t element_size; /* Egyetlen elem mérete bájtokban */
    int write_index;
    int read_index;
    int count;
} AmyGenericBuffer;

/*
 * AMY_PERFECT_PUSH:
 * Nincs status paraméter! Nincs maradékos osztás (%)! Nincs lassú for ciklus!
 * A memmove hardveres szinten, villámgyorsan tolja balra az elemeket,
 * így a legújabb adat (value) MINDIG pontosan a tömb legvégére kerül!
 */
#define AMY_PERFECT_PUSH(arr, count, value) \
    do { \
        if ((count) < MAX_SIZE) { \
            (arr)[(count)] = (value); \
            (count)++; \
        } else { \
            /* Az 1-es indextől kezdve mindent eltolunk a 0. indexre (a legrégebbi kiesik) */ \
            memmove(&(arr)[0], &(arr)[1], (MAX_SIZE - 1) * sizeof((arr)[0])); \
            (arr)[MAX_SIZE - 1] = (value); /* SZIGORÚAN A LEGVEGÉRE! */ \
        } \
    } while(0)

/* Tails-féle gyors tizes alapú PUSH */
#define TAILS_DECIMAL_FAST_PUSH(arr, head, tail, count, value, status) \
    do { \
        (arr)[(head)] = (value); \
        (head)++; \
        if ((head) == MAX_SIZE) { (head) = 0; } \
        if ((count) < MAX_SIZE) { \
            (count)++; \
            (status) = STATUS_OK; \
        } else { \
            (tail)++; \
            if ((tail) == MAX_SIZE) { (tail) = 0; } \
            (status) = STATUS_OVERWRITE; \
        } \
    } while(0)

/* Tails-féle gyors tizes alapú POP */
#define TAILS_DECIMAL_FAST_POP(arr, head, tail, count, out_val, status) \
    do { \
        if ((count) > 0) { \
            (out_val) = (arr)[(tail)]; \
            (tail)++; \
            if ((tail) == MAX_SIZE) { (tail) = 0; } \
            (count)--; \
            (status) = STATUS_OK; \
        } else { \
            (status) = STATUS_EMPTY; \
        } \
    } while(0)

typedef struct {
    int a;
} m;

int pain(void);
void push(void* value);
void* get(int index);

#endif /* HADRON_PUSH_H */
