// Copyright 2021 Teodor Tiron
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "double_circular_linked_list.h"

#define MAX_NAME_LENGTH 21
#define MAX_CMD_LENGTH 100

typedef struct planet_t planet_t;
struct planet_t
{
    char name[MAX_NAME_LENGTH];
    int kills;
    doubly_linked_list_t* shield_list;
};

/* Adds and initialises a node that represents a planet on the nth position */
void
add_planet(doubly_linked_list_t* planet_list, int n, char* name, int sh_count)
{
	if (n > planet_list->size) {
		printf("Planet out of bounds!\n");
		return;
	}
	planet_t planet;
	strncpy(planet.name, name, MAX_NAME_LENGTH);
	planet.kills = 0;
	planet.shield_list = dll_create(sizeof(int));
	int sh_val = 1;
	for (int i = 0; i < sh_count; i++)
		dll_add_nth_node(planet.shield_list, i, &sh_val);
	dll_add_nth_node(planet_list, n, &planet);
	printf("The planet %s has joined the galaxy.\n", name);
}

/* Removes the nth planet from the list and displays a message:
if msg is 0, there is no message (used when freeing memory)
if msg is 1, the message is the one triggered by BLH
if msg is 2, the message is the one triggered by a fatal collision */
void
remove_planet(doubly_linked_list_t* planet_list, int n, int msg)
{
	if (n >= planet_list->size && msg != 0) {
		printf("Planet out of bounds!\n");
		return;
	}
	dll_node_t* tbd;
	tbd = dll_remove_nth_node(planet_list, n);
	doubly_linked_list_t* shield_list;
	shield_list = ((planet_t*)(tbd->data))->shield_list;
	dll_free(&shield_list);
	if (msg == 1)
		printf("The planet %s has been eaten by the vortex.\n",
		((planet_t*)(tbd->data))->name);
	if (msg == 2)
		printf("The planet %s has imploded.\n",
		((planet_t*)(tbd->data))->name);
	free(tbd->data);
	free(tbd);
}

/* Adds a node to the end of the shield list of the nth planet */
void
add_shield(doubly_linked_list_t* planet_list, int n, int val)
{
	if (n >= planet_list->size) {
		printf("Planet out of bounds!\n");
		return;
	}
	dll_node_t* node;
	node = dll_get_nth_node(planet_list, n);
	doubly_linked_list_t* shield_list;
	shield_list = ((planet_t*)(node->data))->shield_list;
	dll_add_nth_node(shield_list, shield_list->size, &val);
}

/* Increases the value of the mth node of the shield list of the nth planet */
void
upgrade_shield(doubly_linked_list_t* planet_list, int n, int m, int upg)
{
	if (n >= planet_list->size) {
		printf("Planet out of bounds!\n");
		return;
	}
	dll_node_t* node;
	node = dll_get_nth_node(planet_list, n);
	doubly_linked_list_t* shield_list;
	shield_list = ((planet_t*)(node->data))->shield_list;
	if (m >= shield_list->size) {
		printf("Shield out of bounds!\n");
		return;
	}
	node = dll_get_nth_node(shield_list, m);
	*(int *)(node->data) += upg;
}

/* Removes the mth node of the shield list of the nth planet */
void
remove_shield(doubly_linked_list_t* planet_list, int n, int m){
	if (n >= planet_list->size) {
		printf("Planet out of bounds!\n");
		return;
	}
	dll_node_t* node;
	node = dll_get_nth_node(planet_list, n);
	doubly_linked_list_t* shield_list;
	shield_list = ((planet_t*)(node->data))->shield_list;
	if (m >= shield_list->size) {
		printf("Shield out of bounds!\n");
		return;
	}
	if (shield_list->size == 4) {
		printf("A planet cannot have less than 4 shields!\n");
		return;
	}
	node = dll_remove_nth_node(shield_list, m);
	free(node->data);
	free(node);
}

/* Moves the head of the shield list of the nth planet */
void
rotate_planet(doubly_linked_list_t* planet_list, int n, char dir, int unit)
{
	if (n >= planet_list->size) {
		printf("Planet out of bounds!\n");
		return;
	}
	if (dir != 'c' && dir != 't') {
		printf("Not a valid direction!\n");
		return;
	}
	dll_node_t* node;
	node = dll_get_nth_node(planet_list, n);
	doubly_linked_list_t* shield_list;
	shield_list = ((planet_t*)(node->data))->shield_list;
	int rot;
	rot = unit % shield_list->size;
	dll_node_t* aux;
	aux = shield_list->head;
	int i;
	if (dir == 'c') {
		for (i = 0; i < rot; i++)
			aux = aux->prev;
	}
	if (dir == 't') {
		for (i = 0; i < rot; i++)
			aux = aux->next;
	}
	shield_list->head = aux;
}

/* Decreases the value of each colliding shield by 1.
If a shield value drops below 0, the planet is removed */
void
collide(doubly_linked_list_t* planet_list, int n1, int n2)
{
	if (n1 >= planet_list->size || n2 >= planet_list->size) {
		printf("Planet out of bounds!\n");
		return;
	}
	dll_node_t* node1;
	node1 = dll_get_nth_node(planet_list, n1);
	doubly_linked_list_t* shield_list1;
	shield_list1 = ((planet_t*)(node1->data))->shield_list;
	dll_node_t* s_node1;
	s_node1 = dll_get_nth_node(shield_list1, shield_list1->size / 4);

	dll_node_t* node2;
	node2 = dll_get_nth_node(planet_list, n2);
	doubly_linked_list_t* shield_list2;
	shield_list2 = ((planet_t*)(node2->data))->shield_list;
	dll_node_t* s_node2;
	s_node2 = dll_get_nth_node(shield_list2, (shield_list2->size / 4) * 3);

	*(int *)(s_node1->data) -= 1;
	*(int *)(s_node2->data) -= 1;

	if (*(int *)(s_node1->data) == -1 && *(int *)(s_node2->data) == -1) {
		remove_planet(planet_list, n1, 2);
		if (n2 == 0) {
			remove_planet(planet_list, 0, 2);
		} else {
			remove_planet(planet_list, n1, 2);
		}
		return;
	}
	if (*(int *)(s_node1->data) == -1) {
		remove_planet(planet_list, n1, 2);
		((planet_t*)(node2->data))->kills++;
		return;
	}
	if (*(int *)(s_node2->data) == -1) {
		remove_planet(planet_list, n2, 2);
		((planet_t*)(node1->data))->kills++;
		return;
	}
}

void
free_galaxy(doubly_linked_list_t** planet_list)
{
	while ((*planet_list)->size > 0) {
		remove_planet(*planet_list, 0, 0);
	}
	free(*planet_list);
	*planet_list = NULL;
}

void
show_planet(doubly_linked_list_t* planet_list, int n)
{
	if (n >= planet_list->size) {
		printf("Planet out of bounds!\n");
		return;
	}
	dll_node_t* node;
	node = dll_get_nth_node(planet_list, n);
	printf("NAME: %s\n", ((planet_t*)(node->data))->name);
	printf("CLOSEST: ");
	if (planet_list->size == 1)
		printf("none\n");
	if (planet_list->size == 2)
		printf("%s\n", ((planet_t*)(node->next->data))->name);
	if (planet_list->size > 2)
		printf("%s and %s\n", ((planet_t*)(node->prev->data))->name,
		((planet_t*)(node->next->data))->name);
	printf("SHIELDS: ");
	dll_print_int_list(((planet_t*)(node->data))->shield_list);
	printf("KILLED: %d\n", ((planet_t*)(node->data))->kills);
}

int main()
{
	int count;
	doubly_linked_list_t* planet_list;
	char cmd[MAX_CMD_LENGTH], act[4];
	planet_list = dll_create(sizeof(planet_t));
	scanf("%d\n", &count);
	for (int i = 0; i < count; i++) {
		fgets(cmd, MAX_CMD_LENGTH, stdin);
		sscanf(cmd, "%s", act);
		if (strncmp(act, "ADD", 3) == 0) {
			char name[MAX_NAME_LENGTH];
			int n, sh_count;
			sscanf(cmd, "%*s %s %d %d", name, &n, &sh_count);
			add_planet(planet_list, n, name, sh_count);
		}
		if (strncmp(act, "BLH", 3) == 0) {
			int n;
			sscanf(cmd, "%*s %d", &n);
			remove_planet(planet_list, n, 1);
		}
		if (strncmp(act, "UPG", 3) == 0) {
			int n, m, upg;
			sscanf(cmd, "%*s %d %d %d", &n, &m, &upg);
			upgrade_shield(planet_list, n, m, upg);
		}
		if (strncmp(act, "EXP", 3) == 0) {
			int n, val;
			sscanf(cmd, "%*s %d %d", &n, &val);
			add_shield(planet_list, n, val);
		}
		if (strncmp(act, "RMV", 3) == 0) {
			int n, m;
			sscanf(cmd, "%*s %d %d", &n, &m);
			remove_shield(planet_list, n, m);
		}
		if (strncmp(act, "COL", 3) == 0) {
			int n1, n2;
			sscanf(cmd, "%*s %d %d", &n1, &n2);
			collide(planet_list, n1, n2);
		}
		if (strncmp(act, "ROT", 3) == 0) {
			int n, unit;
			char dir;
			sscanf(cmd, "%*s %d %c %d", &n, &dir, &unit);
			rotate_planet(planet_list, n, dir, unit);
		}
		if (strncmp(act, "SHW", 3) == 0) {
			int n;
			sscanf(cmd, "%*s %d", &n);
			show_planet(planet_list, n);
		}
	}
	free_galaxy(&planet_list);
	return 0;
}
