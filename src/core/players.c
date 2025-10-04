#include "../../libC/src/liste/liste.h"

#include "players.h"

void free_player_cards(player_t *player)
{
    for (int i = 0; i < NBRE_COUL; i++) {
        gl_free(&(player->cards[i]), NULL);
    }
}

static bool
does_human_player_take_card(player_t *player, carte_t *card,
                            trump_color_turn_t turn, couleur_t *color_chosen)
{
    logger_error("does_human_player_take_card NOT YET IMPLEMENTED");
    return false;
}

static bool
does_virtual_player_take_card(player_t *player, carte_t *card,
                              trump_color_turn_t turn, couleur_t *color_chosen)
{
    logger_error("does_virtual_player_take_card NOT YET IMPLEMENTED");
    return false;
}

bool does_player_take_card(player_t *player, carte_t *card,
                           trump_color_turn_t turn, couleur_t *color_chosen)
{
    if (player->is_human) {
        return does_human_player_take_card(player, card, turn, color_chosen);
    } else {
        return does_virtual_player_take_card(player, card, turn, color_chosen);
    }
}
