#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "defines.h"
#include "core/carte.h"
#include "core/gestion_jeu_carte.h"
#include "core/gestion_partie.h"

int main()
{
    carte_t jeu[NBRE_CARTES];

    srand(time(NULL));

    printf("jeu belote\n");

    if (melange_jeu(jeu) < 0) {
        fprintf(stderr, "error when trying to mix the cards. "
                "The game can not be started");
    }

     demarrage_nvelle_partie(jeu);

    return 0;
}
