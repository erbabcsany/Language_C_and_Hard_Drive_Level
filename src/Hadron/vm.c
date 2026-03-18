//
// Created by ervin on [2026. 03. 09.].
//

#include <stdio.h>
#include "vm.h"

#include <string.h> /* A memcpy-hez feltétlenül kell! */

/* AZ AGY: A Bytecode Végrehajtó Motor */
/* AZ AGY: A Valódi, Fizikai Bájtkód Értelmező */
void vm_run(HadronVM* vm) {
    printf("\n=== [VM KERNEL]: VEGREHAJTO MOTOR INDUL ===\n");

    int i;
    for (i = 0; i < vm->used_memory; i++) {
        unsigned char opcode = vm->memory_current[i][0];

        /* 1. TÖRVÉNY: TÚLÉLÉS.
           Alapértelmezésben a Jelen rekesze átmásolódik a Jövő rekeszébe.
           Ha semmi nem változtatja meg, akkor megmarad! */
        memcpy(vm->memory_next[i], vm->memory_current[i], 32);

        /* 2. TÖRVÉNY: MUTÁCIÓ (A Fizikai Elágazás) */
        switch(opcode) {
            case 0xFF:
                printf("[EXEC 0xFF]: GENEZIS a %d. rekeszben. Stabilizalom az entitast.\n", i);
                break;

            case 0xFE:
                printf("[EXEC 0xFE]: MUTACIO a %d. rekeszben! Beavatkozas a Jovobe!\n", i);
                /* FIZIKAI BEAVATKOZÁS: A Mutáció parancs felülírja a KÖVETKEZŐ rekeszt a Jövőben! */
                if (i + 1 < VM_ARENA_SIZE) {
                    /* A következő rekesz első bájtját kőkeményen átírjuk 0x42-re! */
                    vm->memory_next[i + 1][0] = 0x42;
                    printf("   -> A(z) %d. rekesz sorsa megpecsetelve: 0x42 lesz a Tick utan!\n", i + 1);
                }
                break;

            case 0x01:
                printf("[EXEC 0x01]: TOKEN a %d. rekeszben. Varakozas a fuziora.\n", i);
                break;

            // case 0x00:
                // break;

            default:
                break;
        }
    }
    printf("=== [VM KERNEL]: CIKLUS VEGE. ===\n\n");
}

/* 1. BEKAPCSOLÁS: Mindkét dimenziót kinullázzuk */
void vm_init(HadronVM* vm) {
    memset(vm->memory_current, 0, sizeof(vm->memory_current));
    memset(vm->memory_next, 0, sizeof(vm->memory_next));
    vm->used_memory = 0;
    vm->system_state = 0;
    printf("[VM HARDVER]: Vas bekapcsolva. Jelen es Jovo szalagok (1024) tisztaak.\n");
}

/* 2. ADAT BETOLÁSA: A Bootolás (Parser) a Jelenbe pakol */
void vm_push_token(HadronVM* vm, unsigned char* token) {
    if (vm->used_memory >= VM_ARENA_SIZE) {
        printf("[VM KERNEL PANIK]: A futoszalag megtelt!\n");
        return;
    }
    memcpy(vm->memory_current[vm->used_memory], token, 32);
    vm->used_memory++;
}

/* (A vm_state_transition függvényed MARAD a régi, ahogy megírtad!) */

/* 3. A RÖNTGEN: Mostantól a Jelen szalagját vizsgáljuk! */
void vm_dump_memory(HadronVM* vm, int blocks_to_show) {
    printf("\n=== [VM RÖNTGEN: JELEN SZALAG DUMP] ===\n");
    if (vm->used_memory == 0) {
        printf("A szalag ures.\n==========================================\n");
        return;
    }
    int limit = (vm->used_memory < blocks_to_show) ? vm->used_memory : blocks_to_show;
    int i, b;
    for (i = 0; i < limit; i++) {
        printf(" [BLOCK %04d] | ", i);
        unsigned char* raw_bytes = (unsigned char*)&vm->memory_current[i];
        for (b = 0; b < 8; b++) {
            printf("%02X ", raw_bytes[b]);
        }
        printf("... \n");
    }
    printf("==========================================\n\n");
}

/* 4. AZ ÚJ FIZIKA: AZ ÓRAJEL */
void vm_tick(HadronVM* vm) {
    /* A Jövő kőkeményen rázuhan a Jelenre */
    memcpy(vm->memory_current, vm->memory_next, sizeof(vm->memory_next));

    /* A Jövő kiürítése a következő ciklusra */
    memset(vm->memory_next, 0, sizeof(vm->memory_next));

    printf("=== [VM ORAJEL]: TICK. A Jovo Jelenne valt. ===\n");
}
