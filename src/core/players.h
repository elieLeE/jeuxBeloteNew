#ifndef __JOUEURS_H__
#define __JOUEURS_H__

#include <stdbool.h>

#include "../../libC/src/liste/type.h"

#include "carte.h"
#include "../macros.h"

typedef struct player_t {
    generic_liste_t cards[NBRE_COUL];
    bool is_human;
} player_t;

bool does_player_take_card_first_turn(const player_t *player,
                                      const carte_t *card);

bool does_human_player_take_card_second_turn(const player_t *player,
                                             const carte_t *card,
                                             couleur_t *trump_color);
bool does_player_take_card_second_turn(const player_t *player,
                                       const carte_t *card,
                                       couleur_t *color_chosen);
void add_card_to_player(player_t *player, carte_t *card);

void free_player_cards(player_t *player);

#endif
