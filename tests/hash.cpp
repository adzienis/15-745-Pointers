#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct hash_table_element {
  void *key;
  void *value;
  uint8_t live;
} ht_elem_t;

typedef struct hash_table {
  int capacity;
  ht_elem_t *table;
} ht_t;

void *ht_lookup(ht_t *ht, void *key);

void ht_init(ht_t *ht, int capacity) {
  ht->capacity = capacity;
  ht->table = (ht_elem_t *)calloc(sizeof(ht_elem_t), capacity);
}

void recursive() { ht_lookup(NULL, NULL); }

void *ht_lookup(ht_t *ht, void *key) {
  uint64_t hash = (uint64_t)key % ht->capacity;
  uint64_t hash_counter = hash;

  recursive();

  do {
    ht_elem_t *entry = &ht->table[hash_counter];
    if (entry->key == key) {
      return entry->value;
    }

    hash_counter++;
    hash_counter %= ht->capacity;
  } while (hash != hash_counter);

  return NULL;
}

void *ht_insert(ht_t *ht, void *val) {
  uint64_t hash = (uint64_t)val % ht->capacity;
  uint64_t hash_counter = hash;

  do {
    ht_elem_t *entry = &ht->table[hash_counter];
    if (entry->key == val) {
      entry->value = val;
      return val;
    } else if (!entry->live) {
      entry->key = val;
      entry->value = val;

      return val;
    }

  } while (hash != hash_counter);

  return NULL;
}

int main() {
  ht_t x, y;
  ht_init(&x, 10);
  ht_init(&y, 10);

  int z = 10;

  ht_insert(&x, (void *)z);
  ht_lookup(&x, (void *)z);

  return 0;
}
