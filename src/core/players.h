#ifndef __JOUEURS_H__
#define __JOUEURS_H__

#include <stdbool.h>

#include "../../libC/src/liste/liste.h"

#include "carte.h"
#include "../macros.h"

typedef struct player_t {
    int idx;
    /* XXX: it could have been possible to use arrays here. But as the size
     * will change constantly, list are very useful for that. Moreover, that
     * lets me the opportunity to test, in a "real" situation my list generic
     * library.
     * And it is fun to play with my new generic list.
     *
     * Cards of the player:
     * array of 4 lists of pointers of carte_t */
    generic_liste_t cards[NBRE_COUL];
    bool is_human;
} player_t;

bool does_player_take_card_first_turn(const player_t *player,
                                      const carte_t *card);

bool does_player_take_card_second_turn(const player_t *player,
                                       const carte_t *card,
                                       couleur_t *color_chosen);
void add_card_to_player(player_t *player, carte_t *card);
carte_t * take_first_card_from_player(player_t *player, couleur_t trump_color);
carte_t *take_card_from_player(player_t *player, couleur_t asked_color,
                               couleur_t trump_color,
                               int idx_player_master_card);

void free_player_cards(player_t *player);

#endif
