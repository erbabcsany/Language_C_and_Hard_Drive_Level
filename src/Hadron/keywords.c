//
// Created by ervin on [2026. 03. 17.].
//

/* keywords.c - A Szótár Fizikai Teste */

#include "keywords.h"

#include <stdio.h>
#include <string.h>

/* A kőkemény, különszervezett szótárunk. Bármikor bővíthető egy sorral,
   anélkül, hogy a Lexer kódjához (a motorhoz) hozzá kellene nyúlni. */
static const KeywordDefinition keyword_dictionary[] = {
    {"token",      KW_TOKEN,      VM_TOKEN},
    {"privileged", KW_PRIVILEGED, VM_PRIVILEGED},
    {"hadron",     KW_HADRON,     VM_HADRON},

    /* A syntax meta-ige. (The Loose Rule) */
    {"syntax",     KW_SYNTAX,     VM_SYNTAX},

    /* A HADRON-FLUXUS HARDVERES ÖSSZEKÖTÉSE */
    {"draw",       KW_DRAW,       VM_VGA_DRAW},
    {"clear",      KW_CLEAR,      VM_VGA_CLEAR},

    /* SYSTEM FLOW CONTROLS */
    {"wait",       KW_WAIT,       VM_WAIT},
    {"halt",       KW_HALT,       VM_HALT}
};

/* Kiszámoljuk, hány elem van a szótárban, hogy a gép tudja, meddig keressen */
#define DICTIONARY_SIZE (sizeof(keyword_dictionary) / sizeof(KeywordDefinition))

/* Ugyanitt a keywords.c-ben */

/* Ez a függvény adja vissza a TELJES kapszulát, így a Parser
   azonnal megkapja a Lexer Enum-ot és a VM Bájtot is! */
const KeywordDefinition* lookup_keyword(const char* word) {
    int i;
    static KeywordDefinition dynamic_match;

    /* ELŐSZÖR: Megnézzük a Dinamikus Szimbólumtáblát (The Loose Rule) */
    for (i = 0; i < registered_rules; i++) {
        if (strcmp(word, symbol_table[i].symbol) == 0) {
            dynamic_match.word = symbol_table[i].symbol;
            dynamic_match.lex_enum = KW_NONE; /* Dinamikus elemeknél ez másodlagos */
            dynamic_match.vm_opcode = (unsigned char)symbol_table[i].semantic_action;
            return &dynamic_match;
        }
    }

    /* MÁSODSZOR: Megnézzük a Statikus Szótárt */
    for (i = 0; i < DICTIONARY_SIZE; i++) {
        if (strcmp(word, keyword_dictionary[i].word) == 0) {
            return &keyword_dictionary[i]; /* Megvan a találat! Mutatót adunk vissza. */
        }
    }
    return NULL; /* Nem kulcsszó. Ez egy topológiai jel vagy név lesz. */
}
