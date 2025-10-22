#include "aff.h"
#include "../core/players.h"

void display_game(const carte_t game[])
{
    for (int i = 0; i < NBRE_CARTES; i++) {
        printf(CARD_FMT ", ", CARD_FMT_ARG((&game[i])));
    }
    printf("\n");
}

void display_player_cards(const char *txt, const player_t *player)
{
    char players_cards_str[PLAYER_CARDS_FMT_SIZE];

    get_player_cards_str(player, players_cards_str);

    printf("%s%s", txt, players_cards_str);
}
