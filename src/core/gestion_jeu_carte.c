#include <stdlib.h>

#include "assert.h"

#include "../defines.h"
#include "gestion_jeu_carte.h"

int melange_jeu(carte_t jeu[])
{
    bool done[NBRE_CARTES] = {false};
    unsigned int count = 0;

    for(couleur_t i=0; i<NBRE_COUL; i++) {
        for(rang_t j=SEPT; j<=AS; j++) {
            unsigned int p;
            int idx_carte;

            p = rand() % (NBRE_CARTES - count);

            /* looking for a place for the new card */
            if (!done[p]) {
                /* if the index got randomly is still available, we take it */
                idx_carte = p;
            } else {
                /* else we are looking for another place. As the index got
                 * randomly are decreased, we can suppose that the first index
                 * to be filled will be the first ones, so, we start here by
                 * the biggest one in order to limit the investigation time. */
                idx_carte = NBRE_CARTES -1;

                while(p != 0) {
                    if (!done[idx_carte]) {
                        p--;
                    }
                    idx_carte--;
                }
                while(done[idx_carte]) {
                    idx_carte--;

                    if (idx_carte < 0) {
                        fprintf(stderr, "index got is invalid");
                        assert(false);

                        return -1;
                    }
                }
            }

            jeu[idx_carte].c = i;
            jeu[idx_carte].r = j;

            done[idx_carte] = true;

            count++;
        }
    }

    return 0;
}
