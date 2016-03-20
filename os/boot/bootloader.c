#include "rpsio.h"

extern void putchar(unsigned int c);

extern void init(void);

int main (void)
{
	init();

	printf("hello world\n");
	printf("hi %s at %d\n", "there", 1406);

	return 0;
}
