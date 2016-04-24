#include "rpsio.h"
#include "rpslib.h"
#include "gpio.h"
#include "pmap.h"

extern u_long get_current_el();

void main (void)
{
        
    printf("%lx\n", get_current_el());
    
    printf("We are finally at EL1!\n");

    page_check();
    
    printf("Page check passed!\n");
    
    gpio_output_init(17);
    for (;;)
    {
        gpio_set(17);
        sleep(1000);

        gpio_clr(17);
        sleep(1000);
    }
    
    panic("The end of main()\n");
}
