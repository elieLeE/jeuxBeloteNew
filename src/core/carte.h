#ifndef _GESTION_CARTE_H_
#define _GESTION_CARTE_H_

#include <stdio.h>
#include <stdbool.h>

#include "../../libC/src/logger/logger.h"

#include "../macros.h"

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
    bool is_trump;
} carte_t;

inline static const char* name_rang(rang_t r)
{
    static const char* name[NBRE_CARTES_BY_COUL] = {
        "SEPT", "HUIT", "NEUF", "VALET", "DAME", "ROI", "DIX", "AS"};

    if ( r>= SEPT && r <= AS) {
        return name[r];
    }

    logger_fatal("rang_t inconnu: %d\n", r);
    return "";
}

inline static const char* name_coul(couleur_t c)
{
    static const char* name[NBRE_COUL] = {
        "CARREAU", "COEUR", "PIQUE", "TREFLE"};

    if (c >= CARREAU && c <= TREFLE) {
        return name[c];
    }

    logger_fatal("couleur_t unknown: %d\n", c);
    return "";
}

#define CARD_FMT "%s de %s"
#define CARD_FMT_ARG(_c) \
    name_rang(_c->r), name_coul(_c->c)

#endif
