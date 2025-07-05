#include <stdio.h>
void printinput(unsigned long input) 
{
	unsigned long bit = 1UL << (sizeof(unsigned long) * 8 - 1);
	printf("\n");
	for (int i = 0; i < sizeof(unsigned long) * 8; i++)
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
///tomer comments: in our team we use retvals in functions, please use it also. tell me if you need more explantion.
void main()
{
	unsigned long x;
	unsigned long y;
	printf("please enter a value for x: ");
	scanf_s("%lu", &x);
	printinput(x);
	printf("\n please enter a value for y: ");
///tomer comments: why you used scanf_s? scanf_s is not supporded in linux so you you probably didn't compile with gcc.
	scanf_s("%lu", &y);
	printinput(y);
///tomer comments: this variable can be confusing i would call it "mask" or something else that other people will understand, its a standart in our team.
/// also you used "(sizeof(unsigned long) * 8)" many times, its better to defined as a macro.
	unsigned long check = 1UL << (sizeof(unsigned long) * 8 - 1);

	int counter = 0;
	for (int i = 0;  i < sizeof(unsigned long)*8;  i++)
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
	printf("\n The amount of bits in same place that are on is: %d", counter);
}
