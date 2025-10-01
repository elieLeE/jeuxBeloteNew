#include <string.h>

#include "assert.h"

#include "../libC/src/macros.h"

#include "test_gestion_cartes.h"
#include "../src/defines.h"
#include "../src/core/carte.h"
#include "../src/core/gestion_jeu_carte.h"

static bool is_same_card(carte_t *c1, carte_t *c2)
{
    return ARE_SAME_STRUCT(c1, c2);
}

static bool search_jeu_carte(carte_t *jeu, carte_t *c)
{
    int i;

    for(i=0; i<NBRE_CARTES; i++) {
        if(is_same_card(&jeu[i], c)) {
            return true;
        }
    }
    return false;
}

static bool verif_jeu_carte(carte_t jeu[])
{
    rang_t r;
    couleur_t c;
    carte_t test_carte;

    for(c=CARREAU; c<=TREFLE; c++) {
        test_carte.c = c;

        for(r=SEPT; r<=AS; r++) {
            test_carte.r = r;

            if(!search_jeu_carte(jeu, &test_carte)) {
                return false;
            }
        }
    }
    return true;
}
void test_melange_jeu(void)
{
    carte_t jeu[NBRE_CARTES];

    melange_jeu(jeu);

    assert(verif_jeu_carte(jeu));

    printf("melange_jeu OK\n");
}
