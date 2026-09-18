/*
   Created by ervin on [2026. 09. 19.].
*/

#ifndef HADRON_TYPE_H
#define HADRON_TYPE_H
#include <stddef.h>

typedef struct {
    size_t size;
    union {
        int* i;
        double* d;
        float* f;
        char* s;
    } value;
} Array;

typedef enum {
    TYPE_INT,
    TYPE_DOUBLE,
    TYPE_FLOAT,
    TYPE_CHARACTER,
    TYPE_ARRAY
} DefinedType;

typedef struct {
    DefinedType type;
    union {
        int i;
        double d;
        float f;
        char c;
        Array a;
    } value;
} Variant;

typedef struct {
    Variant variant;
    int i;
} Type;

Type* type_new(char);
int calc(char);

#endif