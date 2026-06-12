//
// Created by ervin on [2026. 04. 08.].
//

#ifndef HADRON_WAVE_FUNCTION_H
#define HADRON_WAVE_FUNCTION_H

// A Hadron-rendszer fizikai állandói
#define AMPLITUDE 6.0      // Kezdeti 6-os fal feszültsége (A)
#define DAMPING 0.15       // A 0-pufferek elnyelési együtthatója (lambda)
#define FREQUENCY 0.5      // A kíséret lüktetése (omega)
#define TIME_STEPS 45      // A szekvencia mélysége

/* A struktura, ami lehorgonyozza a fluktuacio parametereit */
typedef struct {
    double amplitude;  /* A kezdeti 'kvantum-tuz' feszultsege (A) */
    double decay;      /* A 0-pufferek elnyelo ereje (lambda) */
    double frequency;  /* A luktetes sebessege (omega) */
    double phase;      /* A fazis-horgony (phi) */
} HadronWave;

double calculate_wave_point(HadronWave *wave, double time_t);

#endif //HADRON_WAVE_FUNCTION_H
