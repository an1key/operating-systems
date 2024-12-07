#include "unistd.h"
#include <stdlib.h>
#include "string.h"
#include "stdio.h"
#include <fcntl.h>

int main(){
    int fHandle1 = open("./file.txt", O_RDONLY);
    int fHandle2 = dup(fHandle1);
    int fHandle3 = open("./file.txt", O_RDONLY);
    char* buffer = (char*) malloc(256);
    read(fHandle1, buffer, 256);
    write(1, buffer, 256);

    fflush(stdout);
    fprintf(stdout, "Handle 1: %i\nHandle 2: %i\nHandle 3: %i\n", fHandle1, fHandle2, fHandle3);

    lseek(fHandle1, 10, SEEK_SET);
    read(fHandle1, buffer, 7);
    write(1, buffer, 7);
    write(1, "\n", 2);
    read(fHandle2, buffer, 7);
    write(1, buffer, 7);
    write(1, "\n", 2);
    read(fHandle3, buffer, 7);
    write(1, buffer, 7);
    write(1, "\n", 2);

    close(fHandle1);
    close(fHandle2);
    close(fHandle3);
    return 0;
}