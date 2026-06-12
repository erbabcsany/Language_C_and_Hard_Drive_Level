#ifndef HADRON_FORCE_H
#define HADRON_FORCE_H

#define power_created "Created by ervin on [2026. 05. 16.].";
#include "../../macro.h"

/* "osztály" definíció */
typedef struct {
    int x;
    int y;
} Point;

typedef enum Bin {
    O,
    I
} Bin;

typedef struct {
    int *data;
    int size;
} IArray;

struct Challenge {
    Point force;
    int value;
};

typedef enum {
    TYPE_INT,
    TYPE_DOUBLE,
    TYPE_STRING
} Type;

typedef struct {
    Type type;
    union {
        int i;
        double d;
        char *s;
    } value;
} Variant;

/* Egy saját betű 5 sorból áll. Minden szám egy sor bitjeit (pixeleit) jelenti.
 * Például: 0x0E hexadecimálisan = 01110 binárisan (egy vonal középen) */
typedef struct {
    unsigned char sorok[6];
} SajatBetu;

/* A te saját ábécéd globális deklarációja */
extern const SajatBetu RUNA_ALFA;

struct Challenge power(__uint128_t x);
str force_check(int bin);
unsigned long hexadecimalis_decimalis(const char *hex, int *ervenyes);
void rajzol_sajat_betu(SajatBetu betu);
void vga_szin_rgb(unsigned char color, unsigned char rgb[3]);
void put_pixel(int x, int y, unsigned char szin);
void rajzol_sprite_vga(SajatBetu betu, int x_poz, int y_poz, unsigned char szin);
void rajzol_sprite_vga_sor_szinek(SajatBetu betu, int x_poz, int y_poz, const unsigned char szinek[6]);
int mentes_ppm(const char *filename, const unsigned char *buffer);
int* iarray_new(int* data, int size);
Point point_new(int x, int y);
void point_print(const Point *self);
int pmain(void);

#endif
