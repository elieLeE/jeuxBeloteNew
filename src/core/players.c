#include <string.h>

#include "../../libC/src/liste/liste.h"
#include "../../libC/src/io/io.h"
#include "../../libC/src/str/str.h"

#include "players.h"
#include "carte.h"
#include "../front/aff.h"


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

void add_card_to_player(player_t *player, carte_t *card)
{
    generic_liste_t *list_cards;

    list_cards = &(player->cards[card->c]);
    gl_add_elem_first(list_cards, card);
}

void free_player_cards(player_t *player)
{
    for (int i = 0; i < NBRE_COUL; i++) {
        gl_free(&(player->cards[i]), NULL);
    }
}

