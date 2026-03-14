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

typedef struct {
   unsigned char data[32];
} RawToken;

/* =========================================================
   A TISZTA ADAT-KATEGÓRIÁK (A Token Típusai)
   ========================================================= */
typedef enum {
   TOKEN_UNKNOWN = 0,
   TOKEN_ENTITY_OPEN,
   TOKEN_ENTITY_CLOSE,
   TOKEN_ASSIGN,
   TOKEN_ENTITY_DEF,
   TOKEN_TRANSITION,
   TOKEN_QUANTUM_LOCK,

   /* === AZ ŰR FORMÁI (A megőrzött szóközök) === */
   TOKEN_WHITESPACE_SPACE,   /* Sima szóköz (' ') */
   TOKEN_WHITESPACE_TAB,     /* Tabulátor ('\t') */
   TOKEN_WHITESPACE_NEWLINE  /* Sortörés ('\n') */
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
    int numeric_value;  /* <--- EZ AZ ÚJ MEZŐ: A kőkemény számérték! */
} Token;

/* A FÜGGVÉNYEK HIVATALOS BEJELENTÉSE */
void hadron_vm_init(void); /* A VM bekapcsolása */
str read_hadron_file(const str filename);
void hadron_lexer(const str source, Token* output_token);
void hadron_parser(HadronVM* hadron_vm, const Token* token);
int hadron_main(void);

#endif /* HADRON_H */