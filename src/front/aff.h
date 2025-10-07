#ifndef __AFF_H__
#define __AFF_H__

#include "../defines.h"
#include "../core/carte.h"
#include "../core/players.h"

void display_game(const carte_t game[]);
void display_player_cards(const player_t *player);
void display_players_cards(const player_t players[NBRE_JOUEURS]);

#endif
