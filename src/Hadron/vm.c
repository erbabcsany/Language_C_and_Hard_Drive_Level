//
// Created by ervin on [2026. 03. 09.].
//

#include <stdio.h>
#include "hadron.h"
#include "vm.h"
#include "../Fluxus#/kernel.h" /* A Hadron rálát a Vasra! */

#include <string.h> /* A memcpy-hez feltétlenül kell! */

#include <stdlib.h>
#include <unistd.h> /* usleep-hez a szimuláció alatt */


/* A Mátrixod váza */

/* A Vas kíméletlen önvédelmi mechanizmusa */
static void sync_future_with_present(HadronVM* vm) {
    int i, b;
    for (i = 0; i < VM_ARENA_SIZE; i++) {
        int is_empty = 1;
        for (b = 0; b < 32; b++) {
            if (vm->memory_next[i][b] != 0) { is_empty = 0; break; }
        }
        if (is_empty) {
            memcpy(vm->memory_next[i], vm->memory_current[i], 32);
        }
    }
}

static int is_sacred_opcode(unsigned char opcode) {
    /* Rule 4: Bizonyos ős-entitások védettek */
    if (opcode == 0x01) return 1; /* TOKEN (Az Ige) */
    if (opcode == 0xFF) return 1; /* GENEZIS */
    if (opcode == 0xEE) return 1; /* SZABALY VEGE (A Pecset) */
    if (opcode == VM_WAIT) return 1; /* WAIT */
    if (opcode == VM_HALT) return 1; /* HALT */
    return 0;
}

void hadron_reset_vm(HadronVM* vm) {
    printf("\n\n[!!! KERNEL PANIC ELHARITVA !!!]\n");
    printf("[VAS-SZIGOR]: Os-Token mutacio eszlelve! A rendszer korrumpalodott.\n");

    /* A fizikai tisztítás: minden adat megy az űrbe */
    memset(vm->memory_current, 0, sizeof(vm->memory_current));
    memset(vm->memory_next, 0, sizeof(vm->memory_next));
    vm->used_memory = 0;
    vm->instruction_pointer = 0; /* AZ IDŐ NYILA VISSZAGÖRDÜL */
    vm->system_state = 0;
    vm->quantum_flag = 1;

    printf("[VAS-SZIGOR]: Rendszer tiszta. Ujrainditas a Nullapontrol.\n");
}


/* AZ AGY: A Bytecode Végrehajtó Motor */
/* AZ AGY: A Valódi, Fizikai Bájtkód Értelmező */
void vm_run(HadronVM* vm) {
    int i;
    /* 0. TÖRVÉNY: AZ IDŐ NYILA (Rule 1)
       Ha az instrukció-mutató túlszalad a használt memórián, a gép várakozik. */
    if (vm->instruction_pointer >= vm->used_memory) {
        if (vm->system_state != VM_STATE_HALTED) {
            vm->system_state = VM_STATE_WAITING;
        }
        return;
    }

    /* Ha a rendszer állapota nem aktív, ne fussunk tovább */
    if (vm->system_state == VM_STATE_WAITING || vm->system_state == VM_STATE_HALTED) {
        return;
    }

    vm->system_state = VM_STATE_ACTIVE;

    sync_future_with_present(vm);

    printf("\n=== [VM KERNEL]: VEGREHAJTO MOTOR INDUL (IP: %d) ===\n", vm->instruction_pointer);

    while (vm->instruction_pointer < vm->used_memory) {
        unsigned char opcode = vm->memory_current[vm->instruction_pointer][0];
        i = vm->instruction_pointer;

        /* 1. TÖRVÉNY: ZERO WASTE (A Kvantum Alagút)
           Ha a kvantum-zászló le van eresztve, az adat kimosódik a jövőből. */
        if (vm->quantum_flag == 0 && opcode != 0x1F && opcode != 0x1C && opcode != 0xEE) {
            printf("[VM KERNEL]: [ZERO WASTE] Rekesz %d elnyelve. Nem marad nyoma a Jovo szalagon.\n", i);
            memset(vm->memory_next[i], 0, 32); /* Fizikai törlés a jövőből */
            vm->instruction_pointer++;
            continue;
        }

        /* 3. TÖRVÉNY: MUTÁCIÓ ÉS LOGIKA */
        switch(opcode) {
            case 0xFF:
                printf("[EXEC 0xFF]: GENEZIS a %d. rekeszben. Stabilizalom az entitast.\n", i);
                break;

            case 0xFE:
                printf("[EXEC 0xFE]: MUTACIO a %d. rekeszben!\n", i);
                if (i + 1 < VM_ARENA_SIZE) {
                    /* VAS-SZIGOR: Szent Axiómák védelme */
                    unsigned char next_op = vm->memory_current[i+1][0];
                    if (is_sacred_opcode(next_op)) {
                        printf("[VAS-SZIGOR]: SZENT AXIOMA (%02X) MUTACIOJA ESZLELVE!\n", next_op);
                        hadron_reset_vm(vm);
                        return;
                    }
                    vm->memory_next[i + 1][0] = 0x42;
                    printf("   -> A(z) %d. rekesz mutalva.\n", i + 1);
                }
                break;

            case 0xEE:
                printf("[EXEC 0xEE]: SZABALY VEGE (;). Kontextus torlese.\n");
                vm->quantum_flag = 1; /* A szabály végén a téridő visszaáll alaphelyzetbe */
                /* Rule 1: Szekvenciális Tick - a Jövő azonnal rázuhan a Jelenre */
                vm_tick(vm);
                sync_future_with_present(vm);
                printf("=== [VM KERNEL]: Orajel letelt. Haladas az Idő Nyilán... ===\n");
                break;

            case 0x01:
                printf("[EXEC 0x01]: TOKEN a %d. rekeszben. Varakozas a fuziora.\n", i);
                break;

            case 0x41:
                printf("[EXEC 0x41]: HADRON-SYSTEM token a %d. rekeszben. Strukturalis elem.\n", i);
                break;

            case 0x81:
                printf("[EXEC 0x81]: PRIVILEGED token a %d. rekeszben. Adminisztracios szint.\n", i);
                break;

            /* ========================================================= */
            /* A PREDICATED EXECUTION (FELTÉTELES VÉGREHAJTÁS) KAPUI     */
            /* ========================================================= */

            case 0x1F:
                /* A Kvantum Zár (?) - Feltétel kiértékelése */
                printf("[VM KERNEL]: [EXEC 0x1F] -> Kvantum Zar (?).\n");
                /* Ha az első argumentum 0, a zár lecsukódik */
                if (vm->memory_current[i][1] == 0) {
                    vm->quantum_flag = 0;
                    printf("   -> [ALLAPOT]: ZAR LEZARVA (FALSE). Semmi nem jut at.\n");
                } else {
                    vm->quantum_flag = 1;
                    printf("   -> [ALLAPOT]: ZAR NYITVA (TRUE). Az energia szabad.\n");
                }
                break;

            case 0x1C:
                /* A Kvantum Kapu (:) - Az alternatív útvonal */
                printf("[VM KERNEL]: [EXEC 0x1C] -> Kvantum Kapu (:).\n");
                vm->quantum_flag = !vm->quantum_flag;
                printf("   -> [ALLAPOT]: KAPU ATVALTVA. Mostantol: %s\n", vm->quantum_flag ? "NYITVA" : "ZARVA");
                break;


            /* ========================================================= */
            /* A HADRON-FLUXUS HARDVERES ÖSSZEKÖTÉS (THE BRIDGE)        */
            /* ========================================================= */
            case VM_VGA_DRAW:
                printf("[VM KERNEL]: [EXEC 0xD1] -> VGA RAJZOLAS. (X: %d, Y: %d, Char: '%c')\n",
                       vm->memory_current[i][1], vm->memory_current[i][2], vm->memory_current[i][3]);
                draw_particle(vm->memory_current[i][1], vm->memory_current[i][2],
                             vm->memory_current[i][3], vm->memory_current[i][4]);
                break;

            case VM_VGA_CLEAR:
                printf("[VM KERNEL]: [EXEC 0xC1] -> KEPERNYO TORLESE.\n");
                clear_screen();
                break;

            case VM_WAIT:
                printf("[VM KERNEL]: [EXEC 0x05] -> RENDSZER VARAKOZIK (WAIT).\n");
                vm->system_state = VM_STATE_WAITING;
                /* Rule 1: A gép befagyasztja az állapotát. Simulation mode: 1s fagyasztás */
                #if defined(__linux__) || defined(__APPLE__)
                    usleep(1000 * 1000); 
                #endif
                vm_tick(vm);
                sync_future_with_present(vm);
                vm->system_state = VM_STATE_ACTIVE;
                break;

            case VM_HALT:
                printf("[VM KERNEL]: [EXEC 0x06] -> RENDSZER LEALLITVA (HALT).\n");
                vm->system_state = VM_STATE_HALTED;
                vm->instruction_pointer++;
                return;

            case VM_SYNTAX:
                /* THE LOOSE RULE: Dinamikus Szintaxis (Rule 3)
                   Felismerjük az új szabályt az IP+1 rekeszben! */
                printf("[VM KERNEL]: [EXEC 0x55] -> DINAMIKUS SZINTAXIS.\n");
                if (vm->instruction_pointer + 1 < vm->used_memory) {
                    unsigned char next_op = vm->memory_current[vm->instruction_pointer + 1][0];
                    if (next_op == 0x02) { /* 0x02 is Name/Payload */
                        const char* new_sym = (const char*)&vm->memory_current[vm->instruction_pointer + 1][1];
                        unsigned char target_op = vm->memory_current[vm->instruction_pointer][1];
                        
                        if (registered_rules < 4096) {
                            strncpy(symbol_table[registered_rules].symbol, new_sym, 15);
                            symbol_table[registered_rules].semantic_action = target_op;
                            registered_rules++;
                            printf("   -> SZABALY FELVEVE: '%s' = 0x%02X. A nyelvtant mutaltuk!\n", 
                                   new_sym, target_op);
                        }
                    }
                }
                break;

            default:
                break;
        }
        /* THE LOOSE RULE: AZ ADAT ELENGEDÉSE
           Amint egy utasítás végrehajtódott, elengedjük (töröljük) a jövőből,
           hogy ne maradjon szemét a Vason (Zero Waste). */
        memset(vm->memory_next[vm->instruction_pointer], 0, 32);

        vm->instruction_pointer++;
    }
    printf("=== [VM KERNEL]: CIKLUS VEGE (VEGE A SZALAGNAK). ===\n\n");
}

/* 1. BEKAPCSOLÁS: Mindkét dimenziót kinullázzuk */
void vm_init(HadronVM* vm) {
    memset(vm->memory_current, 0, sizeof(vm->memory_current));
    memset(vm->memory_next, 0, sizeof(vm->memory_next));
    vm->used_memory = 0;
    vm->system_state = 0;
    printf("[VM HARDVER]: Vas bekapcsolva. Jelen es Jovo szalagok (1024) tisztaak.\n");
    vm->instruction_pointer = 0;

    /* Alapértelmezett állapot: A Jövő nyitva áll */
    vm->quantum_flag = 1;
}

/* 2. ADAT BETOLÁSA: A Jövőbe (Future) pakolunk! */
void vm_push_token(HadronVM* vm, unsigned char* token) {
    if (vm->used_memory >= VM_ARENA_SIZE) {
        printf("[VM KERNEL PANIK]: A futoszalag megtelt!\n");
        hadron_reset_vm(vm); /* Rule 6: Reset Protocol */
        return;
    }
    /* Rule 2: A Lexer a Jövőbe (memory_next) ír, amit a Tick szilárdít meg. */
    memcpy(vm->memory_next[vm->used_memory], token, 32);
    vm->used_memory++;
}

/* 3. A HIVATALOS GATE: A Kernel (Vas) ezen keresztül beszél a VM-mel */
void write_to_hadron(HadronVM* vm, unsigned char* data, int size) {
    /* Rule 5: Vas-Szigor (Nincs csendes hiba) */
    if (vm->used_memory >= VM_ARENA_SIZE) {
        printf("[VAS-SZIGOR]: Kernel Panic! RAM megtelt (%d). Reset.\n", VM_ARENA_SIZE);
        hadron_reset_vm(vm);
        return;
    }

    /* Adat befecskendezése a Jövőbe */
    int bytes_to_copy = (size > 32) ? 32 : size;
    memcpy(vm->memory_next[vm->used_memory], data, bytes_to_copy);
    
    printf("[VM GATE]: Adat befecskendezve a %d. rekeszbe (Future). Size: %d\n", 
           vm->used_memory, bytes_to_copy);
    
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
