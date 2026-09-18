#include "power.h"

#include <stdio.h>
#include <math.h>

/* Moduláris hatványozás: (base^exp) % mod */
/* C90-kompatibilis megvalósítás double segítségével a túlcsordulás ellen */
long mod_pow(long base, long exp, long mod) {
    long res = 1;
    long b = base % mod;

    while (exp > 0) {
        if (exp % 2 == 1) {
            res = (long)fmod((double)res * b, (double)mod);
        }
        b = (long)fmod((double)b * b, (double)mod);
        exp /= 2;
    }
    return res;
}

/* A kifejezés két szummájának kiszámítása egy adott j értékre */
double sum_j(long d, int j) {
    double sum = 0.0;
    long k;

    /* 1. rész: k = 0-tól d-1-ig (Moduláris rész) */
    for (k = 0; k < d; ++k) {
        long denominator = 8 * k + j;
        long numerator = mod_pow(16, d - 1 - k, denominator);
        sum += (double)numerator / denominator;
        sum = sum - (long)sum; /* Csak a törtrész megtartása */
    }

    /* 2. rész: k = d-től a végtelenig (A sor "farka", ahol 16^(d-1-k) lecseng) */
    /* A gyakorlatban ~15-20 tag bőven elegendő a double pontosságához */
    for (k = d; k < d + 20; ++k) {
        long denominator = 8 * k + j;
        double numerator = pow(16.0, (double)(d - 1 - k));
        sum += numerator / denominator;
        sum = sum - (long)sum;
    }

    return sum;
}

char* pi16(void) {
    long d = 1000; /* A keresett pozíció (pl. az 1000. hexadecimális jegy) */
    double s1, s4, s5, s6, pi_fraction;
    int digit;
    char* hex_digit[32];

    /* A BBP képlet 4 részösszegének kiszámítása */
    s1 = sum_j(d, 1);
    s4 = sum_j(d, 4);
    s5 = sum_j(d, 5);
    s6 = sum_j(d, 6);

    /* A végső törtrész összeállítása a pi = 4*S1 - 2*S4 - S5 - S6 képlet alapján */
    pi_fraction = 4.0 * s1 - 2.0 * s4 - s5 - s6;
    pi_fraction = pi_fraction - (long)pi_fraction;
    if (pi_fraction < 0) {
        pi_fraction += 1.0;
    }

    /* A d-edik pozíción álló jegy kinyerése hexadecimális formában */
    digit = (int)(pi_fraction * 16.0);

    printf("A PI %ld. hexadecimalis jegye: %X\n", d, digit);

    sprintf(hex_digit[d], "A PI %ld. hexadecimalis jegye: %X\n", d, digit);
    return hex_digit[d];
}
