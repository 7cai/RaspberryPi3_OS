#define CYCLES_PER_MILLISECOND 1500

extern void dummy ( unsigned int );

void sleep(unsigned int millisecond)
{
    unsigned int i, j;
    for (i = 0; i < millisecond; i++)
    {
        for (j = 0; j < CYCLES_PER_MILLISECOND; j++)
        {
            dummy(j);
        }
    }

    for (i = 0; i < 150; i++)
    {
        dummy(i);
    }
}