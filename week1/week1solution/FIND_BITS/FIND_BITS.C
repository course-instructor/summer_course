#include <stdio.h>

#define BITS sizeof(unsigned long) * 8

int mask(unsigned long input) 
{
    int retval = 0;

    if (BITS > (sizeof(unsigned long) * 8))
    {
        printf("BITS too large!\n");
        retval = 1;
    }
    else
    {
        unsigned long bit = 1UL << (BITS - 1);

        printf("\n");
        for (int i = 0; i < BITS; i++)
        {
            if (input & bit)
            {
                printf("1");
            }
            else
            {
                printf("0");
            }
            bit >>= 1;
        }
        printf("\n");
    }

    return retval;
}

int main(void)
{
    int retval = 0;
    unsigned long x;
    unsigned long y;

    printf("please enter a value for x: ");
    if (scanf("%lu", &x) != 1)
    {
        printf("cant read x\n");
        retval = 1;
    }
    else
    {
        mask(x);

        printf("\nplease enter a value for y: ");
        if (scanf("%lu", &y) != 1)
        {
            printf("cant read y\n");
            retval = 1;
        }
        else
        {
            mask(y);

            unsigned long check = 1UL << (BITS - 1);
            int counter = 0;

            for (int i = 0; i < BITS; i++)
            {
                if ((x & check) && (y & check))
                {
                    counter++;
                    printf("1");
                }
                else
                {
                    printf("-");
                }
                check >>= 1;
            }

            printf("\nThe amount of bits in same place that are on is: %d\n", counter);
        }
    }

    return retval;
}
