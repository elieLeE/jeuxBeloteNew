#include <time.h>
#include <stdlib.h>

#include "test_gestion_cartes.h"

int main()
{
    srand(time(NULL));

    test_melange_jeu();
    test_coupe_jeu();

    return 0;
}
