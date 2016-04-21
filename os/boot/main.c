#include "rpsio.h"
#include "rpslib.h"
#include "gpio.h"
#include "pmap.h"

int main (void)
{
    printf("this is the start of el1_main\n");

    page_check();
    
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
