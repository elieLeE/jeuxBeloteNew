#include <stdlib.h>

#include "../../defines.h"
#include "gestion_jeu_carte.h"

void melange_jeu(carte_t jeu[])
{
    bool done[NBRE_CARTES] = {false};
    int p, i, j;

    // TODO => to improve
    for(i=0; i<NBRE_COUL; i++) {
        for(j=SEPT; j<=AS; j++) {
            do {
                p = rand() % (NBRE_CARTES-0);
            } while(done[p]);

            jeu[p].c = i;
            jeu[p].r = j;

            done[p] = true;
        }
    }
}
