//
// Created by ervin on [2026. 03. 17.].
//

/* keywords.h - A Különszervezett Szótár Definiálása */

#include "hadron.h" /* Ide jön a HadronKeyword enum */
#include "vm.h"     /* Ide jönnek a VM_TOKEN, VM_PRIVILEGED opcode-ok */

/* A Szótár egyetlen bejegyzése (A Kapszula) */
typedef struct {
    const char* word;          /* A nyers szöveg (pl. "token") */
    HadronKeyword lex_enum;    /* A Parser nyelve (pl. KW_TOKEN) */
    unsigned char vm_opcode;   /* A Vas nyelve (pl. 0x01) */
} KeywordDefinition;