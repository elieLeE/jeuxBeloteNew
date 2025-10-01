#include <string.h>

#include "assert.h"

#include "../libC/src/macros.h"

#include "test_gestion_cartes.h"
#include "../src/defines.h"
#include "../src/core/carte.h"
#include "../src/core/gestion_jeu_carte.h"

static bool are_same_cards(carte_t *c1, carte_t *c2)
{
    return ARE_SAME_STRUCT(c1, c2);
}

static int get_card_idx(carte_t *jeu, carte_t *c)
{
    for (int i = 0; i < NBRE_CARTES; i++) {
        if (are_same_cards(&jeu[i], c)) {
            return i;
        }
    }
    return -1;
}

static void verif_jeu_carte(carte_t jeu[])
{
    rang_t r;
    couleur_t c;
    carte_t tmp;

    for (c = CARREAU; c <= TREFLE; c++) {
        tmp.c = c;

        for (r = SEPT; r <= AS; r++) {
            tmp.r = r;

            assert (get_card_idx(jeu, &tmp) >= 0);
        }
    }
}

void test_melange_jeu(void)
{
    carte_t jeu[NBRE_CARTES];

    melange_jeu(jeu);

    verif_jeu_carte(jeu);

    printf("melange_jeu OK\n");
}
