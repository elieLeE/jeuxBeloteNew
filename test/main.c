#include <time.h>
#include <stdlib.h>

#include "test_gestion_cartes.h"
#include "test_players.h"

int main()
{
    srand(time(NULL));

    test_gestion_cartes();

    test_players();

    return 0;
}
