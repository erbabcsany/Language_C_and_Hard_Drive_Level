#include <stdio.h>
#include <string.h>
#include "hadron.h"

#include <stdbool.h>
#include <stdlib.h>

#include "lexicon.h"
#include "macro.h"
#include "vm.h"

/* Létrehozzuk a Globális Virtuális Gépet a C-ben */
HadronVM vm;
bool is_inside_privileged_block = false;

/* =========================================================
   A KÜLSŐ ZSILIP: Fájl beolvasása a RAM-ba (Buffer)
   ========================================================= */
str read_hadron_file(const str filename) {
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

/* A VM Bekapcsolása (Áram alá helyezés) */
void hadron_vm_init(void) {
    vm.system_state = 0;
    vm.used_memory = 0;
    int i;
    for(i=0; i<VM_ARENA_SIZE; i++) vm.memory_arena[i] = 0; /* RAM nullázása */
    printf("[RENDSZER]: Hadron Virtuális Gép (VM) inicializálva. 256 rekesz lefoglalva.\n\n");
}

/* =========================================================
   1. A LEXER (A Rendszer-Kulcsszavakkal bővítve)
   ========================================================= */
void hadron_lexer(const str source, HadronToken* output_token) {
    printf("[LEXER]: Letapogatas: \"%s\"\n", source);

    output_token->symbol = '\0';
    output_token->space_before = 0;
    output_token->space_after = 0;

    /* ÚJ: Kulcsszavak keresése a téridőben! */
    output_token->is_privileged = (strstr(source, "privileged") != NULL) ? 1 : 0;
    output_token->is_hadron_sys = (strstr(source, "hadron") != NULL) ? 1 : 0;

    /* Egy egyszerűsített szimbólum-kereső a bemutatóhoz */
    const str sym_ptr = strchr(source, H_SYM_ASSIGN);
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

void hadron_parser(const HadronToken* token, const str nyers_szoveg) {
    if (nyers_szoveg[0] == ' ' || nyers_szoveg[0] == H_SYM_COMMENT) return;

    /* Dimenzió nyitása */
    if (strchr(nyers_szoveg, H_SYM_ENTITY_DEF) != NULL && strchr(nyers_szoveg, H_SYM_BLOCK_OPEN) != NULL) {
        scope_depth++;
        printf(" -> [HADRON OS]: UJ ENTITAS NYITVA. Jelenlegi Melyseg (Scope): %d\n", scope_depth);
        return;
    }

    /* Dimenzió zárása */
    if (strchr(nyers_szoveg, H_SYM_BLOCK_CLOSE) != NULL) {
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
    if (scope_depth > 0 && strchr(nyers_szoveg, H_SYM_ASSIGN) != NULL) {

        /* ITT JÖN MAJD A VALÓDI FIZIKA: */
        /* vm_write_memory(); */

        return;
    }

    /* =========================================================
       5. AZ ÁLLAPOTÁTMENET (A Genezis Nyila: ->)
       ========================================================= */
    if (scope_depth > 0 && strstr(nyers_szoveg, H_OP_TRANSITION) != NULL) {

        /* ITT JÖN MAJD A VALÓDI FIZIKA: */
        /* Ezt a függvényt fogjuk meghívni, ami fizikailag is
           átlépteti a Rendszert a 0. állapotból az 1. állapotba! */
        /* vm_state_transition(); */

        return;
    }

    /* 4. KVANTUM-LAKAT */
    if (strchr(nyers_szoveg, H_SYM_QUANTUM_LOCK) != NULL) {

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
    hadron_vm_init();

    /* 2. BEOLVASSUK A 'code' FÁJLT A MEREVLEMEZRŐL */
    str source_code = read_hadron_file("code.hadron");

    if (source_code != NULL) {
        printf("\n[RENDSZER]: 'core.hadron' betoltve. Valodi Szkenner (Téridő-követő) AKTIV!\n");
        printf("--------------------------------------------------\n");

        /* A TÉRIDŐ KOORDINÁTÁK */
        int line = 1;
        int col = 1;
        int start_col = 1;

        char buffer[512] = "";
        int b_idx = 0;
        bool is_comment = false;

        /* A Kőkemény Karakter-ciklus a nyers bájtokon */
        int i;
        for (i = 0; source_code[i] != '\0'; i++) {
            const char c = source_code[i];

            /* Soremelés (Új dimenzió Y-tengelyen) */
            if (c == '\n') {
                line++;
                col = 1;
                is_comment = false; /* Új sorban vége a kommentnek */
                continue;
            }

            /* Komment szűrés a Lexikon alapján */
            if (b_idx == 0 && c == H_SYM_COMMENT) {
                is_comment = true;
            }

            if (is_comment) {
                col++;
                continue;
            }

            /* Behúzások (Indentation) ignorálása csak a parancs LEGELEJÉN */
            if (b_idx == 0 && (c == ' ' || c == '\t' || c == '\r')) {
                col++;
                continue;
            }

            /* Rögzítjük a parancs kezdő koordinátáját (X-tengely) */
            if (b_idx == 0) {
                start_col = col;
            }

            /* Karakter betárazása a pufferbe */
            buffer[b_idx++] = c;
            buffer[b_idx] = '\0';
            col++;

            /* VÉGREHAJTÁSI TRIGGER (Amikor a parancs lezárul a Lexikon szerint) */
            if (c == H_SYM_CMD_END || c == H_SYM_BLOCK_OPEN || c == H_SYM_BLOCK_CLOSE) {

                /* =========================================================
                   A KAPSZULA FELTÖLTÉSE (Vége a Memóriaszemétnek!)
                   ========================================================= */
                HadronToken token;

                /* Nullázzuk az eddigi régi értékeket, hogy ne legyen káosz */
                token.space_before = 0;
                token.space_after = 0;
                token.is_privileged = 0;
                token.is_hadron_sys = 0;
                token.symbol = c; /* Eltároljuk magát az írásjelet is! */

                /* BEÉGETJÜK A PONTOS TÉRIDŐT AZ OBJEKTUMBA! */
                token.filepath = "core.hadron";
                token.line_number = line;
                token.column_number = start_col;

                /* BEKÜLDJÜK A PARSERT AZ INICIALIZÁLT TOKENNEL! */
                hadron_parser(&token, buffer);

                /* Kiürítjük a puffert a következő parancshoz */
                b_idx = 0;
                buffer[0] = '\0';
            }
        }

        free(source_code);
        printf("--------------------------------------------------\n");
        printf("[RENDSZER]: Futas befejezve. RAM biztonsagosan felszabaditva.\n");
    }

    return 0;
}