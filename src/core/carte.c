#include "carte.h"

int cmp_card(const void *_c1, const void *_c2)
{
    const carte_t *c1 = (carte_t *)_c1;
    const carte_t *c2 = (carte_t *)_c2;

    /* Comparing cards has to be done like this:
     * - if only one of the two cards is trump, then the trump card always win
     * - if the two cards are trump, then the order to consider if the trump
     *   one
     * - if the two cards do not have the same color (but none is a trump),
     *   then the first card win as it is considered as the first card to have
     *   been played.
     * - and finally, no trump order is used */
    if (c1->is_trump) {
        if (c2->is_trump) {
            /* Both cards are trump. I could write a method given the card
             * position but it will be almost the same than 'get_value_card'.
             * The only issue with this method in this context is there is a
             * case where we can not differentiate the two trump cards from
             * theirs values. But we can just return the difference of theirs
             * rank as they have 7 or 8 as value. */
            int c1_val, c2_val;

            c1_val = get_value_card(c1, c1->c);
            c2_val = get_value_card(c2, c2->c);

            if (c1_val != c2_val) {
                return c1_val - c2_val;
            }
            return c1->r - c2->r;
        }
        return 1;
    } else if (c2->is_trump) {
        return -1;
    }

    if (c1->c != c2->c) {
        return 1;
    }

    return c1->r - c2->r;
}

int get_value_card(const carte_t *card, couleur_t color_trump)
{
    bool is_trump = (card->c == color_trump);

    switch(card->r) {
    case AS:
        return 11;

    case DIX:
        return 10;

    case ROI:
        return 4;

    case DAME:
        return 3;

    case VALET:
        return is_trump ? 20 : 2;

    case NEUF:
        return is_trump ? 14 : 0;

    case HUIT:
    case SEPT:
        return 0;
    }

    logger_fatal("unknown rank");
    return 0;
}
