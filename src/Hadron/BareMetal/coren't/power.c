/*
 * Created by ervin on [2026. máj. 16. 20∶29∶1778956152].
 */

#include "power.h"
#include "calcπ.h"

#include <stdio.h>
#include <string.h>

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdarg.h>
#include <unistd.h>

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
    va_list args;
    int i;

    va_start(args, count);

    for (i = 0; i < count; i++) {
        const int val = va_arg(args, int);
        printf("%d\n", val);
    }

    va_end(args);
}

/* "konstruktor" függvény */
Point point_new(const int x, const int y) {
    Point p;
    p.x = x;
    p.y = y;
    return p;
}

int* iarray_new(int* data) {
    const int size = sizeof(data);
    IArray array;
    array.data = data;
    array.size = size;
    return array.data;
}

int searchInFile(const char* filename, const char* name) {
    int fd;
    struct stat sb;
    char* file_in_memory;
    int position = EOF;
    size_t name_len;
    char* p;
    char* end;

    if (filename == NULL || name == NULL) return EOF;

    fd = open(filename, O_RDONLY);
    if (fd == -1) return EOF;

    /* Lekérjük a fájl pontos méretét */
    if (fstat(fd, &sb) == -1 || sb.st_size == 0) {
        close(fd);
        return EOF;
    }

    /* Összekötjük a fájlt a memóriával */
    file_in_memory = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (file_in_memory == MAP_FAILED) {
        close(fd);
        return EOF;
    }

    /* Kiszámoljuk a keresett szó hosszát */
    name_len = strlen(name);

    /* Ha üres a keresett szó, vagy hosszabb, mint a fájl, nincs értelme keresni */
    if (name_len == 0 || name_len > (size_t)sb.st_size) {
        munmap(file_in_memory, sb.st_size);
        close(fd);
        return EOF;
    }

    p = file_in_memory;
    /* Csak addig mehetünk, amíg a hátralévő hely elég a keresett szónak */
    end = file_in_memory + sb.st_size - name_len;

    while (p <= end) {
        /* A memchr villámgyorsan megkeresi az első egyező karaktert */
        p = memchr(p, name[0], (size_t)(end - p + 1));
        if (!p) break; /* Nincs több ilyen kezdőbetű, vége */

        /* Ha megvan a kezdőbetű, a memcmp ellenőrzi a teljes szót */
        if (memcmp(p, name, name_len) == 0) {
            position = (int)(p - file_in_memory);
            break; /* Megtaláltuk! */
        }
        p++; /* Tovább lépünk, ha fals riasztás volt */
    }

    munmap(file_in_memory, sb.st_size);
    close(fd);

    return position;
}

int is_non_zero(const int x)
{
    int* array[1024]; /* Ez a mutatók tömbje */
    Variant variants[1024]; /* Ez az ÚJ tömb a 0 és 1 értékeknek */
    str messages[1024]; /* Nem nézőknek való */
    int i;
    int j = 0;
    str result = "ehhez nincs hozzáférésem";
    for (i = 0; i < 1024; i++) {
        if (array[i] == NULL) {
            if (scanf("%d", &variants[i].value.i) != 1) {
                variants[i].type = TYPE_CHARACTER;
                variants[i].value.c = '\0';
                printf("nincs érték megadva");
            } else {
                variants[i].type = TYPE_INT;
                variants[i].value.i = 0;
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
        if (i %5 == 0) {
            messages[j] = "ha itt jársz, akkor a legnagyobb veszélyben vagy";
            variants[j].type = TYPE_DOUBLE;
            variants[j].value.d = (j * 0.09765625);
        }
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
 * @return A string containing either "1" or a sequence of '1' or '0' based on the logic in the function.
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

static double sonic_calc_pi(int iterations) {
    double rest = 0.0;
    double sign = 4.0;

    int j;
    for (j = 0; j < iterations; j++) {
        rest += sign / (j * 2.0 + 1.0);
        sign = -sign;
    }
    return rest;
}

#include <math.h>

static double tails_calc_pi(int steps) {
    double pi = 0.0;
    int k;
    for (k = 0; k < steps; k++) {
        double term = (1.0 / pow(16.0, k)) * (
            4.0 / (8.0 * k + 1.0) -
            2.0 / (8.0 * k + 4.0) -
            1.0 / (8.0 * k + 5.0) -
            1.0 / (8.0 * k + 6.0)
        );
        pi += term;
    }
    return pi;
}

static double shadow_ultimate_pi() {
    const double C = 426880;
    const double L = 13591409;
    const double X = 1;
    const double M = 1;

    const double sum = M * L / X;
    const double sqrtK = sqrt(10005);

    return C * sqrtK / sum;
}

#define PLACES 1000000
#define SIZE (PLACES + 5)

int pi[SIZE], term[SIZE], temp[SIZE];

/* Globális tömbök a memóriatúlcsordulás megelőzésére és a C90 kompatibilitás miatt. */
int pi[SIZE], term[SIZE], temp[SIZE];

/* OKOSABB OSZTÁS: Egy fixpontos tömb leosztása egy sima egész számmal.
   Kezeli a 9-nél nagyobb kiinduló értékeket is a tömb elején! */
void arr_div(int *arr, int divisor) {
    long carry = 0;
    int i;
    for (i = 0; i < SIZE; i++) {
        long cur = arr[i] + carry * 10;

        /* Ha az első (i=0) elem nagyobb mint 9, a cur/divisor egész része
           itt keletkezik, a maradék pedig szabályosan csorog tovább. */
        arr[i] = (int)(cur / divisor);
        carry = cur % divisor;
    }
}

/* Két fixpontos tömb összeadása vagy kivonása (ha sub != 0, akkor kivonás). */
void arr_op(int *dest, const int *src, int sub) {
    int carry = 0, i;
    for (i = SIZE - 1; i >= 0; i--) {
        int val = dest[i] + (sub ? -src[i] : src[i]) + carry;
        if (val < 0) { val += 10; carry = -1; }
        else if (val >= 10) { val -= 10; carry = 1; }
        else { carry = 0; }
        dest[i] = val;
    }
}

/* Az arctan(1/x) kiszámítása Taylor-sorral, a szorzó (mult) azonnali alkalmazásával. */
void add_arctan(int x, int mult, int sub) {
    int k = 1, sign = 0, i;

    /* A term nullázása. */
    for (i = 0; i < SIZE; i++) term[i] = 0;

    /* KOMPAKT MEGOLDÁS: a szorzót egyszerűen beírjuk a 0. indexre... */
    term[0] = mult;

    /* ...és a módosított arr_div egyetlen lépésben elvégzi a mult / x osztást! */
    arr_div(term, x);

    while (1) {
        /* Megnézzük, hogy a term üres-e (elértük-e a pontossági határt). */
        for (i = 0; i < SIZE && term[i] == 0; i++);
        if (i == SIZE) break;

        /* Megőrizzük a term-et, majd leosztjuk: temp = term / (2k - 1). */
        for (i = 0; i < SIZE; i++) temp[i] = term[i];
        arr_div(temp, 2 * k - 1);

        /* Hozzáadjuk vagy kivonjuk a pi-ből az aktuális előjeltől függően. */
        arr_op(pi, temp, sign ? !sub : sub);

        /* Következő tag előkészítése: term = term / x^2. */
        arr_div(term, x);
        arr_div(term, x);

        sign = !sign;
        k++;
    }
}

int pmain(void) {
    int i;
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

    printf("%d\n", searchInFile("core.hadron", "token"));
    printf("o%s", force_check(bin));

    printf("Sonic kódja (5 lépés):   %.15f\n", sonic_calc_pi(5));
    printf("Tails kódja (5 lépés):   %.15f\n", tails_calc_pi(5));
    printf("Shadow kódja (1 lépés):   %.15f\n", shadow_ultimate_pi());

    /* A tömbök alaphelyzetbe állítása. */
    for (i = 0; i < SIZE; i++) pi[i] = 0;

    /* Machin-formula: pi = 16 * arctan(1/5) - 4 * arctan(1/239). */
    add_arctan(5, 16, 0);   /* Hozzáadjuk a 16 * arctan(1/5)-öt. */
    add_arctan(239, 4, 1);  /* Kivonjuk a 4 * arctan(1/239)-et. */

    /* Eredmény formázott kiíratása. */
    printf("A valódi PI(π) értéke %d tizedesjegy pontossággal:\n%d.", PLACES, pi[0]);
    for (i = 1; i <= PLACES; i++) {
        printf("%d", pi[i]);
    }
    printf("\n");

    return 0;
}
