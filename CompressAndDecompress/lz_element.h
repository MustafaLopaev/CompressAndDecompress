#ifndef __LZ_ELEMENT__
#define __LZ_ELEMENT__

#include <stdio.h>
#include <stdlib.h>

#include "util.h"
#include "lz_types.h"

typedef struct __lz_element {
    LZ_Value value;
    LZ_Type  type;
} LZ_Element;

#define LZE_GET_LITERAL(e)  (e)->value.l
#define LZE_GET_DISTANCE(e) (e)->value.p.distance
#define LZE_GET_LENGTH(e)   (e)->value.p.length
#define LZE_IS_LITERAL(e)   ((e)->type == _LZ_LITERAL)
#define LZE_IS_PAIR(e)      ((e)->type == _LZ_PAIR)

LZ_Element* LZ_Element_new();
LZ_Element* LZ_Pair_new(LZ_Pair pair);
LZ_Element* LZ_Literal_new(LZ_Literal literal);

// SETTERS
void LZ_Element_set_literal(LZ_Element *element, LZ_Literal literal);
void LZ_Element_set_pair(LZ_Element *element, LZ_Pair pair);

#endif /* __LZ_ELEMENT__ */
