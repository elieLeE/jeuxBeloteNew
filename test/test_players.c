#include <stdio.h>

#include "assert.h"

#include "../libC/src/io/io.h"
#include "../libC/src/macros.h"
#include "../libC/src/mem/mem.h"
#include "../libC/src/liste/liste.h"

#include "test_players.h"

#include "../src/core/carte.h"
#include "../src/core/players.h"

/* {{{ Trump selection */
/* {{{ Human player */

static void
test_does_human_player_take_card_second_turn_(const char *file_name,
                                              bool should_accept,
                                              couleur_t color_forbidden,
                                              couleur_t color_wanted)
{
    bool has_taken_the_card;
    FILE *f;
    player_t player;
    couleur_t color_got;
    carte_t card = {.r = ROI, .c = color_forbidden};

    p_clear(&player, 1);

    player.is_human = true;

    f = redirect_stream(stdin, file_name, "r");

    logger_trace("start %s", file_name);

    has_taken_the_card =
        does_player_take_card_second_turn(&player, &card, &color_got);
    ASSERT(has_taken_the_card == should_accept, "%s", file_name);

    if (should_accept) {
        ASSERT(color_wanted == color_got, "obtained: %s, expected: %s",
           name_coul(color_got), name_coul(color_wanted));
    }
    fermer_fichier(&f);
}

static void test_does_human_player_take_card_second_turn(void)
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
}

/* }}} */
/* {{{ Vurtual player */

static void free_card(void *card)
{
    p_free(&card);
}

static void free_player_cards_(player_t *player)
{
    for (int i = 0; i < NBRE_COUL; i++) {
        gl_free(&(player->cards[i]), free_card);
    }
}

static void
create_card_and_add_to_player(player_t *player, rang_t r, couleur_t c)
{
    carte_t *card = p_calloc(sizeof(carte_t) * 1);

    card->r = r;
    card->c = c;

    gl_add_elem_sorted(&(player->cards[c]), card, cmp_card);
}

/* {{{ First turn */

static void
check_virtual_player_choice_first_turn(player_t *player, rang_t r, couleur_t c,
                                       bool should_accept,
                                       const char *text_err)
{
    bool has_accepted_the_card;
    carte_t card = {.r = r, .c = c};

    logger_trace("start %s", text_err);

    has_accepted_the_card = does_player_take_card_first_turn(player, &card);

    ASSERT(has_accepted_the_card == should_accept, "%s", text_err);
}

static void test_does_virtual_player_take_card_first_turn(void)
{
    player_t player;

    p_clear(&player, 1);

    player.is_human = false;

    /* Valet as trump card => has to be taken */
    create_card_and_add_to_player(&player, DAME, PIQUE);
    create_card_and_add_to_player(&player, SEPT, CARREAU);
    create_card_and_add_to_player(&player, VALET, CARREAU);
    create_card_and_add_to_player(&player, VALET, TREFLE);
    create_card_and_add_to_player(&player, VALET, PIQUE);
    check_virtual_player_choice_first_turn(&player, VALET, COEUR, true,
                                           "First turn, Valet as trump card");
    free_player_cards_(&player);

    /* Valet, 9, an As and 2 kings => should be taken */
    create_card_and_add_to_player(&player, VALET, PIQUE);
    create_card_and_add_to_player(&player, NEUF, COEUR);
    create_card_and_add_to_player(&player, AS, CARREAU);
    create_card_and_add_to_player(&player, ROI, TREFLE);
    create_card_and_add_to_player(&player, ROI, COEUR);
    check_virtual_player_choice_first_turn(&player, NEUF, PIQUE, true,
                                           "First turn, Valet, 9 and an AS");
    free_player_cards_(&player);

    /* Valet, 9 Roi and Dame => should be taken */
    create_card_and_add_to_player(&player, VALET, TREFLE);
    create_card_and_add_to_player(&player, SEPT, PIQUE);
    create_card_and_add_to_player(&player, HUIT, CARREAU);
    create_card_and_add_to_player(&player, ROI, TREFLE);
    create_card_and_add_to_player(&player, DAME, TREFLE);
    check_virtual_player_choice_first_turn(&player, NEUF, TREFLE, true,
                                           "First turn, Valet, 9, Roi and "
                                           "Dame");
    free_player_cards_(&player);

    /* 5 smalls trump => should be taken */
    create_card_and_add_to_player(&player, DAME, CARREAU);
    create_card_and_add_to_player(&player, SEPT, CARREAU);
    create_card_and_add_to_player(&player, AS, CARREAU);
    create_card_and_add_to_player(&player, ROI, CARREAU);
    create_card_and_add_to_player(&player, DIX, CARREAU);
    check_virtual_player_choice_first_turn(&player, HUIT, CARREAU, true,
                                           "First turn, 5 trump cards");
    free_player_cards_(&player);

    /* No trump and not Valet as trump card => should not been taken */
    create_card_and_add_to_player(&player, DAME, CARREAU);
    create_card_and_add_to_player(&player, SEPT, CARREAU);
    create_card_and_add_to_player(&player, AS, CARREAU);
    create_card_and_add_to_player(&player, ROI, CARREAU);
    create_card_and_add_to_player(&player, DIX, CARREAU);
    check_virtual_player_choice_first_turn(&player, AS, PIQUE, false,
                                           "First turn, No trump cards");
    free_player_cards_(&player);

    /* Some trumps card, but not the big ones => should not been taken */
    create_card_and_add_to_player(&player, DIX, CARREAU);
    create_card_and_add_to_player(&player, ROI, CARREAU);
    create_card_and_add_to_player(&player, SEPT, CARREAU);
    create_card_and_add_to_player(&player, ROI, TREFLE);
    create_card_and_add_to_player(&player, DIX, TREFLE);
    check_virtual_player_choice_first_turn(&player, AS, CARREAU, false,
                                           "First turn, not the big trump "
                                           "cards");
    free_player_cards_(&player);

    /* Valet, 9 and that's it => should not be taken */
    create_card_and_add_to_player(&player, VALET, CARREAU);
    create_card_and_add_to_player(&player, SEPT, COEUR);
    create_card_and_add_to_player(&player, HUIT, TREFLE);
    create_card_and_add_to_player(&player, HUIT, PIQUE);
    create_card_and_add_to_player(&player, VALET, PIQUE);
    check_virtual_player_choice_first_turn(&player, NEUF, CARREAU, false,
                                           "First turn, valet and 9");
    free_player_cards_(&player);
}

/* }}} */
/* }}} */
/* }}} */

void test_players(void)
{
    BEGIN_TEST_MODULE("players");

    CALL_TEST_FUNC(test_does_human_player_take_card_second_turn);
    CALL_TEST_FUNC(test_does_virtual_player_take_card_first_turn);

    END_TEST_MODULE();
}
