#include <stdlib.h>                   
#include <stdio.h>

int main(void)
{
	// in order to see the stack, we would create variables so that the pointer would be moved as we initialize the variables
	int a = 10;
	int b = 20;
	int c = 30;

	printf("Address of a: %p with value: %d\n", &a, a);
	printf("Address of b: %p with value: %d\n", &b, b);
	printf("Address of c: %p with value: %d\n", &c, c);


	return 0;
}