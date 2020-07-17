#ifndef __HASH_TABLE__
#define __HASH_TABLE__

#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "limited_list.h"


#define HASH_TABLE_SIZE 8192

// hash function
#define GET_HASH(key) ((((uint32_t)(*key)&0xffffff)*0x6b43a9b5)>>19)

#define HTABLE_GET(ht,k)   &ht[GET_HASH(k)]
#define HTABLE_PUT(ht,k,v) Limited_List_add(&(ht)[GET_HASH(k)], (v))

typedef uint8_t Hash_Key[3];
typedef Limited_List* Hash_Table;

void Hash_Table_init(Hash_Table ht, size_t max_list_size);
void Hash_Table_reset(Hash_Table ht);

#endif /* __HASH_TABLE__ */
