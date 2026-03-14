//
// Created by ervin on [2026. 03. 09.].
//

#include <stdio.h>
#include <stdlib.h>
#include "vm.h"

#include "macro.h"

/* =========================================================
   A HADRON VM FIZIKAI IMPLEMENTÁCIÓJA
   ========================================================= */

void vm_init(HadronVM* vm) {
    /* 1. Fázis: A Nyers Void (Üresség) létrehozása */
    vm->used_memory = 0;
    vm->system_state = 0; /* 0. Állapot: VOID */
    vm->is_locked = false;

    /* Kinullázzuk a RAM-ot, nehogy memóriaszemét maradjon! */
    int i;
    for (i = 0; i < VM_ARENA_SIZE; i++) {
        vm->memory_arena[i] = 0;
    }

    printf("[VM HARDVER]: Vas bekapcsolva. Arena (%d rekesz) kinullazva. Allapot: VOID (0).\n", VM_ARENA_SIZE);
}

void vm_state_transition(HadronVM* vm) {
    /* A Dimenzióváltás: 0 -> 1 */
    if (vm->system_state == 0) {
        vm->system_state = 1;
        printf(" -> [VM HARDVER]: *** GENEZIS ESEMENY AKTIVALVA ***\n");
        printf(" -> [VM HARDVER]: Dimenziovaltas: VOID (0) -> SPARK (1).\n");
        printf(" -> [VM HARDVER]: A Fluxus folyo elindult.\n");
    } else {
        /* Ha már 1-es állapotban van, és valaki újra próbálja indítani: */
        printf("\n[VM KERNEL PANIK]: Illegalis Dimenziovaltas! A Genezis csak egyszer tortenhet meg!\n");
        exit(1);
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

void vm_write_memory(HadronVM* vm, int value) {
    /* Csak akkor írunk, ha van már lefoglalt rekesz (0-nál nagyobb used_memory) */
    if (vm->used_memory > 0) {
        int target_address = vm->used_memory - 1; /* A legutolsó aktív rekesz */
        vm->memory_arena[target_address] = value;

        printf(" -> [VM HARDVER]: Adat-beiras: RAM[%d] = %d.\n", target_address, value);
    } else {
        printf("\n[VM KERNEL PANIK]: Megkiserelt iras lefoglalatlan memoriaba!\n");
        exit(1);
    }
}
