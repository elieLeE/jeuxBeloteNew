#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "../../libC/src/logger/logger.h"

#include "macros.h"
#include "core/carte.h"
#include "core/gestion_jeu_carte.h"
#include "core/gestion_partie.h"

int main()
{
    carte_t jeu[NBRE_CARTES];

    srand(time(NULL));

    logger_info("starting belote game\n");

    if (melange_jeu(jeu) < 0) {
        logger_error("error when trying to mix the cards. "
                     "The game can not be started");
    }

     start_new_game(jeu);

    return 0;
}
