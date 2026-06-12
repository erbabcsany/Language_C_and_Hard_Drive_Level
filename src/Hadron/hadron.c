#include <stdio.h>
#include <string.h>
#include "hadron.h"

#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>

#include "lexicon.h"
#include "macro.h"
#include "vm.h"
#include "keywords.h"

#define STAGING_SIZE 4096

/* A Zsilip (Staging Arena). Ide zúdul be a kód a külvilágból. */
char staging_arena[STAGING_SIZE];

/* Itt foglaljuk le a tényleges, fizikai memóriát az 1%-os Radarunknak! */
DynamicRule symbol_table[4096];
int registered_rules = 0;

/* EZT TÖRÖLTED KI VÉLETLENÜL: A Dimenzióváltás fizikája */
void vm_state_transition(HadronVM* vm) {
    unsigned char transition_payload[32] = {0};

    if (vm->system_state == 0) {
        vm->system_state = 1;
        printf("-> [VM HARDVER]: *** GENEZIS ESEMENY AKTIVALVA ***\n");
        transition_payload[0] = 0xFF; /* Genezis OP_CODE */
        transition_payload[1] = 1;
        write_to_hadron(vm, transition_payload, 32);
    }
    else if (vm->system_state == 1) {
        vm->system_state = 2;
        printf("\n[VM WARNING]: Ujabb atmenet erzekelve! A szabaly felulirva.\n");
        transition_payload[0] = 0xFE; /* Mutáció OP_CODE */
        transition_payload[1] = 2;
        write_to_hadron(vm, transition_payload, 32);
    }
    else {
        vm->system_state++;
        printf("\n[VM WARNING]: Mutacio! Dimenziovaltas: %d -> %d.\n",
               vm->system_state - 1, vm->system_state);
        transition_payload[0] = 0xFE;
        transition_payload[1] = (unsigned char)vm->system_state;
        write_to_hadron(vm, transition_payload, 32);
    }
}

/* ========================================================================= */
/* UNIVERZÁLIS ZSEB ÜRÍTÉS: KULCSSZÓ VAGY PAYLOAD ELBÍRÁLÁS                  */
/* ========================================================================= */
void flush_buffer_to_tape(HadronVM* vm, char* buffer, int* buf_idx, int* arg_idx) {
    if (*buf_idx == 0) return; /* Üres zsebnél nincs dolgunk */

    buffer[*buf_idx] = '\0'; /* Szó lezárása */

    const KeywordDefinition* kw = lookup_keyword(buffer);

    /* 1. VIZSGÁLAT: Ez egy ismert KULCSSZÓ (Ős-ige)? */
    if (kw != NULL) {
        unsigned char payload[32] = {0};
        payload[0] = kw->vm_opcode;
        printf("[LEXER]: 0x%02X (Kulcsszo) felismerve: %s\n", payload[0], kw->word);
        write_to_hadron(vm, payload, 32);
        *buf_idx = 0;
        *arg_idx = 1; /* A következő szám az 1. indexre kerül */
        return;
    }
    /* 2. VIZSGÁLAT: Szám-e? (Argumentum befecskendezés) */
    bool is_num = true;
    int k;
    for (k = 0; buffer[k]; k++) {
        if (!isdigit(buffer[k]) && buffer[k] != '-') { is_num = false; break; }
    }

    if (is_num && vm->used_memory > 0) {
        unsigned char val = (unsigned char)atoi(buffer);
        int last = vm->used_memory - 1;

        if (*arg_idx < 32) {
            /* Rule 2: Az adat is a Jövőbe (memory_next) kerül! */
            vm->memory_next[last][*arg_idx] = val;
            printf("[LEXER]: Adat (%d) hozzaadva az utolso tokenhez (Op: 0x%02X) az %d. indexen.\n",
                   val, vm->memory_next[last][0], *arg_idx);
            (*arg_idx)++;
            *buf_idx = 0;
            return;
        }
    }

    /* 3. VIZSGÁLAT: Ha nem kulcsszó, akkor az kőkeményen egy NÉV/ADAT (Payload)! */
    unsigned char name_payload[32] = {0};
    name_payload[0] = 0x02; /* 0x02 OP_CODE */
    strncpy((char*)&name_payload[1], buffer, 31);
    printf("[LEXER]: 0x02 (Nev-Payload) befecskendezve: '%s'\n", buffer);
    write_to_hadron(vm, name_payload, 32);
    *arg_idx = 1;

    *buf_idx = 0; /* Zseb kinullázása */
}

/* ========================================================================= */
/* A HADRON SZEME: KARAKTERENKÉNTI ÁLLAPOTGÉP (VAS-SZIGOR AKTIVÁLVA)         */
/* ========================================================================= */
void process_hadron_dimension(FILE* file, HadronVM* vm) {
    char buffer[256]; /* A Zsebünk (Puffer) */
    int buf_idx = 0;
    int arg_idx = 1; /* Argumentum index követése */
    int ch; /* Az éppen beolvasott kőkemény bájt */

    printf("\n[LEXER]: Karakter-Porszivo inditasa. Vas-szigor aktiv.\n");

    /* Végtelen porszívó: amíg nincs vége a fájlnak (EOF) */
    while ((ch = fgetc(file)) != EOF) {

        /* 1. SZABÁLY: SZÓKÖZÖK ÉS ENTEREK (A Zseb ürítése) */
        if (ch == ' ' || ch == '\n' || ch == '\t' || ch == '\r') {
            flush_buffer_to_tape(vm, buffer, &buf_idx, &arg_idx);
            continue; /* Ugrás a következő karakterre! */
        }

        /* 2. SZABÁLY: AZ IRÁNYÍTÓPULTOK (A Kvantum Zár beépítve!) */
        if (ch == '[' || ch == ']' || ch == ':' || ch == ';' || ch == '{' || ch == '}' || ch == '?') {
            /* Ha volt valami a zsebben, azt gyorsan lementjük Payloadként! */
            flush_buffer_to_tape(vm, buffer, &buf_idx, &arg_idx);

            /* A fizikai operátor letétele a szalagra */
            unsigned char payload[32] = {0};
            switch(ch) {
                case '[': payload[0] = 0x1A; printf("[LEXER]: 0x1A (Tomb Nyitas)\n"); break;
                case ']': payload[0] = 0x1B; printf("[LEXER]: 0x1B (Tomb Zaras)\n"); break;
                case ':': payload[0] = 0x1C; printf("[LEXER]: 0x1C (Kettospont / Kapu)\n"); break;
                case ';': payload[0] = 0xEE; printf("[LEXER]: 0xEE (Szabaly Vege)\n"); break;
                case '{': payload[0] = 0x1D; printf("[LEXER]: 0x1D (Blokk Nyitas)\n"); break;
                case '}': payload[0] = 0x1E; printf("[LEXER]: 0x1E (Blokk Zaras)\n"); break;
                case '?': payload[0] = 0x1F; printf("[LEXER]: 0x1F (Kvantum Zar / Feltetel)\n"); break; /* <-- AZ ÚJ DIMENZIÓ */
                default: ; /* A Szellem Utasítás, amit tegnap megbeszéltünk */
            }
            write_to_hadron(vm, payload, 32);
            arg_idx = 1; /* Reset arg index for operators too */
            continue;
        }

        /* 3. SZABÁLY: ÖSSZETETT NYILAK (->) */
        if (ch == '-') {
            /* Előrenézünk 1 bájtot! */
            int next_ch = fgetc(file); 
            
            if (next_ch == '>') {
                flush_buffer_to_tape(vm, buffer, &buf_idx, &arg_idx);
                unsigned char payload[32] = {0}; payload[0] = 0xFE; /* Mutáció/Nyíl */
                write_to_hadron(vm, payload, 32);
                arg_idx = 1;
            } else {
                /* Ha nem nyíl, akkor visszatesszük a karaktert a gépbe és betűként kezeljük */
                ungetc(next_ch, file);
                if (buf_idx < 255) buffer[buf_idx++] = (char)ch;
            }
            continue;
        }

        /* 4. SZABÁLY: NORMÁL ÉPÍTKEZÉS (Betűk gyűjtése a zsebbe) */
        if (buf_idx < 255) {
            buffer[buf_idx++] = (char)ch;
        }
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

/* =========================================================
   A FŐÁRAMKÖR (hadron_main V5.0 - Kvantum-Reaktor Élesítve)
   ========================================================= */
/* Ez a te fő függvényed a hadron.c-ben, amit a main.c hív meg! */
void hadron_main() {
    printf("[HADRON OS]: Rendszer inditasa...\n");
    HadronVM hadron_vm;
    vm_init(&hadron_vm);

    /* 1. FAZIS: A Szem (Lexer) beolvassa a fizikai fájlt a Jelen szalagjára */
    FILE* file = fopen("core.hadron", "r");
    if (file) {
        process_hadron_dimension(file, &hadron_vm);
        fclose(file);
    }

    /* Megnezzuk a szalagot (Röntgen) */
    vm_dump_memory(&hadron_vm, 32);

    /* 2. FAZIS: AZ AGY (Parser/VM) rászabadul a szalagra, és értelmezi a jeleket! */
    vm_run(&hadron_vm);

    /* 3. FAZIS: Az Órajel (Tick). Itt válik a Jövő Jelenné. */
    vm_tick(&hadron_vm);
}