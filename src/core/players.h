#ifndef __JOUEURS_H__
#define __JOUEURS_H__

#include "../../libC/src/liste/type.h"
#include "../defines.h"

typedef struct player_t {
    generic_liste_t cards[NBRE_COUL];
    bool is_human;
} player_t;

#endif
