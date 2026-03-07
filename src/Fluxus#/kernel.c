//
// Created by ervin on (2026. 03. 04.).
//

/* =====================================================================
   FLUXUS OS - BARE METAL KERNEL
   FIGYELEM: Ez a kod operacios rendszer nelkul, egyenesen a vason fut!
   ===================================================================== */

/* 1. HARDVERES MEMORIA CIMEK ES MERETEK */
#define VGA_ADDRESS 0xB8000
#define VGA_WIDTH 80
#define VGA_HEIGHT 25

/* Szinek (Hexadecimalis kódok a videokartyanak) */
#define COLOR_GREEN     0x0A
#define COLOR_RED       0x0C
#define COLOR_WHITE     0x0F
#define COLOR_DARK_GREY 0x08

/* 2. HARDVERES PORT OLVASO (Assembly a C-ben!) */
/* Ezzel hallgatozunk a 0x60-as alaplapi porton a billentyuzet utan. */
unsigned char inb(unsigned short port) {
    unsigned char result;
    __asm__ volatile("inb %1, %0" : "=a"(result) : "Nd"(port));
    return result;
}

/* 3. A KÖZVETLEN VIDEÓ-MEMÓRIA MUTATÓ */
/* A volatile szo mondja meg a C-nek: "Ne nyulj hozza, ez fizikai drot!" */
volatile unsigned short* vga_buffer = (volatile unsigned short*)VGA_ADDRESS;

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
void print_string(int x, int y, const char* str, unsigned char color) {
    int i = 0;
    while (str[i] != '\0') {
        draw_particle(x + i, y, str[i], color);
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
    int particle_x = 40;
    int particle_y = 12;
    volatile int delay; /* Lassito valtozo, hogy a szemunk kovetni tudja */

    /* Pálya elokeszitese */
    clear_screen();
    print_string(0, 0, "=== FLUXUS OS : BARE METAL KERNEL AKTIV ===", COLOR_GREEN);
    print_string(0, 24, "NYOMD MEG AZ 'I' BETUT AZ INJEKTALASHOZ!", COLOR_WHITE);

    /* A HALÁL-HUROK (A fizikai motor vegtelen ciklusa) */
    while (1) {

        /* Figyeljuk a billentyuzetet (I betu scan kódja: 0x17) */
        scan_code = inb(0x60);

        if (scan_code == 0x17) {

            /* 1. Eltuntetjuk a reszecsket a regi helyerol (urzzuk a memoriat) */
            draw_particle(particle_x, particle_y, ' ', COLOR_WHITE);

            /* 2. Leptetjuk a fizikat jobbra */
            particle_x++;
            if (particle_x >= VGA_WIDTH) {
                particle_x = 0; /* Ha kimegy a kepernyorol, visszajon bal oldalt */
            }

            /* 3. Kirajzoljuk az uj helyere (Piros O) */
            draw_particle(particle_x, particle_y, 'O', COLOR_RED);

            /* 4. Hardveres kesleltetes (Mivel a processzor tul gyors lenne) */
            for (delay = 0; delay < 500000; delay++) {
                /* Ures ciklus, csak egeti az orajelet, hogy lassuljon a mozgás */
            }
        }
    }
}