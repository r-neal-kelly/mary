#pragma once

#include <Mary/Vector.h>

typedef struct
{
  Mary_Vector_t buckets;
  size_t units_used;
  size_t unit_key;
  size_t unit_value;
}
Mary_Hashmap_t;

typedef void *Mary_Hashmap_Key_t;

typedef void *Mary_Hashmap_Value_t;

typedef struct
{
  Mary_Hashmap_Key_t key;
  Mary_Hashmap_Value_t value;
}
Mary_Hashmap_Entry_t;

void Mary_Hashmap_Create(Mary_Hashmap_t *hashmap, size_t unit_key, size_t unit_value);
void Mary_Hashmap_Destroy(Mary_Hashmap_t *hashmap);
void Mary_Hashmap_Assign(Mary_Hashmap_t *hashmap, void *in_key, void *in_value);
char Mary_Hashmap_At(Mary_Hashmap_t *hashmap, void *in_key, void *out_value); // return Mary_Bool_t
void *Mary_Hashmap_Point(Mary_Hashmap_t *hashmap, void *in_key);
char Mary_Hashmap_Contains_Key(Mary_Hashmap_t *hashmap, void *in_key); // shouldn't this be Has_Key???? also return Mary_Bool_t
void Mary_Hashmap_Erase(Mary_Hashmap_t *hashmap, void *in_key);
void Mary_Hashmap_Empty(Mary_Hashmap_t *hashmap);
void Mary_Hashmap_Keys(Mary_Hashmap_t *hashmap, Mary_Vector_t *out_vector, uint8_t is_created);
void Mary_Hashmap_Values(Mary_Hashmap_t *hashmap, Mary_Vector_t *out_vector, uint8_t is_created);
void Mary_Hashmap_Entries(Mary_Hashmap_t *hashmap, Mary_Vector_t *out_vector, uint8_t is_created);
