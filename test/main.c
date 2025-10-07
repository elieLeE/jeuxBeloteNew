#include <time.h>
#include <stdlib.h>

#include "test_gestion_cartes.h"
#include "test_players.h"

int main()
{
    srand(time(NULL));

    test_melange_jeu();
    test_coupe_jeu();

    test_does_human_player_take_card_second_turn();

    return 0;
}
