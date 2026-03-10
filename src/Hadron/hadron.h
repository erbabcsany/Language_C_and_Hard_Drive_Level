//
// Created by ervin on [2026. 03. 04.].
//

/* =====================================================================
   HADRON CORE HEADER FÁJL (hadron.h)
   A Rendszer Tervrajza és az Adat-Kapszula definíciója
   ===================================================================== */

/* Ezt a védőpajzsot "Include Guard"-nak hívják. Megakadályozza,
   hogy a C fordító kétszer töltse be a fájlt és összeomoljon. */
#ifndef HADRON_H
#define HADRON_H
#include "macro.h"
#include "vm.h"

/* =========================================================
   A TISZTA ADAT-KATEGÓRIÁK (A Token Típusai)
   ========================================================= */
typedef enum {
    TOKEN_UNKNOWN = 0,
    TOKEN_ENTITY_OPEN,    /* Amikor új Entitást nyitunk: '{' */
    TOKEN_ENTITY_CLOSE,   /* Amikor lezárjuk a dimenziót: '}' */
    TOKEN_ASSIGN,         /* A sima Értékadás / Mutáció: '=' */
    TOKEN_TRANSITION,     /* A Dimenzióváltás / Genezis: '->' */
    TOKEN_QUANTUM_LOCK    /* A Fizikai Lakat: '!' */
} TokenType;

/* =========================================================
   AZ ÚJ, FELOKOSÍTOTT ADAT-KAPSZULA (Token)
   ========================================================= */
typedef struct {
    char symbol;
    int space_before;
    int space_after;
    int is_privileged;
    int is_hadron_sys;

    /* --- AZ ÚJ DIAGNOSZTIKAI DIMENZIÓ --- */
    const str filepath;  /* pl. "code.hadron" */
    int line_number;       /* Y-tengely */
    int column_number;     /* X-tengely */

    TokenType type;       /* EZ AZ ÚJ AGY! A kőkemény kategória. */
} HadronToken;

/* A FÜGGVÉNYEK HIVATALOS BEJELENTÉSE */
void hadron_vm_init(void); /* A VM bekapcsolása */
str read_hadron_file(const str filename);
void hadron_lexer(const str source, HadronToken* output_token);
void hadron_parser(HadronVM* hadron_vm, const HadronToken* token, const str nyers_szoveg);
int hadron_main(void);

#endif /* HADRON_H */