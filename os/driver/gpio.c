#include "rpslib.h"

#define PHYSADDR_OFFSET 0x3F200000

#define GPFSEL0 (PHYSADDR_OFFSET + 0x00000000)
#define GPSET0  (PHYSADDR_OFFSET + 0x0000001C)
#define GPCLR0  (PHYSADDR_OFFSET + 0x00000028)

extern unsigned int GET32(unsigned int);
extern void PUT32 ( unsigned int, unsigned int );

void gpio_output_init(int n)
{
    int ra;

    // Set GPIO n as Output
    ra = GET32(GPFSEL0 + 4 * (n / 10));
    ra &= ~(7 << ((n % 10) * 3));
    ra |= 1 << ((n % 10) * 3);
    PUT32(GPFSEL0 + 4 * (n / 10), ra);
    sleep(0);
}

void gpio_set(int n)
{
    PUT32(GPSET0 + 4 * (n / 32), 1 << (n % 32));
    sleep(0);
}

void gpio_clr(int n)
{
    PUT32(GPCLR0 + 4 * (n / 32), 1 << (n % 32));
    sleep(0);
}