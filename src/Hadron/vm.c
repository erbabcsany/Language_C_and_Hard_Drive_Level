//
// Created by ervin on [2026. 03. 09.].
//

#include <stdio.h>
#include <stdlib.h>
#include "vm.h"

#include <string.h> /* A memcpy-hez feltétlenül kell! */

#include "macro.h"

/* ... többi függvényed (vm_push_token, stb) ... */

void vm_tick(HadronVM* vm) {
    /* 1. A JÖVŐ BELESZAKAD A JELENBE:
       A Next szalag teljes tartalmát kőkeményen ráhúzzuk a Current szalagra */
    memcpy(vm->memory_current, vm->memory_next, sizeof(vm->memory_next));

    /* 2. A JÖVŐ KIÜRÍTÉSE:
       A Next szalagot kinullázzuk (0x00), hogy tiszta lappal induljon a következő ciklus */
    memset(vm->memory_next, 0, sizeof(vm->memory_next));

    printf("=== [VM ORAJEL]: TICK. A Jovo Jelenne valt. ===\n");
}

/* =========================================================
   A HADRON VM FIZIKAI IMPLEMENTÁCIÓJA
   ========================================================= */

void vm_init(HadronVM* vm) {
    /* 1. Fázis: A Nyers Void (Üresség) létrehozása */
    vm->absolute_head = 0;
    vm->used_memory = 0;
    vm->system_state = 0; /* 0. Állapot: VOID */
    vm->is_locked = false;

    /* Kinullázzuk a RAM-ot, nehogy memóriaszemét maradjon! */
    memset(vm->memory_arena, 0, sizeof(vm->memory_arena));
    printf("[VM HARDVER]: Vas bekapcsolva. Arena (%d rekesz) kinullazva. Allapot: VOID (0).\n", VM_ARENA_SIZE);
}

void vm_push_token(HadronVM* vm, unsigned char* new_data) {
    /* 1. Kiszámoljuk a fizikai indexet a chipen (Modulo operátor) */
    int physical_index = vm->absolute_head % VM_ARENA_SIZE;

    /* 2. Csak és kizárólag azt az 1 darab 32 bájtos dobozt írjuk felül. Nincs memmove! */
    memcpy(vm->memory_arena[physical_index].data, new_data, 32);

    /* 3. Lépünk a szalagon előre */
    vm->absolute_head++;
    if (vm->used_memory < VM_ARENA_SIZE) {
        vm->used_memory++;
    }
}

void vm_state_transition(HadronVM* vm) {
    /* Létrehozunk egy üres 32 bájtos fizikai dobozt a váltásnak */
    unsigned char transition_payload[32] = {0};

    if (vm->system_state == 0) {
        vm->system_state = 1;
        printf("-> [VM HARDVER]: *** GENEZIS ESEMENY AKTIVALVA ***\n");

        /* A FIZIKA: Beégetjük a dobozba a Genezis OP_CODE-ját (pl. 0xFF) és betoljuk! */
        transition_payload[0] = 0xFF; /* Rendszer szintű esemény kódja */
        transition_payload[1] = 1;    /* Az új dimenzió száma */
        vm_push_token(vm, transition_payload);

    }
    else if (vm->system_state == 1) {
        vm->system_state = 2;
        printf("\n[VM WARNING]: Ujabb atmenet erzekelve! A szabaly felulirva.\n");

        /* A FIZIKA: Betoljuk a Dimenzióváltás lenyomatát a szalagra! */
        transition_payload[0] = 0xFE; /* Mutáció OP_CODE */
        transition_payload[1] = 2;    /* Cél dimenzió */
        vm_push_token(vm, transition_payload);
    }
    else {
        vm->system_state++;
        printf("\n[VM WARNING]: Mutacio! Dimenziovaltas: %d -> %d.\n",
               vm->system_state - 1, vm->system_state);

        /* A FIZIKA: További mutációk betolása */
        transition_payload[0] = 0xFE;
        transition_payload[1] = (unsigned char)vm->system_state;
        vm_push_token(vm, transition_payload);
    }
}

void vm_allocate_entity(HadronVM* vm, const str entity_name) {
    /* Megnézzük, van-e még hely a Homokozóban! */
    if (vm->used_memory >= VM_ARENA_SIZE) {
        printf("\n==================================================\n");
        printf(" [VM KERNEL PANIK]: Memoria-Tulcsordulas a Homokozoban!\n");
        printf(" A Fluxus folyam megprobalta atlepni az %d. rekeszt!\n", VM_ARENA_SIZE);
        printf(" Karanten protokoll aktivalva. Rendszer leallitasa.\n");
        printf("==================================================\n");
        exit(1);
    }

    /* Lefoglalunk 1 memória egységet az arénából */
    const int current_address = vm->used_memory;
    vm->used_memory++;

    printf(" -> [VM HARDVER]: %s lefoglalva a RAM[%d] cimen. (Szabad: %d)\n",
           entity_name, current_address, VM_ARENA_SIZE - vm->used_memory);
}

void vm_dump_memory(HadronVM* vm, int blocks_to_show) {
    printf("\n=== [VM RÖNTGEN: FIZIKAI MEMORIA DUMP] ===\n");

    if (vm->used_memory == 0) {
        printf("A 1024-es szalag teljesen ures. Nincs adat.\n");
        printf("==========================================\n");
        return;
    }

    /* Csak addig megyünk, ameddig van adat, vagy ameddig a limit engedi */
    int limit = (vm->used_memory < blocks_to_show) ? vm->used_memory : blocks_to_show;

    int i;
    for (i = 0; i < limit; i++) {
        printf(" [BLOCK %04d] | ", i);

        /* Mutató a kőkemény 32 bájtos doboz kezdetére a szalagon */
        unsigned char* raw_bytes = (unsigned char*)&vm->memory_arena[i];

        /* Kiíratjuk az első 8 bájtot hexadecimális formában (OP_CODE + Dimenzio + Adat) */
        int b;
        for (b = 0; b < 8; b++) {
            printf("%02X ", raw_bytes[b]);
        }
        printf("... \n");
    }
    printf("==========================================\n\n");
}
