#include <stdio.h>
int find_bits_helper(unsigned int x , unsigned int y, unsigned int counter);
void find_bits();
int main()
{
    find_bits(15,15);
    printf("\n");

    return 0;
}
void find_bits(){
    unsigned int x,y;
    printf("This program finds all the common bits between X and Y that turned on!\n");
    
    printf("Please enter X:\n");
    scanf("%u",&x);
    printf("Please enter Y:\n");
    scanf("%u",&y);

    printf("\nThere are %d bits turned on!\n",find_bits_helper(x , y,sizeof(unsigned int)*8));
    
}
int find_bits_helper(unsigned int x , unsigned int y, unsigned int counter){
    if(counter==0){   //stop condition
        return ((x&1) & (y&1));
    }
    int bit = find_bits_helper(x>>1,y>>1,counter-1);  // call function recursively
    if((x&1) & (y&1)){ // checks if left most bit of x and y are both equal to 1
        printf("1");
    }else{
        printf("-");
    }
    return bit + ((x&1) & (y&1));
}

