#include "list.h"
#include <stdlib.h>                   
#include <stdio.h>
                            
int
main(void)
{
	printf("gets here \n");

	struct linked_list* list = ll_create();
	printf("contains 5: %d contains 6: %d \n", ll_contains(list, 5), ll_contains(list, 6));
	if(list -> head == NULL){
		printf("head is null\n" );
	}
	printf("length %d \n", ll_length(list));
	ll_add(list, 6);
	ll_add(list, 3);
	ll_add(list, 4);
	printf("length %d, value %d \n", ll_length(list), list->head->value);
	printf("contains 5: %d contains 6: %d \n", ll_contains(list, 5), ll_contains(list, 6));
	ll_remove_first(list);
	printf("length %d, value %d \n", ll_length(list), list->head->value);
	printf("contains 5: %d contains 6: %d \n", ll_contains(list, 5), ll_contains(list, 6));
	ll_remove_first(list);
	printf("length %d, value %d \n", ll_length(list), list->head->value);
	printf("contains 5: %d contains 6: %d \n", ll_contains(list, 5), ll_contains(list, 6));
	ll_add(list, 5);
	printf("length %d, value %d \n", ll_length(list), list->head->value);
	printf("contains 5: %d contains 6: %d \n", ll_contains(list, 5), ll_contains(list, 6));
	printf("destory list: %d \n",ll_destroy(list));
	ll_remove_first(list);
	ll_remove_first(list);
	if(list -> head == NULL){
		printf("head is null\n" );
	}
	printf("-----checking adding after doing several adding and removals----\n"); 
	ll_add(list, -2);
	ll_add(list, 3);
	ll_add(list, 0);
	printf("length %d, value %d \n", ll_length(list), list->head->value);
	printf("contains -2: %d contains 6: %d \n", ll_contains(list, -2), ll_contains(list, 6));
	ll_remove_first(list);
	ll_remove_first(list); 
	printf("length %d, value %d \n", ll_length(list), list->head->value);
	printf("contains -2: %d contains 6: %d \n", ll_contains(list, -2), ll_contains(list, 6));
	ll_remove_first(list);
	if(list -> head == NULL){
		printf("head is null\n" );
	}
	printf("destory list: %d \n",ll_destroy(list));
	
	return 0;
}
