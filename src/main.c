#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "defines.h"
#include "core/carte.h"
#include "core/gestion_jeu_carte.h"

int main()
{
    carte_t jeu[NBRE_CARTES];

    srand(time(NULL));

    printf("jeu belote\n");

    melange_jeu(jeu);

    return 0;
}
