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
                    int idx_first_player, int card_counter)
{
    int idx_card = 0;
    int idx_player = idx_first_player;

    do {
        player_t *player = &(players[idx_player]);

        for (int i = 0; i < card_counter; i++) {
            add_card_to_player(player, &(game[idx_card]));
            idx_card++;
        }
        idx_player = (idx_player + 1) % NBRE_JOUEURS;
    } while (idx_player != idx_first_player);
}


typedef enum trump_color_turn_t {
    TURN_1,
    TURN_2,
} trump_color_turn_t;

/* Method letting us determin the trump.
 * It will "ask" to every player, on eventually two turns if they want to take
 * the card and on which color.
 *
 * return:
 *  0  => the card has been taken
 *  -1 => the card has been taken by no player
 */
static int
chose_trmup_color(player_t players[NBRE_JOUEURS], int idx_first_player,
                  carte_t *card, trump_color_turn_t turn,
                  couleur_t *trump_color, int *idx_player_taking)
{
    int idx_player = idx_first_player;

    do {
        bool has_card_been_taken = false;

        if (turn == TURN_1) {
            has_card_been_taken =
                does_player_take_card_first_turn(&(players[idx_player]), card);
        } else {
            has_card_been_taken =
                does_player_take_card_second_turn(&(players[idx_player]),
                                                  card, trump_color);
        }

        if (has_card_been_taken) {
            *idx_player_taking = idx_player;

            if (turn == TURN_1) {
                *trump_color = card->c;
            }
            return 0;
        }

        idx_player = (idx_player + 1) % NBRE_JOUEURS;
    } while (idx_player != idx_first_player);

    return -1;
}

static int
all_split_cards(carte_t game[NBRE_CARTES], player_t players[NBRE_JOUEURS],
                int idx_first_player, couleur_t *trump_color,
                int *idx_player_taking)
{
    carte_t *trump_card;

    logger_info("first cards splitting");

    partial_split_cards(game, players, idx_first_player, 2);
    partial_split_cards(&(game[8]), players, idx_first_player, 3);

    trump_card = &(game[20]);
    if (chose_trmup_color(players, idx_first_player, trump_card, TURN_1,
                          trump_color, idx_player_taking) == -1)
    {
        RETHROW(chose_trmup_color(players, idx_first_player, trump_card,
                                  TURN_2, trump_color, idx_player_taking));
    }

    if (*trump_color < CARREAU || *trump_color > TREFLE) {
        logger_fatal("the color chosen has unknown value");
    }
    if (*idx_player_taking < 0 || *idx_player_taking >= NBRE_JOUEURS) {
        logger_fatal("the index of the player that has taken the card is "
                     "wrong");
    }
    return 0;
}

/* }}} */
/* {{{ Round */

/* handling a new round:
 * - split the cards (the determining of the trump is done inside)
 * - do the trick
 * - increment the points won by each player
 */
void start_new_ronud(carte_t game[NBRE_CARTES], player_t players[NBRE_JOUEURS],
                     int idx_first_player)
{
    couleur_t trump_color = -1;
    int idx_player_taking = -1;

    logger_info("coupe du game\n");
    coupe_jeu(game);

    if (all_split_cards(game, players, idx_first_player, &trump_color,
                        &idx_player_taking) == 0)
    {
    }

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
