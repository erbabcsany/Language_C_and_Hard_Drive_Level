#include <stdio.h>
#include <string.h>
#include "hadron.h"

#include <stdbool.h>
#include <stdlib.h>

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
    /* 1. BEÖMLÉS: Egyetlen mozdulattal ráborítjuk a kódot a memóriára. Nincs ciklus. */
    size_t flooded_bytes = fread(staging_arena, 1, STAGING_SIZE, source_file);

    if (flooded_bytes == 0) return; /* Üres az univerzum */

    /* 2. A RADAR (Mutatók a Téridőben) */
    char* current_anchor = staging_arena;              /* Hol vagyunk most? */
    char* end_of_space = staging_arena + flooded_bytes; /* Hol a Zsilip vége? */

    /* Itt nem karaktereket olvasunk! A strstr (String Search) egy
       hardveresen optimalizált radar-ugrás a memóriában. */
    char* next_token_pos = strstr(current_anchor, "token");

    /* Ha talált egy pillért (token-t), a C-motor tudja, hogy a 'current_anchor'
       és a 'next_token_pos' KÖZÖTTI távolság maga a kőkemény TOPOLÓGIA (A Szabály)! */

    if (next_token_pos != NULL) {
        /*
          Itt történik az 1% mágiája:
          Kiszámoljuk az űrt a két Ige között.
          Ezt az űrt (pl. a " \n_ " jeleket) belevágjuk a Hash-egyenletbe,
          létrehozzuk a 64 bites utasítást, és átküldjük a Vasra (vm_push_token).
        */

        /* ... A topológiai zúzás helye ... */

        /* Ugrás a következő horgonyra (átugorjuk magát a "token" szót) */
        current_anchor = next_token_pos + 5;
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
int hadron_main(void) {
    printf("=== HADRON V5.0: FIZIKAI VIRTUALIS GEP (VM) AKTIV ===\n\n");

    /* 1. BEKAPCSOLJUK A FIZIKAI MEMÓRIÁT (RAM) */
    HadronVM core_vm;
    vm_init(&core_vm);

    /* 2. BEOLVASSUK A 'code' FÁJLT A MEREVLEMEZRŐL */
    cstr source_code = read_hadron_file("core.hadron");

    if (source_code != NULL) {
        printf("\n[RENDSZER]: 'core.hadron' betoltve. Valodi Szkenner (Téridő-követő) AKTIV!\n");
        printf("--------------------------------------------------\n");

        /* A TÉRIDŐ KOORDINÁTÁK */
        int current_line = 1;
        int current_column = 1;
        // int start_col = 1;
        //
        // char buffer[512] = "";
        // int b_idx = 0;
        // bool is_comment = false;

        /* A Kőkemény Karakter-ciklus a nyers bájtokon */
        int i;
        /* Részlet a fő olvasó ciklusból (ahol a source_code-ot járod be): */
        for (i = 0; source_code[i] != '\0'; i++) {
            cchr c = source_code[i];
            cchr next_c = source_code[i+1]; /* Előretekintünk 1 karaktert! */

            Token token;
            token.filepath = "core.hadron";
            token.line_number = current_line;
            token.column_number = current_column;
            token.symbol = c;
            token.type = TOKEN_UNKNOWN;

            /* A KATEGORIZÁLÁS (A Kohó): */
            if (c == ' ') {
                token.type = TOKEN_WHITESPACE_SPACE;
            } else if (c == '\t') {
                token.type = TOKEN_WHITESPACE_TAB;
            } else if (c == '\n') {
                token.type = TOKEN_WHITESPACE_NEWLINE;
                current_line++;
                current_column = 0; /* Új sor, oszlop nullázva */
            } else if (c == '{') {
                token.type = TOKEN_ENTITY_OPEN;
            } else if (c == '}') {
                token.type = TOKEN_ENTITY_CLOSE;
            } else if (c == '=') {
                token.type = TOKEN_ASSIGN;
            } else if (c == '!') {
                token.type = TOKEN_QUANTUM_LOCK;
            }
            /* A KŐKEMÉNY GENEZIS-NYÍL FELISMERÉSE (2 karakter!): */
            else if (c == '-' && next_c == '>') {
                token.type = TOKEN_TRANSITION;
                i++; /* Átugorjuk a '>' karaktert, hogy ne olvassuk be kétszer! */
                current_column++;
            }

            /* Ha felismertük, beküldjük a Parsernek és a Vasnak! */
            if (token.type != TOKEN_UNKNOWN) {
                hadron_parser(&core_vm, &token);
            }

            current_column++;
        }

        free((void*)source_code);
        printf("--------------------------------------------------\n");
        printf("[RENDSZER]: Futas befejezve. RAM biztonsagosan felszabaditva.\n");
    }

    return 0;
}