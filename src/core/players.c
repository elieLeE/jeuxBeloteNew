#include <string.h>

#include "../../libC/src/liste/liste.h"
#include "../../libC/src/io/io.h"

#include "players.h"
#include "carte.h"


void free_player_cards(player_t *player)
{
    for (int i = 0; i < NBRE_COUL; i++) {
        gl_free(&(player->cards[i]), NULL);
    }
}

/* {{{ Trump selection */
/* {{{ Human player */

static bool
does_human_player_take_card_first_turn(const player_t *player,
                                       const carte_t *card)
{
    char answer;

    printf("It is your turn to speak. Do you want to take the card '"
           CARD_FMT "' ? (y/n)\n", CARD_FMT_ARG(card));
    printf("If you take it, the trump will be %s\n", name_coul(card->c));
    printf("Enter y/Y for yes and n/N for no\n");

    do {
        answer = getchar();

        flush_stdin();

        switch (answer) {
        case 'y':
        case 'Y':
            return true;

        case 'n':
        case 'N':
            return false;
        }

        printf("No understanding answer. Please respond with y/Y or n/N\n");
    } while(true);

    return false;
}

bool
does_human_player_take_card_second_turn(const player_t *player,
                                        const carte_t *card,
                                        couleur_t *trump_color)
{
    logger_error("does_human_player_take_card_second_turn "
                 "NOT YET IMPLEMENTED");
    return false;
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
