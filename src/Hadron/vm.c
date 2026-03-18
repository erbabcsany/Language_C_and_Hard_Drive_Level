//
// Created by ervin on [2026. 03. 09.].
//

#include <stdio.h>
#include <stdlib.h>
#include "vm.h"

#include <string.h>

#include "macro.h"

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
    /* A Dimenzióváltás: 0 -> 1 */
    if (vm->system_state == 0) {
        vm->system_state = 1;
        printf("-> [VM HARDVER]: *** GENEZIS ESEMENY AKTIVALVA ***\n");
        printf("-> [VM HARDVER]: Dimenziovaltas: VOID (0) -> SPARK (1).\n");
        printf("-> [VM HARDVER]: A Fluxus folyo elindult.\n");
    }
    else if (vm->system_state == 1) {
        /* AZ 1% LOGIKÁJA: Nincs Kernel Pánik! A Gép alkalmazkodik és szintet lép! */
        vm->system_state = 2;
        printf("\n[VM WARNING]: Ujabb atmenet erzekelve! A szabaly felulirva.\n");
        printf("-> [VM HARDVER]: Dimenziovaltas: SPARK (1) -> FLUXUS (2).\n");
    }
    else {
        /* Bármilyen további átmenet csak csúsztatja a dimenziót, de a gép SOSEM áll le. */
        vm->system_state++;
        printf("\n[VM WARNING]: Mutacio! Dimenziovaltas: %d -> %d.\n",
               vm->system_state - 1, vm->system_state);
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
