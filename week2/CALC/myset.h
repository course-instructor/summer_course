#ifndef MYSET_H
#define MYSET_H
#include "set.h"
#include <stdio.h>
int input(int * command_num, set * varibles[3], int numbers[128], char * PATH);

int commandValidation   (char * line,int * command_num, int * p_offset);
int numberValidation    (char * line,int * p_offset, int numbers[128]);
int setValidation       (char * line, set * varibles[3],int * p_offset,int command_num);
int commaValidation     (char * line);

void printInstructions(void);
void operation(int command_num, set ** varibles, int * numbers);
void startProgram(int argc , char ** args);
void stop(void);

#endif