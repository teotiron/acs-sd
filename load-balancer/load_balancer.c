/* Copyright 2021 Teodor Tiron */
#include <stdlib.h>
#include <string.h>

#include "load_balancer.h"

#define MAX_TAGS 100000
#define MAX_KEYS 1000

struct server_data {
    unsigned int tag;
    unsigned int hash;
};

struct load_balancer {
    int server_nr;
	server_data* server_tags;
    server_memory** server_list;
};

unsigned int hash_function_servers(void *a) {
    unsigned int uint_a = *((unsigned int *)a);

    uint_a = ((uint_a >> 16u) ^ uint_a) * 0x45d9f3b;
    uint_a = ((uint_a >> 16u) ^ uint_a) * 0x45d9f3b;
    uint_a = (uint_a >> 16u) ^ uint_a;
    return uint_a;
}

unsigned int hash_function_key(void *a) {
    unsigned char *puchar_a = (unsigned char *) a;
    unsigned int hash = 5381;
    int c;

    while ((c = *puchar_a++))
        hash = ((hash << 5u) + hash) + c;

    return hash;
}

load_balancer* init_load_balancer() {
	load_balancer* lb;
    lb = malloc(sizeof(load_balancer));
    DIE(lb == NULL, "lb malloc");
    lb->server_tags = calloc(MAX_TAGS, sizeof(server_data));
    DIE(lb->server_tags == NULL, "lb->server_tags malloc");
    lb->server_list = calloc(MAX_TAGS, sizeof(server_memory*));
    DIE(lb->server_list == NULL, "lb->server_list malloc");
    lb->server_nr = 0;
    return lb;
}

void loader_store(load_balancer* main, char* key, char* value, int* server_id) {
	unsigned int hash = hash_function_key(key);
    if (hash < main->server_tags[0].hash ||
    hash > main->server_tags[main->server_nr - 1].hash) {
        *server_id = main->server_tags[0].tag % 100000;
    } else {
        for (int i = 1; i <= main->server_nr - 1; i++)
            if (hash > main->server_tags[i - 1].hash &&
            hash < main->server_tags[i].hash) {
                *server_id = main->server_tags[i].tag % 100000;
                break;
            }
    }
    server_store(main->server_list[*server_id], key, value);
}


char* loader_retrieve(load_balancer* main, char* key, int* server_id) {
	unsigned int hash = hash_function_key(key);
    if (hash < main->server_tags[0].hash ||
    hash > main->server_tags[main->server_nr - 1].hash) {
        *server_id = main->server_tags[0].tag % 100000;
    } else {
        for (int i = 1; i <= main->server_nr - 1; i++)
            if (hash > main->server_tags[i - 1].hash &&
            hash < main->server_tags[i].hash) {
                *server_id = main->server_tags[i].tag % 100000;
                break;
            }
    }
	return server_retrieve(main->server_list[*server_id], key);
}

void loader_add_server(load_balancer* main, int server_id) {
    main->server_list[server_id] = init_server_memory();
    char **key_list;
    key_list = calloc(MAX_KEYS, sizeof(char*));
    for (int i = 0; i < MAX_KEYS; i++)
        key_list[i] = malloc(KEY_LENGTH);
    int key_count = 0;
    int *orig_server;
    orig_server = calloc(MAX_KEYS, sizeof(int));
	for (int k = 0; k < 3; k++) {
        int tag = server_id + k * 100000;
        unsigned int hash = hash_function_servers(&tag);
        int i;
        for (i = main->server_nr - 1; i >= 0 &&
        main->server_tags[i].hash > hash; i--) {
            main->server_tags[i + 1].hash = main->server_tags[i].hash;
            main->server_tags[i + 1].tag = main->server_tags[i].tag;
        }
        main->server_tags[i + 1].hash = hash;
        main->server_tags[i + 1].tag = tag;
        main->server_nr++;
        if (main->server_nr > 1) {
        int donor = main->server_tags[(i + 2) % main->server_nr].tag % 100000;
        unsigned prev_hash;
        if ((i + 1) == 0) {
            prev_hash = main->server_tags[main->server_nr - 1].hash;
        } else {
            prev_hash = main->server_tags[i].hash;
        }
        server_memory* donor_server = main->server_list[donor];
        for (int j = 0; j < MAX_BUCKETS; j++) {
            if (donor_server->ht->buckets[j]->head != NULL) {
                ll_node_t* aux = donor_server->ht->buckets[j]->head;
                while (aux != NULL) {
                    if (prev_hash > hash) {
                        unsigned int key_hash =
                        hash_function_key(((struct info*)(aux->data))->key);
                        if (key_hash < hash || key_hash > prev_hash) {
                            strcpy(key_list[key_count],
                            ((struct info*)(aux->data))->key);
                            orig_server[key_count] = donor;
                            key_count++;
                        }
                    } else {
                        unsigned int key_hash =
                        hash_function_key(((struct info*)(aux->data))->key);
                        if (key_hash < hash && key_hash > prev_hash) {
                            strcpy(key_list[key_count],
                            ((struct info*)(aux->data))->key);
                            orig_server[key_count] = donor;
                            key_count++;
                        }
                    }
                    aux = aux->next;
                }
            }
        }
        }
    }
    for (int i = 0; i < key_count; i++) {
        int new_server = 0;
        loader_store(main, key_list[i],
        server_retrieve(main->server_list[orig_server[i]],
        key_list[i]), &new_server);
        server_remove(main->server_list[orig_server[i]], key_list[i]);
    }
    for (int i = 0; i < MAX_KEYS; i++)
            free(key_list[i]);
    free(key_list);
    free(orig_server);
}


void loader_remove_server(load_balancer* main, int server_id) {
    server_memory* tbd_server = main->server_list[server_id];
    char **key_list;
    key_list = calloc(MAX_KEYS, sizeof(char*));
    for (int i = 0; i < MAX_KEYS; i++)
        key_list[i] = malloc(KEY_LENGTH);
    int key_count = 0;
    for (int i = 0; i < MAX_BUCKETS; i++) {
        if (tbd_server->ht->buckets[i]->head != NULL) {
            ll_node_t* aux = tbd_server->ht->buckets[i]->head;
            while (aux != NULL) {
                strcpy(key_list[key_count], ((struct info*)(aux->data))->key);
                key_count++;
                aux = aux->next;
            }
        }
    }
	for (int k = 0; k < 3; k++) {
        int tag = server_id + k * 100000;
        unsigned int hash = hash_function_servers(&tag);
        int pos = 0;
        while (hash != main->server_tags[pos].hash)
            pos++;
        for (int i = pos; i < main->server_nr - 1; i++) {
            main->server_tags[i].hash = main->server_tags[i + 1].hash;
            main->server_tags[i].tag = main->server_tags[i + 1].tag;
        }
        main->server_nr--;
    }
    for (int i = 0; i < key_count; i++) {
        int new_server = 0;
        loader_store(main, key_list[i],
        server_retrieve(tbd_server, key_list[i]), &new_server);
        server_remove(tbd_server, key_list[i]);
    }
    for (int i = 0; i < MAX_KEYS; i++)
            free(key_list[i]);
    free(key_list);
    free_server_memory(main->server_list[server_id]);
    main->server_list[server_id] = NULL;
}

void free_load_balancer(load_balancer* main) {
    free(main->server_tags);
    for (int i = 0; i < MAX_TAGS; i++)
        if (main->server_list[i] != NULL)
            free_server_memory(main->server_list[i]);
    free(main->server_list);
    free(main);
}
