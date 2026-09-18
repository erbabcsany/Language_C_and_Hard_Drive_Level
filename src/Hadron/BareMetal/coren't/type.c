/*
   Created by ervin on [2026. 09. 19.].π
*/

#include "type.h"

#include <stdlib.h>

Type* type_new(const char value) {
    Type* type = malloc(sizeof(Type));
    type->value = value;
    type->history[0] = value;
    return type;
}

int calc(const char value) {
    Type* type = {0};
    type->value = value;
    return 0;
}
