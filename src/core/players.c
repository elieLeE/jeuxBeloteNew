#include <string.h>

#include "../../libC/src/liste/liste.h"
#include "../../libC/src/io/io.h"
#include "../../libC/src/str/str.h"
#include "../../libC/libc.h"

#include "players.h"
#include "carte.h"
#include "../front/aff.h"


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

        logger_info("card " CARD_FMT " => %d, total: %d",
                    CARD_FMT_ARG(c), card_pt, sum);
        if (c->r == ROI || c->r == DAME) {
            belote_and_re++;
        }
    }

    if (belote_and_re == 2) {
        sum += 20;
    }
    return sum;
}

/* {{{ Trump selection */
/* {{{ Human player */

static bool
does_human_player_take_card_first_turn(const player_t *player,
                                       const carte_t *card)
{
    printf("It is your turn to speak. Here are yours cards: \n");
    display_player_cards(player);

    printf("\nDo you want to take the card '" CARD_FMT "' ? (y/n)\n",
           CARD_FMT_ARG(card));
    printf("If you take it, the trump will be %s\n", name_coul(card->c));
    printf("Enter y/Y for yes and n/N for no\n");

    do {
        char answer[3];

        if (read_n_carac_and_flush(3, stdin, answer) == -1) {
            continue;
        }

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

bool
does_human_player_take_card_second_turn(const player_t *player,
                                        const carte_t *card,
                                        couleur_t *trump_color)
{
    printf("It is your turn to speak. Here are yours cards: \n");
    display_player_cards(player);

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

__attr_unused__
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

__attr_unused__
static bool
should_player_take_with_color(const generic_liste_t *trump_cards_list,
                            int trump_color_pts, int total_pts)
{
    bool has_valet;

    has_valet = has_player_card(trump_cards_list, VALET);

    if (has_valet) {
        if (trump_cards_list->nbre_elem >= 3) {
            return true;
        }
    }

    if (trump_color_pts >= 34 && total_pts >= 50) {
        return true;
    }

    return false;
}

static bool does_virtual_player_take_card_first_turn(const player_t *player,
                                                     const carte_t *card)
{
    logger_error("does_virtual_player_take_card_first_turn "
                 "NOT YET IMPLEMENTED");
    return false;
}

static bool does_virtual_player_take_card_second_turn(const player_t *player,
                                                      const carte_t *card,
                                                      couleur_t *trump_color)
{
    logger_error("does_virtual_player_take_card_second_turn "
                 "NOT YET IMPLEMENTED");
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

void free_player_cards(player_t *player)
{
    for (int i = 0; i < NBRE_COUL; i++) {
        gl_free(&(player->cards[i]), NULL);
    }
}

