#include "syscall.h"

void parent()
{
    int *test0 = (int *)0x500;
    while(1)
    {
        printf("parent: %lx %d\n", test0, *test0);
        *test0 = 1000;
        printf("parent: %lx %d\n", test0, *test0);
        test0++;
        printf("%d %d %d %d %d %d %d %d %d %d\n", 0, 1, 2, 3, 4, 5, 6, 7, 8, 9);
        sleep(100);
        printf("%u\n", getenvid());
    }
}

void child()
{
    int *test0 = (int *)0x500;
    while(1)
    {
        printf("child: %lx %d\n", test0, *test0);
        *test0 = 1000;
        printf("child: %lx %d\n", test0, *test0);
        test0++;
        yield();
        printf("%d %d %d %d %d %d %d %d %d %d\n", 0, 1, 2, 3, 4, 5, 6, 7, 8, 9);
        sleep(100);
        printf("%u\n", getenvid());
        env_destroy(getenvid());
    }
}

int umain()
{
    printf("test fork:\n");
    unsigned long i = fork();
    if (i == 0)
    {
        printf("this is child: %ld\n", i);
        child();
    }
    else
    {
        printf("this is parent: %ld\n", i);
        parent();
    }
    printf("test finished\n");
    for(;;);
}