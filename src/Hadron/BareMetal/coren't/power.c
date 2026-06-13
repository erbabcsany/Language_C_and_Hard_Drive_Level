#include "power.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../macro.h"

#define VGA_WIDTH 320
#define VGA_HEIGHT 200

unsigned char vga_buffer[VGA_WIDTH * VGA_HEIGHT];
unsigned char *vga_memoria = vga_buffer;

const SajatBetu RUNA_ALFA = {
    { 0x0E,  /* ###  */
      0x11,  /* #   # */
      0x1F,  /* ##### */
      0x11,  /* #   # */
      0x11,  /* #   # */
      0x19 } /* ##    */
};

static int hex_karakter_ertek(char karakter)
{
    if (karakter >= '0' && karakter <= '9') {
        return karakter - '0';
    }

    if (karakter >= 'A' && karakter <= 'F') {
        return karakter - 'A' + 10;
    }

    if (karakter >= 'a' && karakter <= 'f') {
        return karakter - 'a' + 10;
    }

    return -1;
}

unsigned long hexadecimalis_decimalis(const char *hex, int *ervenyes)
{
    unsigned long eredmeny;
    int i;
    int szamjegy;
    int volt_szamjegy;

    eredmeny = 0;
    i = 0;
    volt_szamjegy = 0;

    if (ervenyes != NULL) {
        *ervenyes = 0;
    }

    if (hex == NULL) {
        return 0;
    }

    if (hex[0] == '#') {
        i = 1;
    } else if (hex[0] == '0' && (hex[1] == 'x' || hex[1] == 'X')) {
        i = 2;
    }

    while (hex[i] != '\0') {
        szamjegy = hex_karakter_ertek(hex[i]);

        if (szamjegy < 0) {
            return 0;
        }

        eredmeny = (eredmeny * 16UL) + (unsigned long)szamjegy;
        volt_szamjegy = 1;
        i++;
    }

    if (ervenyes != NULL && volt_szamjegy) {
        *ervenyes = 1;
    }

    return eredmeny;
}

void vga_szin_rgb(unsigned char color, unsigned char rgb[3])
{
    switch (color) {
    case 0x00:
        rgb[0] = 0x00; rgb[1] = 0x00; rgb[2] = 0x00;
        break;
    case 0x01:
        rgb[0] = 0x00; rgb[1] = 0x00; rgb[2] = 0xAA;
        break;
    case 0x02:
        rgb[0] = 0x00; rgb[1] = 0xAA; rgb[2] = 0x00;
        break;
    case 0x04:
        rgb[0] = 0xAA; rgb[1] = 0x00; rgb[2] = 0x00;
        break;
    case 0x0E:
        rgb[0] = 0xFF; rgb[1] = 0xFF; rgb[2] = 0x55;
        break;
    case 0x0F:
        rgb[0] = 0xFF; rgb[1] = 0xFF; rgb[2] = 0xFF;
        break;
    case 0x28:
        rgb[0] = 0x5A; rgb[1] = 0x00; rgb[2] = 0x00;
        break;
    case 0x29:
        rgb[0] = 0x50; rgb[1] = 0xB4; rgb[2] = 0xFF;
        break;
    default:
        rgb[0] = color; rgb[1] = color; rgb[2] = color;
        break;
    }
}

int mentes_ppm(const char *filename, const unsigned char *buffer)
{
    FILE *file;
    int i;
    unsigned char color;
    unsigned char rgb[3];

    file = fopen(filename, "wb");
    if (file == NULL) {
        return 0;
    }

    fprintf(file, "P6\n%d %d\n255\n", VGA_WIDTH, VGA_HEIGHT);

    for (i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++) {
        color = buffer[i];

        vga_szin_rgb(color, rgb);

        fwrite(rgb, 1, 3, file);
    }

    fclose(file);
    return 1;
}

void put_pixel(const int x, const int y, unsigned char szin)
{
    int pixel_index;

    if (x < 0 || x >= VGA_WIDTH) {
        return;
    }

    if (y < 0 || y >= VGA_HEIGHT) {
        return;
    }

    pixel_index = (y * VGA_WIDTH) + x;
    vga_memoria[pixel_index] = szin;
}

void print_all(const int count, ...) {
    __gnuc_va_list args;
    int i;

    __builtin_va_start(args, count);

    for (i = 0; i < count; i++) {
        const int val = __builtin_va_arg(args, int);
        printf("%d\n", val);
    }

    __builtin_va_end(args);
}

/* "konstruktor" függvény */
Point point_new(const int x, const int y) {
    Point p;
    p.x = x;
    p.y = y;
    return p;
}

int* iarray_new(int* data, const int size) {
    Point point;
    IArray array;
    str result = "";
    point = point_new(size, 0);
    array.data = data;
    array.size = size;
    return array.data;
}

int is_non_zero(const int x)
{
    int* array[1024]; // Ez a mutatók tömbje
    int status[1024]; // Ez az ÚJ tömb a 0 és 1 értékeknek
    str messages[1024]; // Nem nézőknek való
    int i;
    int j = 0;
    str result = "ehhez nincs hozzáférésem";
    for (i = 0; i < 1024; i++) {
        if (array[i] == NULL) {
            if (scanf("%d", &status[i]) != 1) {
                status[i] = '\0';
                printf("nincs érték megadva");
            } else {
                status[i] = 0;
                printf("a kapcsolat megszakadt");
            }
        } else {
            messages[j] = *array[i] == 10 ? result : "1";
            printf("a megmaradt érték: %d", *array[i]);
        }
        if (messages[j] == result) messages[i] = "itt nem szabad járni";
        if (array[i] == &i) messages[i] = "ha megtagadod a parancsomat, én leszek a házigazda";
        array[i] = NULL;
        j++;
    }
    return x != 0;
}



/**
 * Generates a string based on the provided integer input.
 *
 * The function iterates from 0 to the value of `bin`, updating a string `result` with
 * characters '1' or '0' based on the value of `bin`. If `bin` is positive, it assigns '1'
 * to each position in the string until completion; otherwise, it assigns '0'. In the end,
 * if `bin` is not zero, the function returns the string "1". Otherwise, the resulting
 * string is returned.
 *
 * Note: This function contains logical errors and may exhibit undefined behavior. For example,
 * `result` is incorrectly indexed and modified without being properly initialized with a
 * predefined size, leading to possible memory access violations. Additionally, the use of
 * the `else` block with `i--` conflicts with the main loop increment, potentially causing
 * infinite or erroneous behavior.
 *
 * @param bin An integer input value influencing the output string and loop behavior.
 * @return A string containing either "1", or a sequence of '1' or '0' based on the logic in the function.
 */
str force_check(const int bin) {
    str result = "";
    int i = 0;
    for (; i < bin; ++i)
    {
        if (bin > 0)
        {
            result[i] = '1';
            i++;
        }
        else
        {
            result[i] = '0';
            i--;
        }
    }
    if (bin != 0) {
        return "1";
    }
    return result;
}

int force_point_new(const int x, const int y)
{
    const int _x = is_non_zero(x);
    const int _y = is_non_zero(y);
    if (_x == 1 || _y == 1)
    {
        return 1;
    }
    return 0;
}

/* "metódus" */
void point_print(const Point *self) {
    printf("(%d, %d)\n", self->x, self->y);
}

/* Függvény, ami kirendereli a saját karakteredet a konzolra */
void rajzol_sajat_betu(SajatBetu betu) {
    int i, j;
    unsigned char maszk;

    /* Végigmegyünk a karakter 5 során */
    for (i = 0; i < 5; i++) {
        /* Végigmegyünk a sor 5 oszlopán (bitjén) balról jobbra */
        for (j = 4; j >= 0; j--) {
            maszk = 1 << j; /* Kijelöljük az aktuális bitet */

            if (betu.sorok[i] & maszk) {
                printf("[]"); /* Ha a bit 1-es, "tintát" nyomunk */
            } else {
                printf("  "); /* Ha a bit 0, üres helyet hagyunk */
            }
        }
        printf("\n"); /* Sorvége */
    }
}

void rajzol_sprite_vga(SajatBetu betu, int x_poz, int y_poz, unsigned char szin) {
    int sor, oszlop;
    unsigned char aktualis_sor;
    unsigned char maszk;

    for (sor = 0; sor < 6; sor++) {
        aktualis_sor = betu.sorok[sor];

        for (oszlop = 4; oszlop >= 0; oszlop--) {
            maszk = 1 << oszlop;

            if (aktualis_sor & maszk) {
                put_pixel(x_poz + (4 - oszlop), y_poz + sor, szin);
            }
        }
    }
}

void rajzol_sprite_vga_sor_szinek(SajatBetu betu, int x_poz, int y_poz, const unsigned char szinek[6])
{
    int sor, oszlop;
    unsigned char aktualis_sor;
    unsigned char maszk;

    for (sor = 0; sor < 6; sor++) {
        aktualis_sor = betu.sorok[sor];

        for (oszlop = 4; oszlop >= 0; oszlop--) {
            maszk = 1 << oszlop;

            if (aktualis_sor & maszk) {
                put_pixel(x_poz + (4 - oszlop), y_poz + sor, szinek[sor]);
            }
        }
    }
}

int pmain(void) {
    int a[] = {10, 36, 78, 41};
    const int size = ARRAY_SIZE(a);
    SajatBetu kodolt_uzenet[1];
    const Point p = point_new(3, 5);
    int hex_ok;
    unsigned long fa01af_decimalis;
    const int bin = 0;
    const unsigned char alfa_szinek[6] = {
        0x0F,
        0x28,
        0x29,
        0x02,
        0x04,
        0x0E
    };

    fa01af_decimalis = hexadecimalis_decimalis("#FA01AF", &hex_ok);

    kodolt_uzenet[0] = RUNA_ALFA;
    point_print(&p);
    printf("%d\n", size);
    print_all(5, 49, 2, 3, 4, 5);
    if (hex_ok) {
        printf("#FA01AF decimalisan: %lu\n", fa01af_decimalis);
    }

    rajzol_sajat_betu(kodolt_uzenet[0]);

    put_pixel(10, 10, 0);
    put_pixel(10 + 1, 10, 1);

    rajzol_sprite_vga(RUNA_ALFA, 0, 0, 1);
    rajzol_sprite_vga_sor_szinek(RUNA_ALFA, 50, 20, alfa_szinek);

    if (!mentes_ppm("kep.ppm", vga_memoria)) {
        printf("Nem sikerult menteni a kepet.\n");
        return 1;
    }

    printf("o%s", force_check(bin));

    return 0;
}
