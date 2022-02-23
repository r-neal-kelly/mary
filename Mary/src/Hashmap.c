#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <Mary/Utils.h>
#include <Mary/Hashmap.h>
#include <Mary/Bitbool.h>

MARY_Primitives;

#define DEFAULT_BUCKETS 16 // this needs to be a power of 2
#define MAX_PERCENT 84 // this needs to be 100 or below

typedef u64 Used;
typedef struct { u64 idx; Used *used; u8 *key, *value; } Bucket; // just points to a real bucket in Buckets vector.

static u64 Hash_Key(void *in_key, size_t unit_key);
static void Hash_Bucket(Mary_Hashmap_t *hashmap, void *in_key, Bucket *out_bucket);
static void Rehash(Mary_Hashmap_t *hashmap, size_t bucket_units);

void Mary_Hashmap_Create(Mary_Hashmap_t *hashmap, size_t unit_key, size_t unit_value)
{
  u64 unit_bucket = MARY_Round_to_64bit(sizeof(Used) + unit_key + unit_value);
  Mary_Vector_Create(&hashmap->buckets, MARY_Allocator_Heap, unit_bucket, DEFAULT_BUCKETS); // this should have an allocator param instead.
  hashmap->buckets.units = DEFAULT_BUCKETS;
  hashmap->units_used = 0;
  hashmap->unit_key = unit_key;
  hashmap->unit_value = unit_value;
  Mary_Hashmap_Empty(hashmap);
}

void Mary_Hashmap_Destroy(Mary_Hashmap_t *hashmap)
{
  Mary_Vector_Destroy(&hashmap->buckets);
}

void Mary_Hashmap_Assign(Mary_Hashmap_t *hashmap, void *in_key, void *in_value)
{
  if (hashmap->units_used >= (MAX_PERCENT * hashmap->buckets.units / 100))
  {
    Rehash(hashmap, hashmap->buckets.units << 1); // * 2
  }
  Bucket bucket; Hash_Bucket(hashmap, in_key, &bucket);
  if (MARY_False(*bucket.used))
  {
    memcpy(bucket.key, in_key, hashmap->unit_key);
    memcpy(bucket.value, in_value, hashmap->unit_value);
    *bucket.used = MARY_TRUE;
    ++hashmap->units_used;
  }
  else
  {
    memcpy(bucket.value, in_value, hashmap->unit_value);
  }
}

char Mary_Hashmap_At(Mary_Hashmap_t *hashmap, void *in_key, void *out_value)
{
  Bucket bucket; Hash_Bucket(hashmap, in_key, &bucket);
  if (MARY_True(*bucket.used))
  {
    memcpy(out_value, bucket.value, hashmap->unit_value);
    return 1;
  }
  else
  {
    return 0;
  }
}

// this can't currently be used to set an item by ptr. it will be overwritten
// because it doesn't check if it's used. need to think about it.
// it should check if it needs to grow.
void *Mary_Hashmap_Point(Mary_Hashmap_t *hashmap, void *in_key)
{
  Bucket bucket; Hash_Bucket(hashmap, in_key, &bucket);
  return bucket.value;
}

char Mary_Hashmap_Contains_Key(Mary_Hashmap_t *hashmap, void *in_key)
{
  Bucket bucket; Hash_Bucket(hashmap, in_key, &bucket);
  return (char)*bucket.used;
}

void Mary_Hashmap_Erase(Mary_Hashmap_t *hashmap, void *in_key)
{
  Bucket bucket; Hash_Bucket(hashmap, in_key, &bucket);
  *bucket.used = MARY_FALSE;
}

void Mary_Hashmap_Empty(Mary_Hashmap_t *hashmap)
{
  MARY_Vector_Each(&hashmap->buckets, Used) *it.ptr = MARY_FALSE;
}

void Mary_Hashmap_Keys(Mary_Hashmap_t *hashmap, Mary_Vector_t *out_vector, uint8_t is_created)
{
  if (MARY_Falsey(is_created))
  {
    Mary_Vector_Create(out_vector, MARY_Allocator_Heap, sizeof(Mary_Hashmap_Key_t), hashmap->units_used);
  }
  Mary_Hashmap_Key_t key;
  MARY_Vector_Each(&hashmap->buckets, Used)
  {
    if (MARY_True(it.val))
    {
      key = (u8 *)it.ptr + sizeof(Used);
      MARY_Vector_Push(out_vector, Mary_Hashmap_Key_t, key);
    }
  }
}

void Mary_Hashmap_Values(Mary_Hashmap_t *hashmap, Mary_Vector_t *out_vector, uint8_t is_created)
{
  if (MARY_Falsey(is_created))
  {
    Mary_Vector_Create(out_vector, MARY_Allocator_Heap, sizeof(Mary_Hashmap_Value_t), hashmap->units_used);
  }
  Mary_Hashmap_Value_t value;
  MARY_Vector_Each(&hashmap->buckets, Used)
  {
    if (MARY_True(it.val))
    {
      value = (u8 *)it.ptr + sizeof(Used) + hashmap->unit_key;
      MARY_Vector_Push(out_vector, Mary_Hashmap_Value_t, value);
    }
  }
}

void Mary_Hashmap_Entries(Mary_Hashmap_t *hashmap, Mary_Vector_t *out_vector, uint8_t is_created)
{
  if (MARY_Falsey(is_created))
  {
    Mary_Vector_Create(out_vector, MARY_Allocator_Heap, sizeof(Mary_Hashmap_Entry_t), hashmap->units_used);
  }
  Mary_Hashmap_Entry_t entry;
  MARY_Vector_Each(&hashmap->buckets, Used)
  {
    if (MARY_True(it.val))
    {
      entry.key = (u8 *)it.ptr + sizeof(Used);
      entry.value = (u8 *)entry.key + hashmap->unit_key;
      MARY_Vector_Push(out_vector, Mary_Hashmap_Entry_t, entry);
    }
  }
}

static u64 Hash_Key(void *in_key, size_t unit_key)
{
  // FNV-1a
  u64 hash =  0xcbf29ce484222325;
  u64 prime = 0x00000100000001b3;
  u8 *p = (u8 *)in_key;
  for (u64 i = 0; i < unit_key; ++i, p += 1)
  {
    hash ^= *p;
    hash *= prime;
  }
  return hash;
}

static void Hash_Bucket(Mary_Hashmap_t *hashmap, void *in_key, Bucket *out_bucket)
{
  u64 last_idx = hashmap->buckets.units - 1;
  out_bucket->idx = Hash_Key(in_key, hashmap->unit_key) & (last_idx);  // no mod, we can & with units a ^ of 2
  out_bucket->used = MARY_Vector_Point(&hashmap->buckets, out_bucket->idx);
  out_bucket->key = (u8 *)out_bucket->used + sizeof(Used);
  out_bucket->value = out_bucket->key + hashmap->unit_key;

  while (1)
  {
    if (MARY_False(*out_bucket->used))
    {
      break;
    }
    else if (memcmp(out_bucket->key, in_key, hashmap->unit_key) == 0)
    {
      break;
    }
    else if (out_bucket->idx == last_idx)
    {
      out_bucket->idx = 0;
      out_bucket->used = hashmap->buckets.data;
      out_bucket->key = (u8 *)out_bucket->used + sizeof(Used);
      out_bucket->value = out_bucket->key + hashmap->unit_key;
    }
    else
    {
      ++out_bucket->idx;
      out_bucket->used += hashmap->buckets.unit >> 3; // works as long as we round bucket unit to 64bit
      out_bucket->key += hashmap->buckets.unit;
      out_bucket->value += hashmap->buckets.unit;
    }
  }
}

static void Rehash(Mary_Hashmap_t *hashmap, size_t bucket_units)
{
  Mary_Vector_t old_buckets = hashmap->buckets;
  Mary_Vector_Create(&hashmap->buckets, MARY_Allocator_Heap, hashmap->buckets.unit, bucket_units);
  hashmap->buckets.units = bucket_units;
  hashmap->units_used = 0;
  Mary_Hashmap_Empty(hashmap);

  u8 *old_key, *old_value; Bucket new_bucket;
  MARY_Vector_Each(&old_buckets, Used)
  {
    if (MARY_True(it.val))
    {
      old_key = (u8 *)it.ptr + sizeof(Used);
      old_value = old_key + hashmap->unit_key;
      Hash_Bucket(hashmap, old_key, &new_bucket);
      memcpy(new_bucket.key, old_key, hashmap->unit_key);
      memcpy(new_bucket.value, old_value, hashmap->unit_value);
      *new_bucket.used = MARY_TRUE;
      ++hashmap->units_used;
    }
  }

  Mary_Vector_Destroy(&old_buckets);
}
