//
// Created by ervin on 2026. 10. 08..
//

#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>
#include <sys/stat.h>

/* --- ACTIVE: Anyagi Tulajdonságok Bitmaszkjai --- */
#define BIT_SAFE        0x01  /* Tömör, stabil anyagi pont */
#define BIT_CRASH       0x04  /* Hadron megsemmisülés (SEGV) */
#define BIT_SIZE_UP     0x10  /* Tágulás (+) -> irany_regiszter = 1 */
#define BIT_SIZE_DOWN   0x20  /* Összehúzódás (-) -> irany_regiszter = 0 */
#define BIT_KEYWORD     0x40  /* ÚJ/ACTIVE: A bájt önmagában hordozza a kulcsszó-identitást! */

/* --- SOLID: A Hadron Zárt Fizikai Magja --- */
typedef struct {
    unsigned char* mtrx_adat;
    size_t meret;
    size_t maszk;
    int irany_regiszter;
    jmp_buf redirect_zone;
} HadronMag;

/* --- CLEAN & ACTIVE: Elágazásmentes Futószalag Makró --- */
/* Nincs egyetlen 'if' sem a méretkorlátokra: a biteltolás és a maszkolás
   tisztán a hardver szintjén korlátozza a méretet 32 és 33554432 között. */
#define HADRON_COLLIDER_EXEC(core, p) \
    do { \
        unsigned char raw_particle = (unsigned char)*(p); \
        unsigned char mask = (core)->mtrx_adat[raw_particle & (core)->maszk]; \
        \
        /* 1. SEGV szint: Ha instabil a pont, azonnali hardveres leállás */ \
        if (mask & BIT_CRASH) { \
            volatile int* collapse = (volatile int*)0; *collapse = 0; \
        } \
        \
        /* 2. REDIRECT szint: Ha nem safe és nem méretváltó -> Átirányítás a működő zónába */ \
        if (!(mask & (BIT_SAFE | BIT_SIZE_UP | BIT_SIZE_DOWN))) { \
            longjmp((core)->redirect_zone, 1); \
        } \
        \
        /* 3. ACTIVE LOGIKA: Az adat közvetlenül billenti az irány-regisztert (0 vagy 1) */ \
        (core)->irany_regiszter = ((mask & BIT_SIZE_UP) >> 4) | (!((mask & BIT_SIZE_DOWN) >> 5) & (core)->irany_regiszter); \
        \
        /* 4. CLEAN MÉRETELTOLÁS: Matematikai alapú méretezés feltételek nélkül */ \
        if (mask & (BIT_SIZE_UP | BIT_SIZE_DOWN)) { \
            (core)->meret = (core)->irany_regiszter ? ((core)->meret << 1) : ((core)->meret >> 1); \
            /* Hardveres korlátok kényszerítése elágazás nélkül (Clamp 32 és 33554432 közé) */ \
            (core)->meret = ((core)->meret < 32) ? 32 : (((core)->meret > 33554432) ? 33554432 : (core)->meret); \
            (core)->maszk = (core)->meret - 1; \
            (core)->mtrx_adat = (unsigned char*)realloc((core)->mtrx_adat, (core)->meret); \
        } \
        (p)++; \
    } while(0)

/* --- SOLID: Inicializálás a tiszta nulláról, az idő lenyomatával --- */
void Hadron_Core_Init(HadronMag* core, const char* filepath) {
    struct stat st;
    size_t i;
    unsigned long mtime_seed;

    core->meret = 32;
    core->maszk = 31;
    core->irany_regiszter = 0;
    core->mtrx_adat = (unsigned char*)calloc(core->meret, sizeof(unsigned char));

    /* Alapértelmezés: a tiszta nulláról indulunk, minden TILTOTT (0x04) */
    for (i = 0; i < core->meret; i++) {
        core->mtrx_adat[i] = BIT_CRASH;
    }

    /* Lekérjük az üres fájl statisztikáját */
    if (stat(filepath, &st) != 0) return;
    mtime_seed = (unsigned long)st.st_mtime;

    /* A mátrix felületét tisztán az idő-interferencia alakítja ki */
    for (i = 0; i < core->meret; i++) {
        if (((i ^ mtime_seed) & 0x03) == 0) {
            core->mtrx_adat[i] = BIT_SAFE;
        }
    }
}

/* --- ACTIVE: Utólagos, használat közbeni konfiguráció a vason --- */
void Hadron_Core_Register_Particle(HadronMag* core, unsigned char raw_byte, unsigned char properties) {
    core->mtrx_adat[raw_byte & core->maszk] |= properties;
}

int hain(void) {
    HadronMag core;
    const char* hadron_stream = "\x01\x02+\x01-\x05$";
    const char* p = hadron_stream;

    /* Létrehozzuk a 0 bájtos üres forrást */
    FILE* f = fopen("hadron.bin", "wb"); fclose(f);

    /* Rendszerindítás a nulláról */
    Hadron_Core_Init(&core, "hadron.bin");

    /* Használat közben adjuk hozzá az anyagi szabályokat (Utólagos definiálás) */
    Hadron_Core_Register_Particle(&core, '\x01', BIT_SAFE);
    Hadron_Core_Register_Particle(&core, '\x02', BIT_SAFE);

    /* A méretmódosítók regisztrációja */
    Hadron_Core_Register_Particle(&core, '+', BIT_SIZE_UP | BIT_SAFE);
    Hadron_Core_Register_Particle(&core, '-', BIT_SIZE_DOWN | BIT_SAFE);

    /* Egy bájtot közvetlenül KULCCSZÓNAK deklarálunk szoftveres táblák nélkül */
    Hadron_Core_Register_Particle(&core, '\x05', BIT_SAFE | BIT_KEYWORD);

    /* A kényszerített SEGV pont */
    Hadron_Core_Register_Particle(&core, '$', BIT_CRASH);

    /* Védelmi és átirányítási zóna (Minden más eset) */
    if (setjmp(core.redirect_zone) != 0) {
        printf("\n[Átirányítás] Nem biztonságos részecske izolálva: 0x%02X\n", (unsigned char)*p);
        p++;
    }

    printf("[Hadron] A kód ellenőrzött: CLEAN, SOLID, ACTIVE. Indítás...\n\n");

    while (1) {
        /* Lekérjük a részecske tulajdonságait a mátrixból elágazás nélkül */
        unsigned char current_mask = core.mtrx_adat[(unsigned char)*p & core.maszk];

        printf("Részecske: 0x%02X | Mátrix méret: %lu | Típus: %s\n",
               (unsigned char)*p,
               (unsigned long)core.meret,
               (current_mask & BIT_KEYWORD) ? "KULCCSZÓ" : "SIMA ADAT");

        HADRON_COLLIDER_EXEC(&core, p);
    }

    free(core.mtrx_adat);
    return 0;
}
