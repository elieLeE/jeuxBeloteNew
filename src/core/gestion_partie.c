#include <string.h>

#include "assert.h"

#include "../../libC/src/macros.h"
#include "../../libC/src/logger/logger.h"
#include "../../libC/libc.h"
#include "../../libC/src/liste/liste.h"

#include "../defines.h"
#include "gestion_partie.h"
#include "gestion_jeu_carte.h"
#include "players.h"


/* {{{ Splitting cards */

static void
partial_split_cards(carte_t game[], player_t players[NBRE_JOUEURS],
                    int first_player, int card_counter)
{
    int idx_card = 0;
    int idx_joueur = first_player;

    do {
        for (int i = 0; i < card_counter; i++) {
            carte_t *card;
            generic_liste_t *list_cards;

            card = &(game[idx_card]);
            list_cards = &(players[idx_joueur].cards[card->c]);
            gl_add_elem_first(list_cards, card);

            idx_card++;
        }
        idx_joueur = (idx_joueur + 1) % NBRE_JOUEURS;
    } while (idx_joueur != first_player);
}

static int
all_split_cards(carte_t game[NBRE_CARTES], player_t players[NBRE_JOUEURS],
                int first_player)
{
    logger_info("first cards splitting");

    partial_split_cards(game, players, first_player, 2);
    partial_split_cards(&(game[8]), players, first_player, 3);

    return 0;
}

/* }}} */
/* {{{ Round */

/* handling a new round:
 * - split the cards
 * - determine the trump
 * - do the trick
 * - increment the points won by each player
 */
void start_new_ronud(carte_t game[NBRE_CARTES], player_t players[NBRE_JOUEURS],
                     int first_player)
{
    logger_info("coupe du game\n");
    coupe_jeu(game);

    all_split_cards(game, players, first_player);

    for (int i = 0; i < NBRE_JOUEURS; i++) {
        free_player_cards(&(players[i]));
    }
}

/* }}} */
/* {{{ Game */

void start_new_game(carte_t game[])
{
    player_t players[NBRE_JOUEURS];

    memset(players, 0, sizeof(players));

    start_new_ronud(game, players, 0);
}

/* }}} */
