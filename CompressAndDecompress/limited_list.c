#include "limited_list.h"

void Limited_List_init(Limited_List *ll)
{
    ll->values = (LL_Value*)malloc(sizeof(LL_Value)*ll->max_size);
    ll->next_pos = 0;
    ll->start_pos = 0;
    ll->cur_size = 0;
}

void Limited_List_add(Limited_List *ll, LL_Value value)
{
    // if the list is empty
    if (ll->values == NULL) {
        Limited_List_init(ll);
    }

    // adds the element
    ll->values[ll->next_pos] = value;
    ll->next_pos = (ll->next_pos + 1) % ll->max_size;

    // handles the special cases for the indexes
    if (ll->cur_size < ll->max_size) {
        ll->cur_size++;
    }
    else {
        ll->start_pos = (ll->start_pos + 1) % ll->max_size;
    }
}


void Limited_List_destroy(Limited_List *ll)
{
    if (ll->values) {
        free(ll->values);
        ll->values = NULL;
    }
}
