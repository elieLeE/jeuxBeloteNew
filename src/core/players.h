#ifndef __JOUEURS_H__
#define __JOUEURS_H__

#include <stdbool.h>

#include "../../libC/src/liste/type.h"

#include "carte.h"
#include "../defines.h"

typedef struct player_t {
    generic_liste_t cards[NBRE_COUL];
    bool is_human;
} player_t;

bool does_player_take_card(player_t *player, carte_t *card,
                           trump_color_turn_t turn, couleur_t *color_chosen);
void add_card_to_player(player_t *player, carte_t *card);

void free_player_cards(player_t *player);

#endif
