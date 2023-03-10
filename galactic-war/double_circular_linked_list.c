// Copyright 2021 Teodor Tiron
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "double_circular_linked_list.h"

doubly_linked_list_t*
dll_create(unsigned int data_size)
{
    doubly_linked_list_t* list;
    list = malloc(sizeof(doubly_linked_list_t));
    if (list == NULL) {
        printf("Memory error!\n");
        return NULL;
    }
    list->head = NULL;
    list->data_size = data_size;
    list->size = 0;
    return list;
}

/* Returns a pointer to the nth node */
dll_node_t*
dll_get_nth_node(doubly_linked_list_t* list, int n)
{
    if (n < 0)
        return NULL;
    if (list->size == 0)
        return NULL;
    int pos = n % list->size;
    dll_node_t* aux = list->head;
    for (int i = 0; i < pos; i++)
        aux = aux->next;
    return aux;
}

/* Adds a new node at the position n */
void
dll_add_nth_node(doubly_linked_list_t* list, int n, const void* data)
{
    int poz;
    if (n >= list->size) {
        poz = list->size;
    } else {
        poz = n;
    }
    if (n < 0) {
        printf("Invalid position\n");
        return;
    }
    if (poz == 0) {
        dll_node_t* new_node = malloc(sizeof(dll_node_t));
        new_node->data = malloc(list->data_size);
        memcpy(new_node->data, data, list->data_size);
        if (list->head == NULL) {
            new_node->next = new_node;
            new_node->prev = new_node;
            list->head = new_node;
            list->size++;
        } else {
            new_node->next = list->head;
            new_node->prev = list->head->prev;
            list->head->prev->next = new_node;
            list->head->prev = new_node;
            list->head = new_node;
            list->size++;
        }
    } else if (poz == list->size) {
        dll_node_t* new_node = malloc(sizeof(dll_node_t));
        new_node->data = malloc(list->data_size);
        memcpy(new_node->data, data, list->data_size);
        new_node->next = list->head;
        new_node->prev = list->head->prev;
        list->head->prev->next = new_node;
        list->head->prev = new_node;
        list->size++;
    } else {
        dll_node_t* aux = list->head;
        for (int i = 0; i < poz - 1; i++)
            aux = aux->next;
        dll_node_t* new_node = malloc(sizeof(dll_node_t));
        new_node->data = malloc(list->data_size);
        memcpy(new_node->data, data, list->data_size);
        new_node->next = aux->next;
        aux->next->prev = new_node;
        aux->next = new_node;
        new_node->prev = aux;
        list->size++;
    }
}

/* Removes node from list and returns a pointer to said node*/
dll_node_t*
dll_remove_nth_node(doubly_linked_list_t* list, int n)
{
    int poz;
    if (n >= list->size - 1) {
        poz = list->size - 1;
    } else {
        poz = n;
    }
    if (n < 0) {
        printf("Invalid position\n");
        return NULL;
    }
    if (poz == 0) {
        dll_node_t* aux = list->head;
        list->head->prev->next = list->head->next;
        list->head->next->prev = list->head->prev;
        list->head = list->head->next;
        list->size--;
        if (list->size == 0)
            list->head = NULL;
        return aux;
    } else if (poz == list->size - 1) {
        dll_node_t* aux = list->head->prev;
        aux->prev->next = list->head;
        list->head->prev = aux->prev;
        list->size--;
        return aux;
    } else {
        dll_node_t* aux = list->head;
        for (int i = 0; i < poz; i++)
            aux = aux->next;
        aux->next->prev = aux->prev;
        aux->prev->next = aux->next;
        list->size--;
        return aux;
    }
}

void
dll_free(doubly_linked_list_t** pp_list)
{
    dll_node_t* tbd;
    dll_node_t* aux;
    dll_node_t* final_node = (*pp_list)->head->prev;
    tbd = (*pp_list)->head;
    while (tbd != final_node) {
    	aux = tbd->next;
    	free(tbd->data);
    	free(tbd);
    	tbd = aux;
    }
    free(final_node->data);
    free(final_node);
    free(*pp_list);
    *pp_list = NULL;
}

/* Prints the value of each node by casting to int */
void
dll_print_int_list(doubly_linked_list_t* list)
{
    dll_node_t* aux = list->head;
    while (aux->next != list->head) {
        printf("%d ", (*(int *)aux->data));
        aux = aux->next;
    }
    printf("%d", (*(int *)aux->data));
    printf("\n");
}
