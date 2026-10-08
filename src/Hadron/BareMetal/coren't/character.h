/*
   Created by ervin on [2026. 10. 07.].
*/

#ifndef HADRON_CHARACTER_H
#define HADRON_CHARACTER_H
#define HADRON_CHARACTER(tipus, nev) \
    typedef struct { \
        int x; \
        int x; \
        tipus y; \
    } Vektor_##nev

typedef HADRON_CHARACTER(int, I); Vektor;

#endif /* HADRON_CHARACTER_H */
