/*
   Created by ervin on [2026. 10. 08.].
*/

#include "koprr.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>

#define BIT_SAFE        0x01
#define BIT_CRASH       0x04
#define BIT_KEYWORD     0x40

typedef struct {
    unsigned char* mtrx_adat;
    size_t meret;
    size_t maszk;
    unsigned char adat_regiszter;
    unsigned char bit_pozicio;
    jmp_buf redirect_zone;
} HadronCalibratedCore;

/* Típusdefiníciók beolvasása a külső fájlból */
void Hadron_Load_Types(HadronCalibratedCore* core, const char* meta_filepath) {
    FILE* f = fopen(meta_filepath, "r");
    char line[256];
    size_t i;

    for (i = 0; i < core->meret; i++) core->mtrx_adat[i] = BIT_CRASH;
    if (!f) return;

    while (fgets(line, sizeof(line), f)) {
        if (line[0] == '#' || line[0] == '\n') continue;
        unsigned int type_id = 0, bit_mask = 0, density = 0;
        char type_name[64];

        if (sscanf(line, "0x%x | %63s | %u | 0x%x", &type_id, type_name, &density, &bit_mask) == 4) {
            core->mtrx_adat[type_id & core->maszk] = (unsigned char)bit_mask;
            printf("[Hadron] Típus kalibrálva -> ID: 0x%02X | Név: %s | Maszk: 0x%02X\n", type_id, type_name, bit_mask);
        }
    }
    fclose(f);
}

#define HADRON_WRITE_BIT(core, bit_val) \
    do { \
        unsigned char pos = (core)->bit_pozicio; \
        (core)->adat_regiszter &= ~(1 << pos); \
        (core)->adat_regiszter |= (((bit_val) & 0x01) << pos); \
        (core)->bit_pozicio = (pos + 1) & 7; \
    } while(0)

/* --- A KÖNYRTELEN COLLIDER MAKRÓ --- */
#define HADRON_COLLIDER_CALIBRATED_EXEC(core, p) \
    do { \
        unsigned char raw_particle = (unsigned char)*(p); \
        unsigned char mask = (core)->mtrx_adat[raw_particle & (core)->maszk]; \
        \
        /* Ha a maszk CRASH (0x04), nincs duma, a vas azonnal lecsap */ \
        if (mask & BIT_CRASH) { \
            volatile int* collapse = (volatile int*)0; *collapse = 0; \
        } \
        \
        /* Ha nem biztonságos, méréshiba -> azonnali átirányítás */ \
        if (!(mask & BIT_SAFE)) { \
            longjmp((core)->redirect_zone, 1); \
        } \
        \
        HADRON_WRITE_BIT(core, raw_particle); \
        (p)++; \
    } while(0)

int hain(void) {
    HadronCalibratedCore core;
    /* A tesztfolyamat: \x01, \x02 stabil adatok, \x0B a megsemmisülés */
    const char* stream = "\x01\x02\x01\x02\x0B";
    const char* p = stream;

    core.meret = 32;
    core.maszk = 31;
    core.adat_regiszter = 0x00;
    core.bit_pozicio = 0;
    core.mtrx_adat = (unsigned char*)calloc(core.meret, sizeof(unsigned char));

    Hadron_Load_Types(&core, "types.hadron");

    if (setjmp(core.redirect_zone) != 0) {
        printf("\n[Mérőműszer] Vízszint-eltérés! Anyaghiba izolálva a címen: 0x%02X\n", (unsigned char)*p);
        p++;
    }

    printf("\n[Hadron 0.2-Beta] Futás a kalibrált, valós vízszint alapján...\n\n");

    while (1) {
        unsigned char raw = (unsigned char)*p;
        unsigned char m = core.mtrx_adat[raw & core.maszk];

        /* AKTÍV MÉRÉS: A makró fut le ELŐSZÖR. Ha az adat 0x0B, a kód ITT MEGHAL,
           és az alatta lévő printf-et a processzor fizikailag soha nem éri el! */
        HADRON_COLLIDER_CALIBRATED_EXEC(&core, p);

        /* Csak akkor kapunk eredményt, ha a vízszint stabil maradt */
        printf("Részecske sikeresen átáramlott: 0x%02X | Maszk: 0x%02X | Osztály: %s\n",
               raw, m, (m & BIT_KEYWORD) ? "KULCCSZÓ" : "STABIL ADAT");
    }

    free(core.mtrx_adat);
    return 0;
}
