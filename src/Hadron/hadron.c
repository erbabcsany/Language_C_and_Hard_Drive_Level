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
    /* 1. BEÖMLÉS: A nyers kód bezúdul a memóriába */
    size_t flooded_bytes = fread(staging_arena, 1, STAGING_SIZE, source_file);
    if (flooded_bytes == 0) return; /* Üres az univerzum */

    /* Átmásoljuk egy biztonságos pufferbe, és lezárjuk, hogy a strtok ne fusson túl a memórián */
    char buffer[STAGING_SIZE + 1];
    strncpy(buffer, staging_arena, flooded_bytes);
    buffer[flooded_bytes] = '\0';

    /* 2. A SZEM AKTIVÁLÁSA: Felvágjuk a szöveget.
       A fura karaktersor a végén a SZIKÉK listája: szóközök, újsorok és mindenféle írásjelek! */
    char* current_word = strtok(buffer, " \n\t\r;!(){}[]:.,");

    /* Pásztázás a memóriablokk végéig */
    while (current_word != NULL) {

        /* 3. A FÓKUSZ: Megkérdezzük a Szótárat (Az Agyat) */
        const KeywordDefinition* kw_def = lookup_keyword(current_word);

        if (kw_def != NULL) {
            /* ŐS-IGE AZONOSÍTVA! Rátoljuk a Vasra! */
            if (kw_def->lex_enum == KW_TOKEN || kw_def->lex_enum == KW_PRIVILEGED || kw_def->lex_enum == KW_HADRON) {
                unsigned char payload[32] = {0};
                payload[0] = kw_def->vm_opcode; /* A kőkemény OP_CODE beégetése */
                vm_push_token(vm, payload);
            }
        }

        /* A Szem ugrik a következő szóra */
        current_word = strtok(NULL, " \n\t\r;!(){}[]:.,");
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
        printf("[RENDSZER]: RAM biztonsagosan felszabaditva.\n");

        /* Kérünk egy röntgenfelvételt az első 5 használt memóriablokkról */
        /* Így kell kinéznie a main.c-nek: Tiszta fizika. */
        FILE* file = fopen("core.hadron", "r");
        if (file != NULL) {
            /* 1. Ráborítjuk az egészet az új motorra EGYETLEN LÉPÉSBEN */
            process_hadron_dimension(file, &core_vm);
            fclose(file);
        } else {
            printf("[HIBA]: Nem talalom a core.hadron fajlt!\n");
        }

        /* 2. Röntgen a futás legvégén (50 blokkig, hogy mindent lássunk) */
        vm_dump_memory(&core_vm, 50);

        printf("[RENDSZER]: Futas befejezve.\n");
    }

    return 0;
}