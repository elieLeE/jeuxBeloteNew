#ifndef _GESTION_JEU_CARTE_H_
#define _GESTION_JEU_CARTE_H_

#include "carte.h"

int melange_jeu(carte_t jeu[]);
void coupe_jeu(carte_t jeu[]);
void set_cards_trump_status(carte_t cards[NBRE_CARTES], couleur_t color_card);
void reset_cards_trump_status(carte_t cards[NBRE_CARTES]);

#endif
