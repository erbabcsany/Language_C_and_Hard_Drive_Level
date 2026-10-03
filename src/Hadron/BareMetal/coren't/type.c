/*
   Created by ervin on [2026. 09. 19.].π
*/

#include "type.h"

#include <stdlib.h>

Type* type_new(const char value) {
    Type* type = malloc(sizeof(Type));
    type->variant.value.c = value;
    type->i++;
    return type;
}

int calc(const char value) {
    Type* type = {0};
    type->variant.value.c = value;
    return 0;
}
