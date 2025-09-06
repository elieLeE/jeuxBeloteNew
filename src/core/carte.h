#ifndef _GESTION_CARTE_H_
#define _GESTION_CARTE_H_

#include <stdio.h>
#include <stdbool.h>

typedef enum rang_t {
    SEPT,
    HUIT,
    NEUF,
    VALET,
    DAME,
    ROI,
    DIX,
    AS
} rang_t;

typedef enum couleur_t {
    CARREAU,
    COEUR,
    PIQUE,
    TREFLE
} couleur_t;

typedef struct carte_t {
    rang_t r;
    couleur_t c;
    bool est_atout;
} carte_t;

#endif
