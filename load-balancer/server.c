/* Copyright 2021 <> */
#include <stdlib.h>
#include <string.h>

#include "server.h"

server_memory* init_server_memory() {
	server_memory* server_mem;
	server_mem = malloc(sizeof(*server_mem));
	DIE(server_mem == NULL, "server_memory malloc");
	server_mem->ht = ht_create(MAX_BUCKETS,
	hash_function_string, compare_function_strings);
	return server_mem;
}

void server_store(server_memory* server, char* key, char* value) {
	ht_put(server->ht, key, strlen(key) + 1, value, strlen(value) + 1);
}

void server_remove(server_memory* server, char* key) {
	ht_remove_entry(server->ht, key);
}

char* server_retrieve(server_memory* server, char* key) {
	return (char*)ht_get(server->ht, key);
}

void free_server_memory(server_memory* server) {
	ht_free(server->ht);
	free(server);
}
