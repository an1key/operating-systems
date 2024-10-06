#include <stdio.h>
#include <stdlib.h>
#include "Windows.h"
#include <conio.h>
int main(){
	HANDLE inpHandle = GetStdHandle(STD_INPUT_HANDLE);
	HANDLE outHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	HANDLE errHandle = GetStdHandle(STD_ERROR_HANDLE);
	_cprintf("Input handle: %i  Output handle: %i  Error handle: %i\n", inpHandle, outHandle, errHandle);
	_cprintf("Enter text: \n");
	char * input = (char*)malloc(100);
	ReadFile(inpHandle, input, 100, 0 , NULL);
	WriteFile(outHandle, input, strlen(input), 0, NULL);
    return 0;
}