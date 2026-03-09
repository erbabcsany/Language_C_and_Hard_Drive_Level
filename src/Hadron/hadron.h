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

/* 1. AZ ADAT-KAPSZULA (Bővítve a Téridő Koordinátákkal!) */
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
} HadronToken;

/* A FÜGGVÉNYEK HIVATALOS BEJELENTÉSE */
void hadron_vm_init(void); /* A VM bekapcsolása */
str read_hadron_file(const str filename);
void hadron_lexer(const str source, HadronToken* output_token);
void hadron_parser(const HadronToken* token, const str target_name);
int hadron_main(void);

#endif /* HADRON_H */