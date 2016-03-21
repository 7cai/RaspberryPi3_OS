#include "rpsio.h"
#include "rpslib.h"
#include "gpio.h"

extern void init(void);

int bootloader (void)
{
    init();

    printf("hello world\n");
    printf("hi %s at %d\n", "there", 1406);

    gpio_output_init(17);

    for (;;)
    {
        gpio_set(17);
        sleep(1000);

        gpio_clr(17);
        sleep(1000);
    }

    return 0;
}
