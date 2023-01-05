#include "table.h"
#include "memory.h"
#include "object.h"
#include <string.h>

#define TABLE_MAX_LOAD 0.75

void initTable(Table *table) {
  table->count = 0;
  table->capacity = 0;
  table->entries = NULL;
}

void freeTable(Table *table) {
  FREE_ARRAY(Entry, table->entries, table->capacity);
  initTable(table);
}

static Entry *findEntry(Entry *entries, int capacity, ObjString *key) {
  uint32_t index = key->hash % capacity;
  Entry *tombstone = NULL;
  for (;;) {
    Entry *entry = &entries[index];
    if (entry->key == NULL) {
      if (IS_NIL(entry->value)) {
        // we found an empty bucket, but if tombstone appears in probe sequence,
        // reuse it.
        return tombstone != NULL ? tombstone : entry;
      } else {
        // we found a tombstone.
        // we will reuse the first one in the sequence, by preventing tombstone
        // overridden by the past tombstones

        //[v] [v] [T] [v] [T] [] <=empty
        //        ^       ^
        // reuse this, not this
        // because the former is faster for later operations
        if (tombstone == NULL)
          tombstone = entry;
      }
    } else if (entry->key == key) { // pointer equality is string equality
                                    // because all strings are interned
      return entry;
    }

    index = (index + 1) % capacity;
  }
}

bool tableGet(Table *table, ObjString *key, Value *value) {
  if (table->count == 0)
    return false;

  Entry *entry = findEntry(table->entries, table->capacity, key);
  if (entry->key == NULL)
    return false;
  *value = entry->value;
  return true;
}

static void adjustCapacity(Table *table, int capacity) {
  Entry *entries = ALLOCATE(Entry, capacity);
  for (int i = 0; i < capacity; i++) {
    // NULL initialize, as new hash table should be recalculated with respect to
    // its NEW SIZE
    entries[i].key = NULL;
    entries[i].value = NIL_VAL;
  }

  table->count = 0; // new count is not the same because we don't copy tombstone
  for (int i = 0; i < table->capacity; i++) {
    Entry *entry = &table->entries[i];
    if (entry->key == NULL)
      continue;

    Entry *dest = findEntry(entries, capacity, entry->key);
    dest->key = entry->key;
    dest->value = entry->value;
    table->count++;
  }

  FREE_ARRAY(Entry, table->entries, table->capacity); // free old array
  table->entries = entries;
  table->capacity = capacity;
}

bool tableSet(Table *table, ObjString *key, Value value) {

  if (table->count + 1 > table->capacity * TABLE_MAX_LOAD) {
    int capacity = GROW_CAPACITY(table->capacity);
    adjustCapacity(table, capacity);
  }

  Entry *entry = findEntry(table->entries, table->capacity, key);
  bool isNewKey = entry->key == NULL;
  if (isNewKey && IS_NIL(entry->value))
    table->count++;

  entry->key = key;
  entry->value = value;
  return isNewKey;
}

bool tableDelete(Table *table, ObjString *key) {
  if (table->count == 0)
    return false;

  Entry *entry = findEntry(table->entries, table->capacity, key);
  if (entry->key == NULL)
    return false;

  entry->key = NULL;
  entry->value = BOOL_VAL(
      true); // (any non-nil val) represents tombstone. Without tombstone
             // findEntry's probe sequence breaks at tombstone
  return true;
}

void tableAddAll(Table *from, Table *to) {
  for (int i = 0; i < from->capacity; i++) {
    Entry *entry = &from->entries[i];
    if (entry->key != NULL) {
      tableSet(to, entry->key, entry->value);
    }
  }
}

ObjString *tableFindString(Table *table, const char *chars, int length,
                           uint32_t hash) {
  if (table->count == 0)
    return NULL;

  uint32_t index = hash % table->capacity;
  for (;;) {
    Entry *entry = &table->entries[index];
    if (entry->key == NULL) {
      if (IS_NIL(entry->value))
        return NULL;
    } else if (entry->key->length == length && entry->key->hash == hash &&
               memcmp(entry->key->chars, chars, length) == 0) {
      // we found it
      return entry->key;
    }

    index = (index + 1) % table->capacity;
  }
}
