//
// Created by ervin on (2026. 03. 04.).
//

/* =====================================================================
   FLUXUS OS - BARE METAL KERNEL
   FIGYELEM: Ez a kod operacios rendszer nelkul, egyenesen a vason fut!
   ===================================================================== */

#include <stdio.h>
#include "kernel.h"
#include "../Hadron/hadron.h" /* Hadron beemelése a Kernelbe */
#include "../Hadron/vm.h"

#if defined(__linux__) || defined(__APPLE__)
#include <unistd.h>
#define SLEEP_MS(ms) usleep((ms) * 1000)
#elif defined(_WIN32)
#include <windows.h>
#define SLEEP_MS(ms) Sleep(ms)
#else
#define SLEEP_MS(ms)
#endif

/* 1. HARDVERES MEMORIA CIMEK ES MERETEK */
#define VGA_ADDRESS 0xB8000
#define VGA_WIDTH 80
#define VGA_HEIGHT 25

/* SIMULATION DETECTION */
#if defined(__linux__) || defined(__linux) || defined(linux) || defined(__APPLE__) || defined(_WIN32)
#define IS_SIMULATION 1
#else
#define IS_SIMULATION 0
#endif

/* Szinek (Hexadecimalis kódok a videokartyanak) */
#define COLOR_GREEN     0x0A
#define COLOR_RED       0x0C
#define COLOR_WHITE     0x0F
#define COLOR_DARK_GREY 0x08

/* 2. HARDVERES PORT OLVASO (Assembly a C-ben!) */
/* Ezzel hallgatozunk a 0x60-as alaplapi porton a billentyuzet utan. */
unsigned char inb(unsigned short port) {
    unsigned char result = 0;
#if IS_SIMULATION
    /* Szimuláció alatt nem férünk hozzá a valódi portokhoz */
    (void)port;
    return 0;
#else
    __asm__ volatile("inb %1, %0" : "=a"(result) : "Nd"(port));
    return result;
#endif
}

/* 3. A KÖZVETLEN VIDEÓ-MEMÓRIA MUTATÓ */
/* A volatile szo mondja meg a C-nek: "Ne nyulj hozza, ez fizikai drot!" */
#if IS_SIMULATION
unsigned short vga_sim[VGA_WIDTH * VGA_HEIGHT];
volatile unsigned short* vga_buffer = vga_sim;
#else
volatile unsigned short* vga_buffer = (volatile unsigned short*)VGA_ADDRESS;
#endif

/* PIXEL RAJZOLO FUGGVENY (Kiszamolja az 1D memoriat a 2D koordinatabol) */
void draw_particle(int x, int y, char c, unsigned char color) {
    int index;
    if (x >= 0 && x < VGA_WIDTH && y >= 0 && y < VGA_HEIGHT) {
        index = y * VGA_WIDTH + x;
        /* A VGA memoria 16 bites: 8 bit a karakter (ASCII), 8 bit a szin */
        vga_buffer[index] = (unsigned short)c | (unsigned short)(color << 8);
    }
}

/* SZOVEG KIRO FUGGVENY (Mivel nincs printf, nekunk kell megiirni) */
void print_string(int x, int y, const char* text, unsigned char color) {
    int i = 0;
    while (text[i] != '\0') {
        draw_particle(x + i, y, text[i], color);
        i++;
    }
}

/* KEPERNYO TÖRLŐ FUGGVENY */
void clear_screen() {
    int x, y;
    for (y = 0; y < VGA_HEIGHT; y++) {
        for (x = 0; x < VGA_WIDTH; x++) {
            draw_particle(x, y, ' ', COLOR_WHITE);
        }
    }
}

/* =====================================================================
   4. A KERNEL BELÉPÉSI PONTJA (Ezt fogja meghivni a Bootloader)
   ===================================================================== */
extern void kernel_main() {
    unsigned char scan_code;
    volatile int delay; /* Lassito valtozo, hogy a szemunk kovetni tudja */

    /* --- HADRON INTEGRÁCIÓ --- */
    HadronVM vm;
    vm_init(&vm);

    printf("[FLUXUS KERNEL]: Hadron VM inicializalva.\n");

    /* Betöltjük az "Ős-programot" */
    FILE* file = fopen("core.hadron", "r");
    if (file) {
        printf("[FLUXUS KERNEL]: core.hadron betoltese a Jovo szalagra...\n");
        process_hadron_dimension(file, &vm);
        fclose(file);

        /* SZILARDITAS: A Jövő Jelenne válik (Boot Tick) */
        vm_tick(&vm);

        vm_dump_memory(&vm, 32);
    } else {
        printf("[FLUXUS KERNEL PANIK]: core.hadron nem talalhato!\n");
    }

    /* Pálya elokeszitese */
    if (IS_SIMULATION) {
        printf("[VM KERNEL]: Szimulációs mód aktív. Nincs valódi hardver-hozzáférés.\n");
    }
    clear_screen();
    print_string(0, 0, "=== FLUXUS OS + HADRON CORE AKTIV ===", COLOR_GREEN);
    print_string(0, 24, "HADRON INSTRUKCIOK VEGREHAJTASA...", COLOR_WHITE);

    /* A RENDSZER ÉLETCIKLUSA (A Constitution Rule 1 alapján) */
    /* Szekvenciális haladás az Idő Nyilán (nincs spinning while(1) hurok). */
    /* Ha a rendszer várakozik, befagyasztjuk az állapotot, amíg adat nem érkezik. */
    vm_run(&vm);

    if (vm.system_state == VM_STATE_WAITING) {
        printf("\n[FLUXUS KERNEL]: A rendszer VARAKOZIK. Adatbefecskendezesre var...\n");
        printf("[VAS-SZIGOR]: Nyomj ENTER-t a szimulalt megszakitashoz (Interrupt)...\n");
        getchar();
        vm.system_state = VM_STATE_ACTIVE;
        vm_run(&vm); /* Folytatjuk pontosan onnan! */
    }

    vm_dump_memory(&vm, 32); /* THE LOOSE RULE: Ellenorizzuk a torlest! */
    
    printf("[FLUXUS KERNEL]: A rendszer leallt. Halal-hurok vege.\n");
}