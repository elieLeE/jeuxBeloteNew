#include <string.h>

#include "assert.h"

#include "../libC/src/macros.h"
#include "../libC/src/logger/logger.h"

#include "test_gestion_cartes.h"
#include "../src/macros.h"
#include "../src/core/carte.h"
#include "../src/core/gestion_jeu_carte.h"

static bool are_same_cards(carte_t *c1, carte_t *c2)
{
    return c1->r == c2->r && c1->c == c2->c;
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

    logger_test_ok("melange_jeu");
}

void test_coupe_jeu()
{
    int idx_coupe = -1;

    carte_t jeu[NBRE_CARTES];
    carte_t jeu2[NBRE_CARTES];

    melange_jeu(jeu);
    verif_jeu_carte(jeu);

    memcpy(jeu2, jeu, sizeof(jeu2));

    coupe_jeu(jeu2);

    idx_coupe = get_card_idx(jeu2, &jeu[0]);
    assert(idx_coupe > 0 && idx_coupe < NBRE_CARTES -1);

    for (int i = 1, j = idx_coupe + 1; j != idx_coupe; )
    {
        assert (are_same_cards(&jeu[i], &jeu2[j]));

        i = (i + 1) % NBRE_CARTES;
        j = (j + 1) % NBRE_CARTES;
    }

    logger_test_ok("coupe_jeu");
}
