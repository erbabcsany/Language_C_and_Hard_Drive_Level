//
// Created by ervin on [2026. 03. 09.].
//

#ifndef HADRON_VM_H
#define HADRON_VM_H

#include <stdbool.h>

/* vm.h - A Bare Metal utasításkészlete (Opcode-ok) */

#define VM_TOKEN      0x01  /* Bináris 0000 0001 : A tiszta ige / adatblokk */
#define VM_PRIVILEGED 0x81  /* Bináris 1000 0001 : Token, de a legfelső "Admin" bit (0x80) be van kapcsolva! */
#define VM_HADRON     0x41  /* Bináris 0100 0001 : Token, de a "Struktúra" bit (0x40) be van kapcsolva! */

/* Megjegyzés: A "syntax" ige NINCS ITT!
   Miért? Mert a "syntax" sosem jut le a Vasra! Azt a Parser elnyeli,
   hogy új szabályokat alkosson belőle a memóriában. */

/* =========================================================
   A HADRON VIRTUÁLIS GÉP (VM) HARDVER-DEFINÍCIÓJA
   ========================================================= */

#define VM_ARENA_SIZE 1024  /* A Fluxus Sandbox maximális fizikai mérete */

typedef struct {
    unsigned char data[32];
} RawToken;

typedef struct {
    RawToken memory_arena[VM_ARENA_SIZE]; /* A nyers RAM (A Meder) */

    /* A Kettéhasított Téridő: */
    unsigned char memory_current[VM_ARENA_SIZE][32]; /* A JELEN (Csak Olvasás) */
    unsigned char memory_next[VM_ARENA_SIZE][32];    /* A JÖVŐ (Csak Írás / Mutáció) */

    int used_memory;                 /* Mutató: Mennyi van már lefoglalva? */
    int absolute_head;

    int system_state;                /* A Rendszer állapota (0 = Void, 1 = Spark) */
    bool is_locked;                  /* A Kvantum-Lakat fizikai állapota */
} HadronVM;

void vm_run(HadronVM* vm);

/* ÚJ FIZIKAI TÖRVÉNY: Az Órajel */
void vm_tick(HadronVM* vm);

/* --- A Végrehajtó Rendszerhívások (SysCalls) --- */

/* Inicializálja a Vasat (Bekapcsolás) */
void vm_init(HadronVM* vm);

void vm_push_token(HadronVM* vm, unsigned char* new_data);

/* A Genezis: Átmenet a 0-ból az 1-be */
void vm_state_transition(HadronVM* vm);

/* Memória lefoglalása egy új Entitásnak */
void vm_allocate_entity(HadronVM* vm, const char* entity_name);

/* A fizikai memória nyers hexadecimális kiíratása */
void vm_dump_memory(HadronVM* vm, int blocks_to_show);

#endif