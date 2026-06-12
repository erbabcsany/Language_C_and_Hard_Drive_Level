//
// Created by ervin on [2026. 04. 08.].
//

#include <math.h>
#include "wave_function.h"

/* A tiszta fuggveny: az idotengely metszete a strukturaval */
double calculate_wave_point(HadronWave *wave, double time_t) {
    /* C90 szigor: minden valtozot a blokk elejen kell deklaralni! */
    double envelope;
    double oscillation;
    double result;

    /* Zero Tolerancia: negativ idoben nincs adat */
    if (time_t < 0.0) {
        return 0.0;
    }

    /* 1. Fazis: A csillapitas kiszamitasa (a falak felemesztik az energiat) */
    envelope = wave->amplitude * exp(-1.0 * wave->decay * time_t);

    /* 2. Fazis: A fluktuacio kiszamitasa */
    oscillation = cos((wave->frequency * time_t) + wave->phase);

    /* 3. Fazis: A ket kamra talalkozasa (Feszultseg * Racz) */
    result = envelope * oscillation;

    return result;
}

#include <stdio.h>
#include <math.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

/* A VOLATILE ZSILIP
   A volatile kulcsszo jelzi a forditonak, hogy ez a memoriacim barmikor,
   barmilyen masodperctorthuszad alatt megvaltozhat a hardver altal.
   Zeren tolerancia az optimalizacios illuziokra: a processzornak mindig
   a nyers RAM-bol kell kiolvasnia. */
volatile double global_time = 0.0;
volatile double current_voltage = 0.0;
volatile int system_halt = 0;

/* A horgonyzott struktura (A tiszta allapot) */
typedef struct {
    double amplitude;
    double decay;
    double frequency;
    double phase;
    double time_step;
} HadronState;

/* Globális statikus beallitas a 6->1 lecsengeshez */
HadronState core = {111.0, 0.5, 2.0, 0.0, 0.1};

/* =========================================================================
   AZ ISR (INTERRUPT SERVICE ROUTINE) - A Hardveres Kivetules
   Ez a fuggveny a "semmibol" csap le, amikor a rendszer orajele ut.
   ========================================================================= */
void hadron_interrupt_vector(int signum) {
    double envelope;
    double oscillation;

    /* Ha a lecsenges befejezodott (Stabil Adat), a megszakitas csendben kilep */
    if (system_halt) {
        return;
    }

    /* 1. Kiszamitas a pillanatnyi globalis idoben */
    envelope = core.amplitude * exp(-1.0 * core.decay * global_time);
    oscillation = cos((core.frequency * global_time) + core.phase);

    current_voltage = envelope * oscillation;

    /* 2. Zéro Tolerancia ellenorzes: Lehorgonyzas */
    /* Amint az elnyeles (envelope) egysegnyi feszultseg ala esik, a tuz elaludt. */
    if (envelope < 1.0) {
        system_halt = 1;
        printf("\n[VAS TELEMETRIA] Hadron-Mag lehorgonyozva. Stabil Adat elerve (E < 1).\n");
    } else {
        printf("[INTERRUPT %05.2f] Feszultseg: %07.2f | Puffer-Zaj: %07.2f\n",
               global_time, envelope, current_voltage);
    }

    /* 3. A mutato tovabbleptetese a kovetkezo hardveres utesig */
    global_time += core.time_step;

    /* Visszakapcsoljuk az orajelet (rendszertol fuggoen szukseges lehet C90-ben) */
    signal(SIGALRM, hadron_interrupt_vector);
}

/* =========================================================================
   A FO CIKLUS - A Zaj (Ami megszakitast szenved)
   ========================================================================= */
int maina(void) {
    printf("Hadron Rendszer Inicializalasa... varakozas az 1. megszakitasra.\n");
    printf("--------------------------------------------------------------\n");

    /* Az interrupt vektor (zsilip) elesitese a Vas szintjen */
    signal(SIGALRM, hadron_interrupt_vector);

    /* Az elso hardveres idozito (timer) elsutese 1 masodperc mulva */
    alarm(1);

    /* A fo szoftver egy vegtelen, ertektelen pufferciklusban ragad.
       Nem csinal semmit, csak varja, hogy a konyortelen megszakitas darabolja. */
    while (!system_halt) {
        /* Ide kerulhetne barmilyen mas program, az ISR kettevagná */
    }

    printf("--------------------------------------------------------------\n");
    printf("Rendszer leallt. A 121. karakter utani csend beallt.\n");

    return 0;
}
