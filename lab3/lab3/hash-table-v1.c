#include "hash-table-base.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <sys/queue.h>

#include <pthread.h>

struct list_entry
{
	const char *key;
	uint32_t value;
	SLIST_ENTRY(list_entry)
	pointers;
};

SLIST_HEAD(list_head, list_entry); // defines list_head struct

struct hash_table_entry
{
	struct list_head list_head;
};

struct hash_table_v1
{
	struct hash_table_entry entries[HASH_TABLE_CAPACITY];
};

struct hash_table_v1 *hash_table_v1_create()
{
	struct hash_table_v1 *hash_table = calloc(1, sizeof(struct hash_table_v1));
	assert(hash_table != NULL);
	// make each entry in the hash table a list head
	for (size_t i = 0; i < HASH_TABLE_CAPACITY; ++i)
	{
		struct hash_table_entry *entry = &hash_table->entries[i]; // get address of each hash table entry
		SLIST_INIT(&entry->list_head);
	}
	return hash_table;
}

static struct hash_table_entry *get_hash_table_entry(struct hash_table_v1 *hash_table,
													 const char *key)
{
	assert(key != NULL);
	uint32_t index = bernstein_hash(key) % HASH_TABLE_CAPACITY;
	struct hash_table_entry *entry = &hash_table->entries[index];
	return entry; // returns head of LL
}

static struct list_entry *get_list_entry(struct hash_table_v1 *hash_table,
										 const char *key,
										 struct list_head *list_head)
{
	assert(key != NULL);

	struct list_entry *entry = NULL;

	SLIST_FOREACH(entry, list_head, pointers)
	{
		if (strcmp(entry->key, key) == 0)
		{
			return entry; // returns node with the correct key value
		}
	}
	return NULL;
}

bool hash_table_v1_contains(struct hash_table_v1 *hash_table,
							const char *key)
{
	struct hash_table_entry *hash_table_entry = get_hash_table_entry(hash_table, key);
	struct list_head *list_head = &hash_table_entry->list_head;
	struct list_entry *list_entry = get_list_entry(hash_table, key, list_head);
	return list_entry != NULL;
}

pthread_mutex_t entry_mutex;

void hash_table_v1_add_entry(struct hash_table_v1 *hash_table,
							 const char *key,
							 uint32_t value)
{
	if (pthread_mutex_lock(&entry_mutex) != 0)
	{
		perror("mutex locking failed.");
		exit(1);
	}

	struct hash_table_entry *hash_table_entry = get_hash_table_entry(hash_table, key);
	struct list_head *list_head = &hash_table_entry->list_head;
	struct list_entry *list_entry = get_list_entry(hash_table, key, list_head);

	/* Update the value if it already exists */
	if (list_entry != NULL)
	{
		list_entry->value = value;
		return;
	}

	list_entry = calloc(1, sizeof(struct list_entry));
	list_entry->key = key;
	list_entry->value = value;
	SLIST_INSERT_HEAD(list_head, list_entry, pointers);

	if (pthread_mutex_unlock(&entry_mutex) != 0)
	{
		perror("mutex unlocking failed.");
		exit(1);
	}
}

uint32_t hash_table_v1_get_value(struct hash_table_v1 *hash_table,
								 const char *key)
{
	struct hash_table_entry *hash_table_entry = get_hash_table_entry(hash_table, key);
	struct list_head *list_head = &hash_table_entry->list_head;
	struct list_entry *list_entry = get_list_entry(hash_table, key, list_head);
	assert(list_entry != NULL);
	return list_entry->value;
}

void hash_table_v1_destroy(struct hash_table_v1 *hash_table)
{
	for (size_t i = 0; i < HASH_TABLE_CAPACITY; ++i)
	{
		struct hash_table_entry *entry = &hash_table->entries[i];
		struct list_head *list_head = &entry->list_head;
		struct list_entry *list_entry = NULL;
		while (!SLIST_EMPTY(list_head))
		{
			list_entry = SLIST_FIRST(list_head);
			SLIST_REMOVE_HEAD(list_head, pointers);
			free(list_entry);
		}
	}
	free(hash_table);
}
