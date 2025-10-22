#include <string.h>

#include "../../libC/src/liste/liste.h"
#include "../../libC/src/io/io.h"
#include "../../libC/src/str/str.h"
#include "../../libC/src/macros.h"
#include "../../libC/libc.h"

#include "players.h"
#include "carte.h"
#include "../front/aff.h"

static void remove_elem_card_from_player(player_t *player, gl_elem_t *elem,
                                         couleur_t color);

void
get_player_cards_str(const player_t *player, char out[PLAYER_CARDS_FMT_SIZE])
{
    int len = 0;

    for (int i = 0; i < NBRE_COUL; i++) {
        const generic_liste_t *l = &(player->cards[i]);

        gl_for_each(elem, l->first) {
            len += snprintf(out + len, PLAYER_CARDS_FMT_SIZE - len,
                           CARD_FMT ", ",
                           CARD_FMT_ARG(((carte_t *)elem->data)));
        }
    }
}

static bool has_player_card(const generic_liste_t *cards, rang_t r)
{
    gl_for_each(elem, cards->first) {
        carte_t *card = (carte_t *)(elem->data);

        if (card->r == r) {
            return true;
        }
    }

    return false;
}

/* return the number of points that a player has for a specific color,
 * with this color the trump */
static int
get_sum_points_list_card(const generic_liste_t *cards, couleur_t trump_color)
{
    int sum = 0;
    int belote_and_re = 0;

    gl_for_each(elem, cards->first) {
        carte_t *c = (carte_t *)elem->data;
        int card_pt;

        card_pt = get_value_card(c, trump_color);
        sum += card_pt;

        logger_trace("card " CARD_FMT " => %d, total: %d",
                    CARD_FMT_ARG(c), card_pt, sum);
        if (c->r == ROI || c->r == DAME) {
            belote_and_re++;
        }
    }

    if ((belote_and_re == 2) && !gl_is_empty(cards)) {
        carte_t *c = (carte_t *)(cards->first->data);

        if (c->c == trump_color) {
            sum += 20;
        }
    }
    return sum;
}

/* {{{ Trump selection */
/* {{{ Human player */

static bool
does_human_player_take_card_first_turn(const player_t *player,
                                       const carte_t *card)
{
    printf("It is your turn to speak. ");
    display_player_cards("Yours cards are these ones: %s\n", player);

    printf("\nDo you want to take the card '" CARD_FMT "' ? (y/n)\n",
           CARD_FMT_ARG(card));
    printf("If you take it, the trump will be %s\n", name_coul(card->c));
    printf("Enter y/Y for yes and n/N for no\n");

    do {
        char answer[3];

        if (read_n_carac_and_flush(3, stdin, answer) == -1) {
            continue;
        }

        logger_trace("answer: %s", answer);

        if (strlen(answer) == 1) {
            switch (answer[0]) {
            case 'y':
            case 'Y':
                return true;

            case 'n':
            case 'N':
                return false;
            }
        }

        printf("No understanding answer. Please respond with y/Y or n/N\n");
    } while(true);
}

static int get_couleur_from_str(const char *str, couleur_t *coul_found)
{
    if (strcmp(str, "CARREAU") == 0) {
        *coul_found = CARREAU;
        return 0;
    }
    if (strcmp(str, "COEUR") == 0) {
        *coul_found = COEUR;
        return 0;
    }
    if (strcmp(str, "PIQUE") == 0) {
        *coul_found = PIQUE;
        return 0;
    }
    if (strcmp(str, "TREFLE") == 0) {
        *coul_found = TREFLE;
        return 0;
    }
    return -1;
}

static bool
does_human_player_take_card_second_turn(const player_t *player,
                                        const carte_t *card,
                                        couleur_t *trump_color)
{
    printf("It is your turn to speak. ");
    display_player_cards("Yours cards are these ones: %s\n", player);

    printf("\nYou can chose the color of the trump (except the one of the "
           "card, as you refused this color on the first turn)\n"
           "Do you want to take the card '" CARD_FMT "' ?\n",
           CARD_FMT_ARG(card));
    printf("If you want to take the card, just enter the color of the trump "
           "you would like\n");
    printf("If you do not want to take the card, just enter n/N\n");

    do {
        char answer[10];
        couleur_t color_asked_by_player;

        if (read_n_carac_and_flush(9, stdin, answer) == -1) {
            continue;
        }
        logger_trace("answer: %s", answer);

        upper_string(answer);

        if (get_couleur_from_str(answer, &color_asked_by_player) == 0) {
            if (color_asked_by_player == card->c) {
                printf("The color %s is not authorized on this turn\n",
                       answer);
            } else {
                *trump_color = color_asked_by_player;
                return true;
            }
        } else {
            if ((strlen(answer) == 1) && (answer[0] == 'N')) {
                return false;
            }
            printf("No understanding answer. Please respond with the color of "
                   "the card or n/N\n");
        }
    } while(true);
}

/* }}} */
/* {{{ Virtual player */

static void
get_player_cards_value(const player_t *player, couleur_t trump_color,
                       int *trump_color_pts, int *total_pts)
{
    const generic_liste_t *trump_cards = &(player->cards[trump_color]);

    *trump_color_pts = get_sum_points_list_card(trump_cards, trump_color);

    *total_pts = *trump_color_pts;
    for (couleur_t i = CARREAU; i <= TREFLE; i++) {
        if (i != trump_color) {
            *total_pts +=
                get_sum_points_list_card(&(player->cards[i]), trump_color);
        }
    }
}

static bool
should_player_take_with_color(const generic_liste_t *trump_cards,
                              int trump_color_pts, int total_pts)
{
    bool has_valet;

    has_valet = has_player_card(trump_cards, VALET);

    if (has_valet) {
        if (trump_cards->nbre_elem >= 3) {
            carte_t *c = (carte_t *)trump_cards->first->data;

            logger_debug("player has at least 3 cards, including the valet "
                         "on color %s - automatically accepted",
                         name_coul(c->c));
            return true;
        }
    }

    if (trump_color_pts >= 34 && total_pts >= 50) {
        carte_t *c = (carte_t *)trump_cards->first->data;

        logger_debug("player has %d points on trump color %s and %d points "
                     "on others colors - accepted",
                     trump_color_pts, name_coul(c->c), total_pts);
        return true;
    }

    return false;
}

static bool does_virtual_player_take_card_first_turn(const player_t *player,
                                                     const carte_t *card)
{
    char players_cards_str[PLAYER_CARDS_FMT_SIZE];
    int trump_color_pts, total_pts, trump_card_pt;
    const generic_liste_t *trump_cards = &(player->cards[card->c]);

    get_player_cards_str(player, players_cards_str);
    logger_trace("player %d has these cards: %s",
                 player->idx, players_cards_str);

    if (card->r == VALET || trump_cards->nbre_elem == 5) {
        return true;
    }

    if (trump_cards->nbre_elem == 0) {
        return false;
    }

    trump_card_pt = get_value_card(card, card->c);
    logger_trace("card trump: " CARD_FMT ", %d",
                 CARD_FMT_ARG(card), trump_card_pt);

    get_player_cards_value(player, card->c, &trump_color_pts, &total_pts);
    trump_color_pts += trump_card_pt;
    total_pts += trump_card_pt;

    logger_debug("trump_color_pts: %d, total_pts: %d",
                trump_color_pts, total_pts);

    return
        should_player_take_with_color(trump_cards, trump_color_pts, total_pts);
}

static bool does_virtual_player_take_card_second_turn(const player_t *player,
                                                      const carte_t *card,
                                                      couleur_t *trump_color)
{
    char players_cards_str[PLAYER_CARDS_FMT_SIZE];
    int total_pts_best_color = 0;
    int trump_color_pts_best_color = 0;
    int best_color = -1;

    get_player_cards_str(player, players_cards_str);
    logger_trace("player %d has these cards: %s",
                 player->idx, players_cards_str);

    for (unsigned int i = CARREAU; i <= TREFLE; i++) {
        bool is_color_ok;
        int trump_color_pts = 0;
        int total_pts, selecting_trump_card;
        const generic_liste_t *trump_cards;

        logger_trace("investigating with color %s for trump", name_coul(i));

        if (i == card->c) {
            logger_trace("on second turn, color of visible card can not be "
                         "selected: skip");
            continue;
        }

        trump_cards = &(player->cards[i]);

        if (trump_cards->nbre_elem == 0) {
            logger_trace("player has no card on color %s - "
                         "automatically rejected", name_coul(i));
            continue;
        }

        if (trump_cards->nbre_elem == 5) {
            carte_t *c = (carte_t *)trump_cards->first->data;

            logger_trace("player has 5 cards on color %s - "
                         "automatically accepted", name_coul(c->c));

            *trump_color = i;

            return true;
        }

        selecting_trump_card = get_value_card(card, i);
        logger_trace("selecting trump card: " CARD_FMT ", %d",
                     CARD_FMT_ARG(card), selecting_trump_card);

        get_player_cards_value(player, i, &trump_color_pts, &total_pts);
        total_pts += selecting_trump_card;

        logger_debug("color %s, trump_color_pts: %d, total_pts: %d",
                     name_coul(i), trump_color_pts, total_pts);

        is_color_ok = should_player_take_with_color(&(player->cards[i]),
                                                    trump_color_pts,
                                                    total_pts);
        if (is_color_ok) {
            if ((total_pts - total_pts_best_color)  +
                (trump_color_pts - trump_color_pts_best_color) > 0)
            {
                logger_debug("color %s should be taken and is the best color "
                             "for now", name_coul(i));
                best_color = i;
            } else {
                logger_debug("color %s could be taken but not better than "
                             "best color", name_coul(i));
            }
        } else {
            logger_trace("color %s should not be taken", name_coul(i));
        }
    }

    if (best_color != -1) {
        *trump_color = best_color;
        return true;
    }

    return false;
}

/* }}} */

bool
does_player_take_card_first_turn(const player_t *player, const carte_t *card)
{
    if (player->is_human) {
        return does_human_player_take_card_first_turn(player, card);
    } else {
        return does_virtual_player_take_card_first_turn(player, card);
    }
}

bool
does_player_take_card_second_turn(const player_t *player, const carte_t *card,
                                  couleur_t *trump_color)
{
    if (player->is_human) {
        return does_human_player_take_card_second_turn(player, card,
                                                       trump_color);
    } else {
        return does_virtual_player_take_card_second_turn(player, card,
                                                         trump_color);
    }
}

/* }}} */
/* {{{ Play a card */

static int get_all_cards_of_color(player_t *player, couleur_t card_color,
                                  gl_elem_t *out[NBRE_CARTES_BY_PLAYER],
                                  int first_idx)
{
    int idx = first_idx;
    const generic_liste_t *cards_list = &(player->cards[card_color]);

    gl_for_each(elem, cards_list->first) {
        logger_trace("getting card " CARD_FMT,
                     CARD_FMT_ARG(((carte_t *)elem->data)));

        ASSERT(idx < NBRE_CARTES_BY_PLAYER, "idx: %d", idx);

        out[idx] = elem;

        idx++;
    }

    return (idx - first_idx);
}

__attr_unused__
static int
get_all_possible_cards(player_t *player, couleur_t asked_color,
                       couleur_t trump_color, int idx_leading_player,
                       gl_elem_t *out[NBRE_CARTES_BY_PLAYER])
{
    int count_possible_cards = 0;

    if (!gl_is_empty(&(player->cards[asked_color]))) {
        return get_all_cards_of_color(player, asked_color, out, 0);
    } else {
        logger_trace("player has not card of the asked color (%s)",
                     name_coul(asked_color));
    }

    if (!gl_is_empty(&(player->cards[trump_color]))) {
        count_possible_cards =
            get_all_cards_of_color(player, trump_color, out, 0);

        if (player->idx != (idx_leading_player + 2) % NBRE_JOUEURS) {
            logger_trace("teammate is no master - player has to play a trump "
                         "card (as it has at least one)");
            return count_possible_cards;
        }
    } else {
        logger_trace("player has not trump color (%s) card",
                     name_coul(trump_color));
    }

    for (couleur_t i = CARREAU; i <= TREFLE; i++) {
        if (i != asked_color && i != trump_color) {
            int count = get_all_cards_of_color(player, i, out,
                                               count_possible_cards);
            count_possible_cards += count;

            logger_trace("%d cards on color %s, %d cards at total",
                         count, name_coul(i), count_possible_cards);
        }
    }

    return count_possible_cards;
}

/* {{{ Human player */

__attr_unused__
static void
get_all_poss_cards_to_play_str(gl_elem_t * const cards[NBRE_CARTES_BY_PLAYER],
                               int nber_cards,
                               char out[PLAYER_CARDS_FMT_SIZE])
{
    int len = 0;

    for (int i = 0; i < nber_cards; i++) {
        carte_t *c = cards[i]->data;

        len += snprintf(out + len, PLAYER_CARDS_FMT_SIZE - len,
                        CARD_FMT " (%d), ", CARD_FMT_ARG(c), i + 1);
    }
}


gl_elem_t *
get_elem_card_from_human_player(gl_elem_t * elem_cards[NBRE_CARTES_BY_PLAYER],
                                int elem_count, couleur_t trump_color,
                                int idx_leading_player)
{
    char players_cards_str[PLAYER_CARDS_FMT_SIZE];
    __attr_unused__ gl_elem_t *elem = NULL;

    get_all_poss_cards_to_play_str(elem_cards, elem_count, players_cards_str);

    printf("It is your turn to play.\n");
    if (idx_leading_player == -1) {
        printf("You are the first to speak on this turn.\n");
    }
    printf("The trump color is %s. Here are the cards that you can play on "
           "this turn:\n%s\n", name_coul(trump_color), players_cards_str);
    printf("To indicate the card you want to play, you have two choice:\n"
           "- indicate the digit beside the card\n"
           "- indicate the card (for example 'ROI DE TREFLE')\n");

    do {
        char answer[20];

        if (read_n_carac_and_flush(19, stdin, answer) == -1) {
            continue;
        }
        logger_trace("answer: %s", answer);

        if (elem) {
            break;
        } else {
            printf("wrong answer\n");
        }
    } while(true);

    return elem;
}

const carte_t *
take_first_card_from_human_player(player_t *player, couleur_t trump_color)
{
    gl_elem_t *player_cards[NBRE_CARTES_BY_PLAYER] = {NULL};
    int idx = 0;
    const carte_t *card_to_play;
    gl_elem_t *elem = NULL;

    for (couleur_t i = CARREAU; i <= TREFLE; i++) {
        gl_for_each(elem, player->cards[i].first) {
            player_cards[idx] = elem;
            idx++;
        }
    }

    elem = get_elem_card_from_human_player(player_cards, idx, trump_color, -1);
    card_to_play = elem->data;

    remove_elem_card_from_player(player, elem, card_to_play->c);

    return card_to_play;
}

const carte_t *
take_card_from_human_player(player_t *player, couleur_t asked_color,
                            couleur_t trump_color, int idx_leading_player)
{
    logger_error("'take_card_from_human_player' NOT YET IMPLEMENTED");
    return NULL;
}

/* }}} */
/* {{{ Virtual player */

const carte_t *
take_first_card_from_virtual_player(player_t *player, couleur_t trump_color)
{
    logger_error("'take_card_from_virtual_player' NOT YET IMPLEMENTED");
    return NULL;
}

const carte_t *
take_card_from_virtual_player(player_t *player, couleur_t asked_color,
                              couleur_t trump_color,
                              int idx_leading_player)
{
    logger_error("'take_card_from_virtual_player' NOT YET IMPLEMENTED");
    return NULL;
}

/* }}} */

const carte_t *
take_first_card_from_player(player_t *player, couleur_t trump_color)
{

    if (player->is_human) {
        return take_first_card_from_human_player(player, trump_color);
    } else {
        return take_first_card_from_virtual_player(player, trump_color);
    }
}

const carte_t *
take_card_from_player(player_t *player, couleur_t asked_color,
                      couleur_t trump_color, int idx_leading_player)
{
    if (player->is_human) {
        return take_card_from_human_player(player, asked_color, trump_color,
                                           idx_leading_player);
    } else {
        return take_card_from_virtual_player(player, asked_color, trump_color,
                                             idx_leading_player);
    }
    return NULL;
}

/* }}} */

static int cmp_card_descending_order(const void *c1, const void *c2)
{
    return - cmp_card(c1, c2);
}

void add_card_to_player(player_t *player, carte_t *card)
{
    generic_liste_t *list_cards;

    list_cards = &(player->cards[card->c]);
    gl_add_elem_sorted(list_cards, card, cmp_card_descending_order);
}

static void remove_elem_card_from_player(player_t *player, gl_elem_t *elem,
                                         couleur_t color)
{
    gl_remove_elem(&(player->cards[color]), elem, NULL);
}

void free_player_cards(player_t *player)
{
    for (int i = 0; i < NBRE_COUL; i++) {
        gl_free(&(player->cards[i]), NULL);
    }
}

