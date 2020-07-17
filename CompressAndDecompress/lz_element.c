#include "lz_element.h"


LZ_Element* LZ_Element_new()
{
    LZ_Element *e = (LZ_Element*)malloc(sizeof(LZ_Element));

    if (e == NULL) {
        die_error("malloc on LZ_Element failed!\n");
    }

    return e;
}


LZ_Element* LZ_Pair_new(LZ_Pair pair)
{
    LZ_Element *e = (LZ_Element*)malloc(sizeof(LZ_Element));

    if (e == NULL) {
        die_error("malloc on LZ_Element failed!\n");
    }

    e->type             = _LZ_PAIR;
    e->value.p.distance = pair.distance;
    e->value.p.length   = pair.length;

    return e;
}


LZ_Element* LZ_Literal_new(LZ_Literal literal)
{
    LZ_Element *e = (LZ_Element*)malloc(sizeof(LZ_Element));

    if (e == NULL) {
        die_error("malloc on LZ_Element failed!\n");
    }

    e->type    = _LZ_LITERAL;
    e->value.l = literal;

    return e;
}


void LZ_Element_set_literal(LZ_Element *e, LZ_Literal literal)
{
    e->type    = _LZ_LITERAL;
    e->value.l = literal;
}


void LZ_Element_set_pair(LZ_Element *e, LZ_Pair pair)
{
    e->type             = _LZ_PAIR;
    e->value.p.distance = pair.distance;
    e->value.p.length   = pair.length;
}
