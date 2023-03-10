// Copyright 2021 Teodor Tiron
#ifndef DOUBLE_CIRCULAR_LINKED_LIST_H_
#define DOUBLE_CIRCULAR_LINKED_LIST_H_

typedef struct dll_node_t dll_node_t;
struct dll_node_t
{
    void* data;
    dll_node_t *prev, *next;
};

typedef struct doubly_linked_list_t doubly_linked_list_t;
struct doubly_linked_list_t
{
    dll_node_t* head;
    int data_size;
    int size;
};

doubly_linked_list_t*
dll_create(unsigned int data_size);

dll_node_t*
dll_get_nth_node(doubly_linked_list_t* list, int n);

void
dll_add_nth_node(doubly_linked_list_t* list, int n, const void* data);

dll_node_t*
dll_remove_nth_node(doubly_linked_list_t* list, int n);

void
dll_free(doubly_linked_list_t** pp_list);

void
dll_print_int_list(doubly_linked_list_t* list);

#endif  // DOUBLE_CIRCULAR_LINKED_LIST_H_
