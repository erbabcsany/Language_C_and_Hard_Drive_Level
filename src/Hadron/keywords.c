//
// Created by ervin on [2026. 03. 17.].
//

/* keywords.c - A Szótár Fizikai Teste */

#include "keywords.h"
#include <string.h>

/* A kőkemény, különszervezett szótárunk. Bármikor bővíthető egy sorral,
   anélkül, hogy a Lexer kódjához (a motorhoz) hozzá kellene nyúlni. */
static const KeywordDefinition keyword_dictionary[] = {
    {"token",      KW_TOKEN,      VM_TOKEN},
    {"privileged", KW_PRIVILEGED, VM_PRIVILEGED},
    {"hadron",     KW_HADRON,     VM_HADRON},

    /* A syntax meta-ige. Ennek nincs VM opcode-ja, mert sosem jut el a Vasig.
       Ezért a Vas szintjén adunk neki egy 0x00 (Üres) értéket. */
    {"syntax",     KW_SYNTAX,     0x00}
};

/* Kiszámoljuk, hány elem van a szótárban, hogy a gép tudja, meddig keressen */
#define DICTIONARY_SIZE (sizeof(keyword_dictionary) / sizeof(KeywordDefinition))

/* Ugyanitt a keywords.c-ben */

/* Ez a függvény adja vissza a TELJES kapszulát, így a Parser
   azonnal megkapja a Lexer Enum-ot és a VM Bájtot is! */
const KeywordDefinition* lookup_keyword(const char* word) {
    int i;
    for (i = 0; i < DICTIONARY_SIZE; i++) {
        if (strcmp(word, keyword_dictionary[i].word) == 0) {
            return &keyword_dictionary[i]; /* Megvan a találat! Mutatót adunk vissza. */
        }
    }
    return NULL; /* Nem kulcsszó. Ez egy topológiai jel vagy név lesz. */
}
