#include "syscall.h"

void umain()
{
    int a = 0, b = 1, c = 2, d = 3, e = 4, f = 5;
    int i = 0;
    for ( ; ; i++)
    {
        printf("user program 1 running now!\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n", a, b, c, d, e, f, a, b, c, d, e, f);
        sleep(100);
    }
}
