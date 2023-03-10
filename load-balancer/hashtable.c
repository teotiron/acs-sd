/* Copyright 2021 Teodor Tiron */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hashtable.h"

int
compare_function_uints(void *a, void *b)
{
	unsigned int uint_a = *((unsigned int *)a);
	unsigned int uint_b = *((unsigned int *)b);

	if (uint_a == uint_b) {
		return 0;
	} else if (uint_a < uint_b) {
		return -1;
	} else {
		return 1;
	}
}

unsigned int
hash_function_string(void *a)
{
	unsigned char *puchar_a = (unsigned char*) a;
	unsigned long hash = 5381;
	int c;

	while ((c = *puchar_a++))
		hash = ((hash << 5u) + hash) + c;

	return hash;
}

int
compare_function_strings(void *a, void *b)
{
	char *str_a = (char *)a;
	char *str_b = (char *)b;

	return strcmp(str_a, str_b);
}

hashtable_t*
ht_create(unsigned int hmax, unsigned int (*hash_function)(void*),
		int (*compare_function)(void*, void*))
{
	hashtable_t *htable;
	htable = malloc(sizeof(hashtable_t));
	htable->buckets = malloc(hmax * sizeof(linked_list_t *));
	for (unsigned int i = 0; i < hmax; i++)
		htable->buckets[i] = ll_create(sizeof(struct info));
	htable->size = 0;
	htable->hmax = hmax;
	htable->hash_function = hash_function;
	htable->compare_function = compare_function;
	return htable;
}

void
ht_put(hashtable_t *ht, void *key, unsigned int key_size,
	void *value, unsigned int value_size)
{
	unsigned int hash;
	hash = ht->hash_function(key);
	hash = hash % ht->hmax;
	struct info tba;
	ll_node_t *aux = ht->buckets[hash]->head;
	while (aux != NULL) {
		if (ht->compare_function(((struct info *)(aux->data))->key, key) == 0) {
			((struct info *)(aux->data))->value =
			realloc(((struct info *)(aux->data))->value, value_size);
			memcpy(((struct info *)(aux->data))->value, value, value_size);
			return;
		}
		aux = aux->next;
	}
	tba.key = malloc(key_size);
	memcpy(tba.key, key, key_size);
	tba.value = malloc(value_size);
	memcpy(tba.value, value, value_size);
	ll_add_nth_node(ht->buckets[hash], 0, &tba);
	ht->size++;
}

void *
ht_get(hashtable_t *ht, void *key)
{
	unsigned int hash;
	hash = ht->hash_function(key);
	hash = hash % ht->hmax;
	ll_node_t *aux = ht->buckets[hash]->head;
	while (aux != NULL &&
	ht->compare_function(((struct info *)(aux->data))->key, key) != 0)
		aux = aux->next;
	if (aux == NULL)
		return NULL;
	if (ht->compare_function(((struct info *)(aux->data))->key, key) == 0)
		return ((struct info *)(aux->data))->value;
	return NULL;
}

int
ht_has_key(hashtable_t *ht, void *key)
{
	unsigned int hash;
	hash = ht->hash_function(key);
	hash = hash % ht->hmax;
	ll_node_t *aux = ht->buckets[hash]->head;
	while (aux != NULL &&
	ht->compare_function(((struct info *)(aux->data))->key, key) != 0)
		aux = aux->next;
	if (aux == NULL)
		return 0;
	if (ht->compare_function(((struct info *)(aux->data))->key, key) == 0)
		return 1;
	return 0;
}

void
ht_remove_entry(hashtable_t *ht, void *key)
{
	int pos = 0;
	unsigned int hash;
	hash = ht->hash_function(key);
	hash = hash % ht->hmax;
	ll_node_t *aux = ht->buckets[hash]->head;
	while (aux != NULL &&
	ht->compare_function(((struct info *)(aux->data))->key, key) != 0) {
		pos++;
		aux = aux->next;
	}
	if (aux == NULL)
		return;
	if (ht->compare_function(((struct info *)(aux->data))->key, key) == 0) {
		ll_node_t *tbd;
		tbd = ll_remove_nth_node(ht->buckets[hash], pos);
		free(((struct info *)(tbd->data))->key);
		free(((struct info *)(tbd->data))->value);
		free(tbd->data);
		free(tbd);
	}
}

void
ht_free(hashtable_t *ht)
{
	for (unsigned int i = 0; i < ht->hmax; i++) {
		for (ll_node_t* aux = ht->buckets[i]->head; aux != NULL; aux = aux->next) {
			free(((struct info*)(aux->data))->key);
			free(((struct info*)(aux->data))->value);
		}
		ll_free(&(ht->buckets[i]));
	}
	free(ht->buckets);
	free(ht);
}

unsigned int
ht_get_size(hashtable_t *ht)
{
	if (ht == NULL)
		return 0;

	return ht->size;
}

unsigned int
ht_get_hmax(hashtable_t *ht)
{
	if (ht == NULL)
		return 0;

	return ht->hmax;
}
