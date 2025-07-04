#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#define NUM_OF_UNIQUE_NUMBER_WORDS 28
#define UPPER_LIMIT 99
#define MAX_DIGITS 2

int covert(int argc, char **argv);
int fileInput(char *input_file,char * input);
int fileOutput(char **output_file,char * output);
void getUserInput(char *input);
void getUserInput(char *input);
void printOutput(char *output[100]);
int translateFromStringToInt(char str_numbers[100],int numbers[100]);
char * getLingusticNumber(int num,char str_number[20]);
void numberWordBuilder(int num,int lingustic_num[MAX_DIGITS]);

static const char *number_names[NUM_OF_UNIQUE_NUMBER_WORDS] = {
   [0]="zero",
   [1]="one",
   [2]="two",
   [3]="three",
   [4]="four",
   [5]="five",
   [6]="six",
   [7]="seven",
   [8]="eight",
   [9]="nine",
   [10]="ten",
   [11]="eleven",
   [12]="twelve",
   [13]="thirteen",
   [14]="fourteen",
   [15]="fifteen",
   [16]="sixteen",
   [17]="seventeen",
   [18]="eighteen",
   [19]="nineteen",
   [20]="twenty",
   [21]="thirty",
   [22]="forty",
   [23]="fifty",
   [24]="sixty",
   [25]="seventy",
   [26]="eighty",
   [27]="ninety"
};
int main(int argc, char **argv)
{   
    covert(argc,argv);
    printf("\n");
    return 0;
}
int covert(int argc, char **argv){
    char input[100];
    char * output[100];
    int * numbers;
    int numbers_count;
    if(1){ //               TODO: get input from files
        getUserInput(input);
    }
    numbers_count = translateFromStringToInt(input,numbers);
    printf("number count : %d\n",numbers_count);
    for (int i = 0; i < numbers_count; i++)
    {
        getLingusticNumber(numbers[i],output[i]);
    }

    if(1){      // TODO: give output to files
        printOutput(output);
    }
    
    




}
int fileInput(char *input_file,char * input)
{
    FILE *fptr;
    if(fptr = fopen(input_file, "r") == NULL){
        return 0;
    }
    fgets(input, 100, fptr); 
    fclose(fptr);
    return 0;
}
int fileOutput(char **output_file,char * output)
{
    FILE *fptr;
    if(fptr = fopen(output_file, "w") != NULL){
        return 1;        
    }
    // Write some text to the file
    fprintf(fptr, output); 
    fclose(fptr);
    return 0;
}
void getUserInput(char *input){
    fgets(input,sizeof(char)*100,stdin);
}
void printOutput(char *output[100])
{
    int i=0;
    while(output[i])
    {
        printf("%s\n",output[i]);
        i++;
    }
    

}
int translateFromStringToInt(char str_numbers[100],int numbers[100])
{
    int count = 0;
    char * token = strtok(str_numbers, " ");

     while( token != NULL ) {
      numbers[count++] = atoi(token);
      token = strtok(NULL, " ");
   }
    return count;
    
    
}
char * getLingusticNumber(int num, char str_number[20])
{
    int isZero = 1;
    str_number = "";
    int lingustic_num[MAX_DIGITS]; 
    
    numberWordBuilder(num, lingustic_num);
    for (int i = 0; i < MAX_DIGITS && isZero; i++)
    {
        if(lingustic_num[i] !=0)
        {
            isZero = 0;
        }
    }
    if(!isZero){
        for (int i = 0; i < MAX_DIGITS; i++)
        {
            if(lingustic_num[i]!=0)
            {
               // strcat(str_number ,number_names[lingustic_num[i]]);
                printf("%s ",number_names[lingustic_num[i]]);
            }
        }

    }
    else
    {
        strcpy(str_number,number_names[0]);
    }
            printf("\n");

    return str_number;


}
void numberWordBuilder(int num,int lingustic_num[MAX_DIGITS])
{
        int index = 0;
        if(num<20){
            lingustic_num[index]   = 0;
            lingustic_num[index+1] = num;
        }else{
            lingustic_num[index] = 18+num/10;
            lingustic_num[index+1] = num%10;
        }
}    







// void numberWordBuilder(int num,int lingustic_num[MAX_DIGITS],int index)
// {
//     if(index == MAX_DIGITS)
//     {
//         return;
//     }
//     if(num<20)
//     {
//         lingustic_num[index] = num;

//     }
//     else
//     {
//         lingustic_num[index] = 18+num/10;

//     }
//     numberWordBuilder(num/10,lingustic_num,index+1);

// }


// 0 - 3
// 0 - 10
