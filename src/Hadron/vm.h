//
// Created by ervin on [2026. 03. 09.].
//

#ifndef HADRON_VM_H
#define HADRON_VM_H

#include <stdbool.h>

/* =========================================================
   A HADRON VIRTUÁLIS GÉP (VM) HARDVER-DEFINÍCIÓJA
   ========================================================= */

#define VM_ARENA_SIZE 1024  /* A Fluxus Sandbox maximális fizikai mérete */

typedef struct {
    int memory_arena[VM_ARENA_SIZE]; /* A nyers RAM (A Meder) */
    int used_memory;                 /* Mutató: Mennyi van már lefoglalva? */

    int system_state;                /* A Rendszer állapota (0 = Void, 1 = Spark) */
    bool is_locked;                  /* A Kvantum-Lakat fizikai állapota */
} HadronVM;

/* --- A Végrehajtó Rendszerhívások (SysCalls) --- */

/* Inicializálja a Vasat (Bekapcsolás) */
void vm_init(HadronVM* vm);

/* A Genezis: Átmenet a 0-ból az 1-be */
void vm_state_transition(HadronVM* vm);

/* Memória lefoglalása egy új Entitásnak */
void vm_allocate_entity(HadronVM* vm, const char* entity_name);

/* Érték beírása az aktuális (legutóbb lefoglalt) memóriarekeszbe */
void vm_write_memory(HadronVM* vm, int value);

#endif