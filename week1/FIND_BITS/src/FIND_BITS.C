    #include <stdio.h>

    unsigned long countBits(unsigned long comon_bits)
    {
        /**
         * @brief this function counts the number of bits in an unsigend integer and prints it in binary from its first 1 bit.
         * @param comon_bits: the unsigned long integer to count the bits of.
         * @return: the number of 1 bits in the unsigned long integer.
         */
        int return_value = 0;
        if(comon_bits != 0)
        {
            return_value = countBits(comon_bits >> 1);
            if(comon_bits & 1)
            {
                printf("1");
                return_value++;
            }
            else
            {
                printf("0");
            }
        }
        return return_value;        
    }
    

    int main()
    {
        /**
         * @brief this program counts how many common 1 bits two unsigned long integers have and prints where each common bit is. 
         */
        

        unsigned long x; // the first unsigned long integer
        unsigned long y; // the second unsigned long integer

        printf("Enter an unsigened long x and y to count how many common 1 bits they have:\n");
        scanf("%lu %lu", &x, &y);
        unsigned long comon_bits = x & y; 
        
        
        printf("the common bits of x and y:\t");
        if (!comon_bits) // ensure that there is still a print even if there are no common on bits
            printf("0");
        unsigned long count = countBits(comon_bits);
        printf("\n");    
        printf("x and y share %lu common 1 bits.\n", count);
        return 0;
    }