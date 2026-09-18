/*
   Created by ervin on [2026. 09. 26.].
*/

#include "push.h"

#include <string.h> /* Szükséges az Amy-féle memmove függvényhez */

static Variant TÖMB[MAX_SIZE] = {0};
static int INDEX = 0;

int push(const Variant value) {
   AMY_PERFECT_PUSH(TÖMB, INDEX, value);
   return 0;
}
