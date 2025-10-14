#include "../../libC/src/liste/liste.h"

#include "aff.h"

void display_game(const carte_t game[])
{
    for (int i = 0; i < NBRE_CARTES; i++) {
        printf(CARD_FMT ", ", CARD_FMT_ARG((&game[i])));
    }
    printf("\n");
}

void display_player_cards(const player_t *player)
{
    for (int i = 0; i < NBRE_COUL; i++) {
        const generic_liste_t *l = &(player->cards[i]);

        gl_for_each(elem, l->first) {
            printf(CARD_FMT ", ", CARD_FMT_ARG(((carte_t *)elem->data)));
        }
    }
    printf("\n");
}

void display_players_cards(const player_t players[NBRE_JOUEURS])
{
    for (int i = 0; i < NBRE_JOUEURS; i++) {
        printf("joueur %d: ", i);
        display_player_cards(&(players[i]));
        printf("\n");
    }
}

