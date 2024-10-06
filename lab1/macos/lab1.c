#include <unistd.h>
#include <string.h>
#include <stdlib.h>
int main(){

    write(0, "Введите текст!\n", 27);
    char* inp = (char*)malloc(100);
    read(0, inp, 100);
    if (isatty(1)){
        write(1, "Вы ввели:\n", 17);
        write(1, inp, strlen(inp));
    } else {
        write(0, "Вывод в файл\n", 27);
        write(1, inp, strlen(inp));
    }

    return 0;
}