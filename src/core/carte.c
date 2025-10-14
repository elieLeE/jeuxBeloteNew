#include "carte.h"

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
