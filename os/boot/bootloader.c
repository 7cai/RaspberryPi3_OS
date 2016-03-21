#include "rpsio.h"

extern void putchar(unsigned int);

extern void init(void);

extern void gpio_output_init(int);
extern void gpio_set(int);
extern void gpio_clr(int);

extern void sleep(unsigned int);

int main (void)
{
    init();

    gpio_output_init(17);

    for (;;)
    {
        gpio_set(17);

        sleep(1000);

        gpio_clr(17);

        sleep(1000);
    }

    printf("hello world\n");
    printf("hi %s at %d\n", "there", 1406);

    return 0;
}
