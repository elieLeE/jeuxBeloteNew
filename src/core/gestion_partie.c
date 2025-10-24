#include <string.h>

#include "assert.h"

#include "../../libC/src/macros.h"
#include "../../libC/src/logger/logger.h"
#include "../../libC/src/mem/mem.h"

#include "../macros.h"
#include "gestion_partie.h"
#include "gestion_jeu_carte.h"
#include "players.h"
#include "../front/aff.h"

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
    display_player_cards("Yours cards are these ones: ", &(players[0]));

    /* human player is always the first one. His cards always are
     * displayed just above => start at the index 1. */
    for (int i = 1; i < NBRE_JOUEURS; i++) {
        char players_cards_str[PLAYER_CARDS_FMT_SIZE];

        get_player_cards_str(&players[i], players_cards_str);
        logger_trace("player %d has these cards:\n%s\n",
                     players[i].idx, players_cards_str);
    }

    return 0;
}

/* }}} */
/* {{{ Round */

typedef struct card_played_t {
    const carte_t *card;
    int idx_player;
} card_played_t;

typedef struct trick_t {
    card_played_t cards[NBRE_JOUEURS];
    int idx_player_won;
} trick_t;

static void set_card_played_info(card_played_t *card_played,
                                 const carte_t *card, int idx_player)
{
    card_played->card = card;
    card_played->idx_player = idx_player;
}

static const carte_t *
get_next_first_trick_card(player_t *player, couleur_t trump_color)
{
    const carte_t *card;

    logger_trace("it is the turn of the player %d", player->idx);

    card = take_first_card_from_player(player, trump_color);
    if (!card) {
        logger_fatal("the player %d has returned a card NULL", player->idx);
    }

    logger_info("the player %d has played the card '" CARD_FMT "'",
                player->idx, CARD_FMT_ARG(card));

    return card;
}

static const carte_t *
get_next_trick_card(player_t *player,couleur_t asked_color,
                    couleur_t trump_color, int idx_leading_player)
{
    const carte_t *card;

    logger_trace("it is the turn of the player %d", player->idx);

    card = take_card_from_player(player, asked_color, trump_color,
                                 idx_leading_player);
    if (!card) {
        logger_fatal("the player %d has returned a card NULL", player->idx);
    }

    logger_info("the player %d has played the card '" CARD_FMT "'",
                player->idx, CARD_FMT_ARG(card));

    return card;
}

static int get_next_trick(player_t players[NBRE_JOUEURS], int idx_first_player,
                          couleur_t trump_color, trick_t *out)
{
    int idx_player;
    int idx_leading_player = idx_first_player;
    int player_counter = 1;
    const carte_t *master_card = NULL;
    const carte_t *first_card = NULL;

    first_card = master_card =
        get_next_first_trick_card(&(players[idx_first_player]), trump_color);

    set_card_played_info(&out->cards[0], first_card, idx_first_player);

    idx_player = GET_NEXT_PLAYER_IDX(idx_first_player);

    do {
        const carte_t *opponent_card;

        opponent_card =
            get_next_trick_card(&(players[idx_player]), first_card->c,
                                trump_color, idx_leading_player);

        if (cmp_card(master_card, opponent_card) < 0) {
            logger_debug("the card '" CARD_FMT "' takes the lead",
                         CARD_FMT_ARG(opponent_card));
            master_card = opponent_card;
            idx_leading_player = idx_player;
        }

        set_card_played_info(&(out->cards[player_counter]), opponent_card,
                             idx_player);

        player_counter++;
        idx_player = GET_NEXT_PLAYER_IDX(idx_player);
    } while (idx_player != idx_first_player);

    logger_info("the player %d has won this trick", idx_leading_player);

    out->idx_player_won = idx_leading_player;

    return idx_leading_player;
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

#define GET_TEAM_INDEX(_player_idx)                                           \
   _player_idx % NBER_TEAMS

static int get_trick_pts(const trick_t *trick, couleur_t trump_color,
                         int count_belote_and_re_teams[NBER_TEAMS])
{
    int sum = 0;

    for (int i = 0; i < NBER_CARDS_BY_TRICK; i++) {
        const card_played_t *c = &trick->cards[i];

        sum += get_value_card(c->card, trump_color);

        if (c->card->is_trump && (c->card->r == ROI || c->card->r == DAME)) {
            int idx_team = GET_TEAM_INDEX(c->idx_player);

            count_belote_and_re_teams[idx_team]++;
        }
    }

    return sum;
}

static void add_last_trick_ten_pts(trick_t *last_trick, int out[NBER_TEAMS])
{
    int idx_team_won_trick = GET_TEAM_INDEX(last_trick->idx_player_won);

    out[idx_team_won_trick] += 10;
}

static void
get_round_teams_pts(trick_t tricks[NBER_TRICKS], couleur_t trump_color,
                    int idx_player_taking, int out[NBER_TEAMS])
{
    /* TODO => handle case where the teams are the same number of points.
     * In this case, the team that has not taken the card wins 81 points.
     * The other 81 points will be won by the teams winning the next round.
     */
    int idx_team_taking = GET_TEAM_INDEX(idx_player_taking);
    int idx_team_no_taking = GET_TEAM_INDEX((idx_player_taking + 1));
    int count_belote_and_re_teams[NBER_TEAMS] = {0, 0};

    for (int i = 0; i < NBER_TRICKS; i++) {
        trick_t *trick = &tricks[i];
        int idx_team = GET_TEAM_INDEX(trick->idx_player_won);

        out[idx_team] +=
            get_trick_pts(trick, trump_color, count_belote_and_re_teams);
    }

    add_last_trick_ten_pts(&tricks[NBER_TRICKS - 1], out);

    if (count_belote_and_re_teams[0] == 2) {
        out[0] += 20;
        logger_info("team 0 has done 'belote and re'");
    } else if (count_belote_and_re_teams[1] == 2) {
        out[1] += 20;
        logger_info("team 1 has done 'belote and re'");
    }

    logger_info("team 0 has won %d pts, team 1 has won %d pts",
                out[0], out[1]);

    if (out[idx_team_taking] < out[idx_team_no_taking]) {
        out[idx_team_no_taking] = 162;
        out[idx_team_taking] = 0;

        if (count_belote_and_re_teams[0] == 2) {
            out[0] += 20;
        } else if (count_belote_and_re_teams[1] == 2) {
            out[1] += 20;
        }
        logger_info("team %d has taken card but did not succeed its aim. "
                    "The team %d wins 0, pts and the team %d wins 162 pts",
                    idx_team_taking, idx_team_taking, idx_team_no_taking);
    } else {
        logger_info("team %d has taken the card and won this round",
                    idx_team_taking);
    }
}

static void sort_all_players_trump_cards(player_t players[NBRE_JOUEURS],
                                         couleur_t trump_color)
{
    for (int i = 0; i < NBRE_JOUEURS; i++) {
        sort_player_trump_cards(&(players[i]), trump_color);
    }
}

/* handling a new round:
 * - split the cards (the determining of the trump is done inside)
 * - do the trick
 * - increment the points won by each player
 */
static void
start_new_ronud(carte_t game[NBRE_CARTES], player_t players[NBRE_JOUEURS],
                int idx_first_player, int teams_pts[NBER_TEAMS])
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
        sort_all_players_trump_cards(players, trump_color);

        if (get_all_tricks(players, idx_first_player, trump_color, tricks) < 0)
        {
            logger_error("an error happened in 'get_all_tricks'");
        } else {
            get_round_teams_pts(tricks, trump_color, idx_player_taking,
                                teams_pts);
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
    const int game_won_pts = 500;
    player_t players[NBRE_JOUEURS];
    int teams_pts[NBER_TEAMS] = {0, 0};
    int first_player = 0, round_counter = 0;

    init_players(players);

    first_player = rand() % NBRE_JOUEURS;

    do {
        int round_pts[NBER_TEAMS] = {0, 0};

        start_new_ronud(game, players, first_player, round_pts);

        teams_pts[0] += round_pts[0];
        teams_pts[1] += round_pts[1];

        logger_info("end of the round #%d, team 1: %d, team 2: %d",
                    round_counter, teams_pts[0], teams_pts[1]);

        first_player = GET_NEXT_PLAYER_IDX(first_player);
        round_counter++;
    } while(teams_pts[0] < game_won_pts && teams_pts[1] < game_won_pts);

    logger_info("end of this game. The team %d has won",
                teams_pts[0] > teams_pts[1] ? 0 : 1 );
}

/* }}} */
