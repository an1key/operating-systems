#include "unistd.h"
#include <stdlib.h>
#include "stdio.h"
#include "string.h"
#include <fcntl.h>
int main(){
    int f = open("./file.txt", O_RDWR | O_CREAT | O_TRUNC, 0666);
    write(1, "Введите текст!\n", 27);
    char* inp = (char*)malloc(256);
    read(0, inp, 256);
    write(f, inp, strlen(inp));
    close(f);
    return 0;
}