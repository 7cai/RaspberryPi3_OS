#include "rpsio.h"
#include "rpslib.h"
#include "gpio.h"
#include "pmap.h"

extern u_long get_current_el();
extern void el1_mmu_activate();

void main(void)
{
    printf("We are now at EL%lx.\n", get_current_el());

    // activate mmu for el1
    // el1_mmu_activate();
    // printf("MMU activated.\n");

    //printf("%s\n", "Page check start");
    //page_check();
    //printf("Page check passed!\n\n\n\n");

    printf("Activating LED...\n");
    gpio_output_init(17);
    for ( ; ; )
    {
        gpio_set(17);
        sleep(1000);

        gpio_clr(17);
        sleep(1000);
    }

    panic("The end of main()\n");
}
