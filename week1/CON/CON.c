#include <stdio.h>
#include <ctype.h>
void contract(char s1[], char s2[]);
int main() {
    char s1[80];
    char s2[80];
    printf("ENTER SENTENCE OR A WORD TO BE SHORTEND : \n");
    fgets(s1,sizeof(s1),stdin); //get user input
    contract(s1,s2);
    printf("\n");
    printf("Orignal word : %s\n",s1);
    printf("Shorted word : %s\n",s2);
    return 0;
}

void contract(char s1[], char s2[]){
        while (*s1 != '\0') // go over s1
        {
            *s2 = *s1;    // copy s1 char to s2
            if(isalnum(*s1) && *s1 == *(s1 + 1)-1 && *s1 == *(s1 + 2)-2) // if the charcter is a letter or a number 
            {                                                            // and there are sequence of the of 3 cherecters in assending assci values
                
                while(*s1 == *(s1 + 1)-1) // while assci assending countinue s1
                {
                    s1++;
                }
                s2++;
                *s2 = '-'; //replaced skip chars with ' - '
                s2++;
                *s2 = *s1; // end sequence
                
            }
            //countinue s1 and s2
            s1++; 
            s2++;
        }
        *s2 = '\0'; // finish string


}



