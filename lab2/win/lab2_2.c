#include "string.h"
#include "stdio.h"
#include "Windows.h"

int main() {
    HANDLE outHandle = GetStdHandle(STD_OUTPUT_HANDLE);

    // Открываем файл для чтения
    HANDLE fHandle1 = CreateFile("./file.txt", GENERIC_READ,          // open for reading
                       FILE_SHARE_READ,       // share for reading
                       NULL,                  // default security
                       OPEN_EXISTING,         // existing file only
                       FILE_ATTRIBUTE_NORMAL, // normal file
                       NULL);
    if (fHandle1 == INVALID_HANDLE_VALUE) {
        WriteFile(outHandle, "Error opening file\n", 19, NULL, NULL);
        return 1;
    }

    // Создаем второй хэндл через DuplicateHandle
    HANDLE fHandle2;
    DuplicateHandle(GetCurrentProcess(), fHandle1, GetCurrentProcess(), &fHandle2, 0, FALSE, DUPLICATE_SAME_ACCESS);

    // Открываем третий хэндл к тому же файлу
    HANDLE fHandle3 = CreateFile("./file.txt", GENERIC_READ,          // open for reading
                       FILE_SHARE_READ,       // share for reading
                       NULL,                  // default security
                       OPEN_EXISTING,         // existing file only
                       FILE_ATTRIBUTE_NORMAL, // normal file
                       NULL);

    // Позиционируем чтение для fHandle1 на 10-ю позицию
    SetFilePointer(fHandle1, 10, NULL, FILE_BEGIN);

    // Выводим числовые значения всех хэндлов
    printf("Handle 1: %i\nHandle 2: %i\nHandle 3: %i\n", fHandle1, fHandle2, fHandle3);

    // Функция для чтения и вывода данных с указанного хэндла
    void readAndPrint(HANDLE handle, HANDLE outHandle) {
        char buffer[8];  // 7 символов + нулевой символ для завершения строки
        DWORD readLen;

        // Чтение 7 символов
        BOOL readSuccess = ReadFile(handle, buffer, 7, &readLen, NULL);
        if (readSuccess && readLen > 0) {
            buffer[readLen] = '\0';  // Завершаем строку
            WriteFile(outHandle, buffer, readLen, NULL, NULL);  // Выводим на экран
            WriteFile(outHandle, "\n", 1, NULL, NULL);  // Перенос строки
        } else {
            WriteFile(outHandle, "Error reading file\n", 19, NULL, NULL);
        }
    }

    // Используем все три хэндла для чтения по 7 символов
    readAndPrint(fHandle1, outHandle);
    readAndPrint(fHandle2, outHandle);
    readAndPrint(fHandle3, outHandle);

    // Освобождаем ресурсы
    CloseHandle(fHandle1);
    CloseHandle(fHandle2);
    CloseHandle(fHandle3);

    return 0;
}
