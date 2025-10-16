#include <string.h>
#include <stdio.h>

#include "assert.h"

#include "../libC/src/io/io.h"
#include "../libC/src/macros.h"

#include "test_players.h"

#include "../src/core/carte.h"
#include "../src/core/players.h"

void test_does_human_player_take_card_second_turn_(const char *file_name,
                                                   bool should_accept,
                                                   couleur_t color_forbidden,
                                                   couleur_t color_wanted)
{
    bool has_taken_the_card;
    FILE *f;
    player_t player;
    couleur_t color_get;
    carte_t card = {.r = ROI, .c = color_forbidden};

    memset(&player, 0, sizeof(player_t));

    player.is_human = true;

    f = redirect_stream(stdin, file_name, "r");

    has_taken_the_card =
        does_player_take_card_second_turn(&player, &card, &color_get);
    if (has_taken_the_card && !should_accept) {
        ASSERT(false, "user should not have accepted the card");
    } else if (!has_taken_the_card && should_accept) {
        ASSERT(false, "user should have accepted the card");
    }

    if (should_accept) {
        ASSERT(color_wanted == color_get, "obtained: %s, expected: %s",
           name_coul(color_get), name_coul(color_wanted));
    }
    fermer_fichier(&f);
}

void test_does_human_player_take_card_second_turn(void)
{
    test_does_human_player_take_card_second_turn_(
        "test_files/human_player_tale_card_second_turn/coeur_sucess_loop_1.txt",
        true, CARREAU, COEUR);

    test_does_human_player_take_card_second_turn_(
        "test_files/human_player_tale_card_second_turn/pique_sucess_loop_1.txt",
        true, TREFLE, PIQUE);

    test_does_human_player_take_card_second_turn_(
        "test_files/human_player_tale_card_second_turn/trefle_sucess_loop_1.txt",
        true, COEUR, TREFLE);

    test_does_human_player_take_card_second_turn_(
        "test_files/human_player_tale_card_second_turn/carreau_sucess_loop_1.txt",
        true, PIQUE, CARREAU);

    test_does_human_player_take_card_second_turn_(
        "test_files/human_player_tale_card_second_turn/coeur_sucess_loop_3.txt",
        true, CARREAU, COEUR);

    test_does_human_player_take_card_second_turn_(
        "test_files/human_player_tale_card_second_turn/reject.txt",
        false, CARREAU, COEUR);

    test_does_human_player_take_card_second_turn_(
        "test_files/human_player_tale_card_second_turn/reject_2.txt",
        false, CARREAU, COEUR);

    logger_test_ok("does_human_player_take_card_second_turn");
}
