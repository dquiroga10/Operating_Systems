#include <stdlib.h>
#include <stdbool.h>

#ifndef LIST_H
#define LIST_H

typedef struct object
{
	int value;
	struct object *prev;

} object;

typedef struct linked_list
{
	int length;
	struct object *head;
} linked_list;



static inline struct linked_list *
ll_create(void)
{
	object *node = NULL;
	linked_list *list;
	list = (linked_list *)malloc(sizeof(linked_list));
	list -> head = node;
	list -> length = 0;
	return list;
}

static inline int
ll_destroy(struct linked_list *ll)
{
	if(ll->head == NULL)
	{
		free(ll);
		return 1;
	}
	return 0;
}

static inline void
ll_add(struct linked_list *ll, int value)
{
	object *node;
	node = (object *)malloc(sizeof(object));
	node -> value = value;
	node -> prev = ll -> head;
	ll -> head = node;
	int len = ll -> length; 
	ll -> length = ++len;
}

static inline int
ll_length(struct linked_list *ll)
{
	return ll->length;
}

static inline bool
ll_remove_first(struct linked_list *ll)
{
	if (ll->head == NULL){
		return false;
	}
	object *node;
	node = ll->head;
	int len = ll -> length;
	ll -> head = node -> prev; 
	ll -> length = --len;
	free(node);
	return true;
}

static inline int
ll_contains(struct linked_list *ll, int value)
{
	object *search;
	search = ll->head;
	int index = 1;
	while (search != NULL){
		if(search->value == value)
			return index;
		search = search -> prev;
		index++;
	}
	return 0;
}

#endif
