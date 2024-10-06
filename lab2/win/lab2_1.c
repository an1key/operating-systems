#include "stdio.h"
#include "string.h"
#include "Windows.h"
int main(){
	HANDLE inpHandle = GetStdHandle(STD_INPUT_HANDLE);
	HANDLE outHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	HANDLE fHandle = CreateFile("file.txt", GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	if (fHandle == INVALID_HANDLE_VALUE) {
        WriteFile(outHandle, "Error opening file!\n", 19, NULL, NULL);
        return 1;
    }
	DWORD actlen;
    WriteFile(outHandle, "Enter text!\n", 12, &actlen, NULL);
    
	char* inp = (char*)malloc(256);
	if (inp == NULL) {
        WriteFile(outHandle, "Memory allocation failed!\n", 26, &actlen, NULL);
        CloseHandle(fHandle);
        return 1;
    }
	
    DWORD readLen;
    // Чтение данных с консоли
    ReadFile(inpHandle, inp, 256, &readLen, NULL);

    // Убираем символ новой строки, если он есть
    if (readLen > 0 && inp[readLen - 2] == '\r' && inp[readLen - 1] == '\n') {
        inp[readLen - 2] = '\0';  // Заменяем символы возврата каретки и новой строки
    } else if (readLen > 0 && inp[readLen - 1] == '\n') {
        inp[readLen - 1] = '\0';  // Убираем только символ новой строки
    }

    // Записываем в файл
    WriteFile(fHandle, inp, strlen(inp), &actlen, NULL);

    // Освобождаем ресурсы
    free(inp);
    CloseHandle(fHandle);

    return 0;
}