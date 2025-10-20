#include "../../libC/src/liste/liste.h"

#include "aff.h"

void display_game(const carte_t game[])
{
    for (int i = 0; i < NBRE_CARTES; i++) {
        printf(CARD_FMT ", ", CARD_FMT_ARG((&game[i])));
    }
    printf("\n");
}

