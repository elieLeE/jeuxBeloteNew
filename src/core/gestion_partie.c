#include <string.h>

#include "assert.h"

#include "../../libC/src/macros.h"
#include "../../libC/src/logger/logger.h"
#include "../../libC/src/mem/mem.h"

#include "../macros.h"
#include "gestion_partie.h"
#include "gestion_jeu_carte.h"
#include "players.h"

#define GET_NEXT_PLAYER_IDX(_idx_player)                                      \
    (_idx_player + 1) % NBRE_JOUEURS;

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
        idx_player = GET_NEXT_PLAYER_IDX(idx_player);
    } while (idx_player != idx_first_player);
}

static void
final_split_cards(carte_t game[], player_t players[NBRE_JOUEURS],
                  int idx_first_player, int idx_player_taking)
{
    /* The first card of the current cards will be given the player that has
     * taken it */
    int idx_card = 1;
    int idx_player = idx_first_player;

    add_card_to_player(&(players[idx_player_taking]), &game[0]);
    do {
        int card_counter = (idx_player == idx_player_taking) ? 2 : 3;
        player_t *player = &(players[idx_player]);

        for (int i = 0; i < card_counter; i++) {
            add_card_to_player(player, &(game[idx_card]));
            idx_card++;
        }
        idx_player = GET_NEXT_PLAYER_IDX(idx_player);
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
chose_trmup_color(const player_t players[NBRE_JOUEURS], int idx_first_player,
                  const carte_t *card, trump_color_turn_t turn,
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
            logger_info("the player %d has accepted the card on turn %d",
                        idx_player, turn + 1);
            return 0;
        } else {
            logger_info("the player %d has refused the card on turn %d",
                        idx_player, turn + 1);
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

    logger_info("asking to the players players if they want the card '"
                CARD_FMT "' - turn 1", CARD_FMT_ARG(trump_card));

    if (chose_trmup_color(players, idx_first_player, trump_card, TURN_1,
                          trump_color, idx_player_taking) == -1)
    {
        logger_info("all players have refused the card on first turn - "
                    "turn 2");
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
    logger_info("the color of the trump is %s", name_coul(*trump_color));

    final_split_cards(&(game[20]), players, idx_first_player,
                      *idx_player_taking);

    logger_info("all the cards have been well split");

    for (int i = 0; i < NBRE_JOUEURS; i++) {
        char players_cards_str[PLAYER_CARDS_FMT_SIZE];

        get_player_cards_str(&players[i], players_cards_str);
        logger_trace("player %d has these cards:\n%s\n",
                     players[i].idx, players_cards_str);
    }

    return 0;
}

/* }}} */
/* {{{ Round */

typedef struct trick_t {
    const carte_t *cards[NBRE_JOUEURS];
    int idx_player_won;
} trick_t;

static int get_next_trick(player_t players[NBRE_JOUEURS], int idx_first_player,
                          couleur_t trump_color, trick_t *out)
{
    int idx_player;
    int idx_master_player = idx_first_player;
    int player_counter = 1;
    const carte_t *master_card = NULL;
    const carte_t *first_card = NULL;

    first_card = master_card =
        RETHROW_PN(take_first_card_from_player(&(players[idx_first_player]),
                                               trump_color));
    out->cards[player_counter] = first_card;


    idx_player = GET_NEXT_PLAYER_IDX(idx_first_player);

    do {
        const carte_t *opponent_card;

        opponent_card =
            RETHROW_PN(take_card_from_player(&(players[idx_player]),
                                             first_card->c, trump_color,
                                             idx_master_player));

        if (cmp_card(master_card, opponent_card) < 0) {
            master_card = opponent_card;
            idx_master_player = idx_player;
        }

        out->cards[player_counter] = opponent_card;

        player_counter++;
        idx_player = GET_NEXT_PLAYER_IDX(idx_player);
    } while (idx_player != idx_first_player);

    out->idx_player_won = idx_master_player;

    return idx_master_player;
}

static int get_all_tricks(player_t players[NBRE_JOUEURS], int idx_first_player,
                          couleur_t trump_color, trick_t tricks[NBER_TRICKS])
{
    for (int i = 0; i < NBER_TRICKS; i++) {
        idx_first_player =
            RETHROW(get_next_trick(players, idx_first_player, trump_color,
                                   &(tricks[i])));
    }
    return 0;
}

/* handling a new round:
 * - split the cards (the determining of the trump is done inside)
 * - do the trick
 * - increment the points won by each player
 */
static void
start_new_ronud(carte_t game[NBRE_CARTES], player_t players[NBRE_JOUEURS],
                int idx_first_player)
{
    int res;
    couleur_t trump_color = -1;
    int idx_player_taking = -1;

    logger_info("coupe du game\n");
    coupe_jeu(game);

    res = all_split_cards(game, players, idx_first_player, &trump_color,
                          &idx_player_taking);
    if (res >= 0) {
        trick_t tricks[NBER_TRICKS];

        p_clear(tricks, NBER_TRICKS);

        set_cards_trump_status(game, trump_color);
        if (get_all_tricks(players, idx_first_player, trump_color, tricks) < 0)
        {
            logger_error("an error happened in 'get_all_tricks'");
        } else {
            logger_error("COUNT POINTS WON BY TEAMS - NOT YET IMPLEMENTED");
        }
        reset_cards_trump_status(game);
    } else {
        logger_info("none players has taken the card - "
                    "this round is canceled");
    }

    for (int i = 0; i < NBRE_JOUEURS; i++) {
        free_player_cards(&(players[i]));
    }
}

/* }}} */
/* {{{ Game */

static void init_players(player_t players[NBRE_JOUEURS])
{
    p_clear(players, NBRE_JOUEURS);

    players[0].is_human = true;

    for (int i = 0; i < NBRE_JOUEURS; i++) {
        players[i].idx = i;
    }
}

void start_new_game(carte_t game[])
{
    player_t players[NBRE_JOUEURS];

    init_players(players);

    start_new_ronud(game, players, 0);
}

/* }}} */
