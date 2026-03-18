#include <stdio.h>
#include <string.h>
#include "hadron.h"

#include <stdbool.h>
#include <stdlib.h>

#include "keywords.h"
#include "lexicon.h"
#include "macro.h"
#include "vm.h"

#define STAGING_SIZE 4096

/* A Zsilip (Staging Arena). Ide zúdul be a kód a külvilágból. */
char staging_arena[STAGING_SIZE];

/* Itt foglaljuk le a tényleges, fizikai memóriát az 1%-os Radarunknak! */
DynamicRule symbol_table[4096];
int registered_rules = 0;

void process_hadron_dimension(FILE* source_file, HadronVM* vm) {
    /* 1. BEÖMLÉS (Ugyanaz marad) */
    size_t flooded_bytes = fread(staging_arena, 1, STAGING_SIZE, source_file);
    if (flooded_bytes == 0) return;

    char buffer[STAGING_SIZE + 1];
    strncpy(buffer, staging_arena, flooded_bytes);
    buffer[flooded_bytes] = '\0';

    /* 2. A FINOMHANGOLT SZEM: MOSTANTÓL CSAK A SZÓKÖZT ÉS ÚJSORT VÁGJUK LE!
       Így a '->' és a ';' önálló szavakként (tokenként) is láthatóak maradnak! */
    char* current_word = strtok(buffer, " \n\t\r");

    while (current_word != NULL) {

        /* A) Megkérdezzük a Szótárat (Ős-igék) */
        const KeywordDefinition* kw_def = lookup_keyword(current_word);

        if (kw_def != NULL) {
            /* Ős-ige! Betoljuk a szalagra! */
            if (kw_def->lex_enum == KW_TOKEN || kw_def->lex_enum == KW_PRIVILEGED || kw_def->lex_enum == KW_HADRON) {
                unsigned char payload[32] = {0};
                payload[0] = kw_def->vm_opcode;
                vm_push_token(vm, payload);
            }
        }
        /* B) BEOLVASZTOTT RÉGI MOTOR: Kőkemény Topológia felismerés! */
        else if (strcmp(current_word, "->") == 0) {
            /* Megtaláltuk az Átmenet jelét! Meghívjuk a Dimenzióváltást! */
            vm_state_transition(vm);
        }
        else if (strcmp(current_word, ";") == 0) {
            /* Később ide jön a szabály-lezárás logikája */
        }
        else {
            /* Minden más (pl. a változóid nevei: 'state_void', 'ignition_event')
               egyelőre csak csendben átfolyik a rendszeren. */
        }

        /* Ugrás a következő szóra */
        current_word = strtok(NULL, " \n\t\r");
    }
}

/* Létrehozzuk a Globális Virtuális Gépet a C-ben */
HadronVM vm;
bool is_inside_privileged_block = false;

/* =========================================================
   A KÜLSŐ ZSILIP: Fájl beolvasása a RAM-ba (Buffer)
   ========================================================= */
str read_hadron_file(cstr filename) {
    /* 1. Kőkemény fizikai hozzáférés a merevlemezhez (Olvasás / Read Binary mód) */
    FILE* file = fopen(filename, "rb");
    if (file == NULL) {
        printf("[RENDSZER HIBA]: Fomernok ur, nem talalom a '%s' nevu fajlt a meghajton!\n", filename);
        return NULL;
    }

    /* 2. Megmérjük a fájl méretét a Téridőben */
    fseek(file, 0, SEEK_END);
    const long filesize = ftell(file);
    fseek(file, 0, SEEK_SET);

    /* 3. Lefoglaljuk a nyers RAM-ot a szilikonban (A memóriaszivárgásra itt nagyon vigyázni kell!) */
    str buffer = (str)malloc(filesize + 1);
    if (buffer == NULL) {
        printf("[RENDSZER HIBA]: Nincs eleg RAM a fajl betoltesehez!\n");
        fclose(file);
        return NULL;
    }

    /* 4. Átszivattyúzzuk az adatot, majd lezárjuk a fájlt */
    fread(buffer, 1, filesize, file);
    buffer[filesize] = '\0'; /* Kőkemény string lezáró jel */

    fclose(file);
    return buffer;
}

/* =========================================================
   1. A LEXER (A Rendszer-Kulcsszavakkal bővítve)
   ========================================================= */
void hadron_lexer(cstr source, Token* output_token) {
    printf("[LEXER]: Letapogatas: \"%s\"\n", source);

    output_token->symbol = '\0';
    output_token->space_before = 0;
    output_token->space_after = 0;

    /* ÚJ: Kulcsszavak keresése a téridőben! */
    output_token->is_privileged = (strstr(source, "privileged") != NULL) ? 1 : 0;
    output_token->is_hadron_sys = (strstr(source, "hadron") != NULL) ? 1 : 0;

    /* Egy egyszerűsített szimbólum-kereső a bemutatóhoz */
    cstr sym_ptr = strchr(source, H_SYM_ASSIGN);
    if (sym_ptr != NULL) {
        output_token->symbol = H_SYM_ASSIGN;

        /* Határellenőrzött Téridő-mérés */
        if (sym_ptr > source) {
            output_token->space_before = *(sym_ptr - 1) == ' ' ? 1 : 0;
        }
        if (strlen(sym_ptr) > 1) {
            output_token->space_after = *(sym_ptr + 1) == ' ' ? 1 : 0;
        }
    }
}

/* Részlet a javított Hadron Virtuális Gépből */

bool is_sacred_axiom(const str target_name) {
    if (strcmp(target_name, "null") == 0) return true;
    if (strcmp(target_name, "true") == 0) return true;
    if (strcmp(target_name, "false") == 0) return true;
    /* Ide jöhetnek a számok és más alap-entitások */
    return false;
}

/* =========================================================
   2. A PARSER (Az Értelmező és Végrehajtó)
   ========================================================= */
/* A Hadron Parser Dimenzió-követője (Scope Tracking) */
int scope_depth = 0;

void hadron_parser(HadronVM* hadron_vm, const Token* token) {
    /* Dimenzió nyitása */
    if (token->type == TOKEN_ENTITY_DEF || token->type == TOKEN_ENTITY_OPEN) {
        scope_depth++;
        printf(" -> [HADRON OS]: UJ ENTITAS NYITVA. Jelenlegi Melyseg (Scope): %d\n", scope_depth);
        vm_allocate_entity(hadron_vm, "Uj Entitas");
        return;
    }

    /* Dimenzió zárása */
    if (token->type == TOKEN_ENTITY_CLOSE) {
        scope_depth--;

        /* A KŐKEMÉNY FIZIKAI VÉDELEM a hadron_parser-en belül */
        if (scope_depth < 0) {
            /* ÍME A KÉRT PROFESSZIONÁLIS HIBAÜZENET A TOKENBŐL: */
            printf("\n%s:%d:%d: [FATAL ERROR] Zarojel-tulcsordulas!\n",
                   token->filepath, token->line_number, token->column_number);
            printf("Vegzetes hiba: Tobb a befejezo '}' mint a nyito '{'.\n");
            exit(1);
        }

        /* ITT JÖN MAJD A VALÓDI FIZIKA: */
        /* vm_close_entity(); */

        return;
    }

    /* 3. ÉRTÉKADÁS / MUTÁCIÓ (A régi "=" jel) */
    if (scope_depth > 0 && token->type == TOKEN_ASSIGN) {

        /* AZ ÚJ FIZIKA: Létrehozzuk az üres 32 bájtos dobozt (Payload) */
        unsigned char payload[32] = {0};

        /* A számértéket beletesszük a doboz elejébe.
           (Később a 0. indexre jön a kőkemény OP_CODE, így az adatot az 1. indexre tesszük) */
        payload[1] = (unsigned char)token->numeric_value;

        /* Rátoljuk a dobozt a Vas 1024-es Egyirányú Szalagjára (Single Flow Buffer) */
        vm_push_token(hadron_vm, payload);

        return;
    }

    /* =========================================================
       5. AZ ÁLLAPOTÁTMENET (A Genezis Nyila: ->)
       ========================================================= */
    if (scope_depth > 0 && token->type == TOKEN_TRANSITION) {

        /* ITT JÖN MAJD A VALÓDI FIZIKA: */
        /* Ezt a függvényt fogjuk meghívni, ami fizikailag is
           átlépteti a Rendszert a 0. állapotból az 1. állapotba! */
        vm_state_transition(hadron_vm);

        return;
    }

    /* 4. KVANTUM-LAKAT */
    if (token->type == TOKEN_QUANTUM_LOCK) {

        /* ITT JÖN MAJD A VALÓDI FIZIKA: */
        /* vm_lock_memory(); */

        return;
    }
}

/* =========================================================
   A FŐÁRAMKÖR (hadron_main V5.0 - Kvantum-Reaktor Élesítve)
   ========================================================= */
/* Ez a te fő függvényed a hadron.c-ben, amit a main.c hív meg! */
int hadron_main() {
    printf("[HADRON OS]: Rendszer inditasa...\n");

    HadronVM hadron_vm;
    vm_init(&hadron_vm);

    FILE* file = fopen("core.hadron", "r");
    if (file) {
        /* EGYETLEN MOTOR, EGYETLEN OLVASÁS! Nincs rewind, nincs régi while ciklus! */
        process_hadron_dimension(file, &hadron_vm);
        fclose(file);
    } else {
        printf("[HIBA]: A core.hadron nem talalhato!\n");
    }

    /* Röntgen a legvégén, hogy lássuk a fizikai szalagot */
    vm_dump_memory(&hadron_vm, 50);

    return 0;
}