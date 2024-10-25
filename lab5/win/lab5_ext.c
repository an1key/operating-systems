#include <stdio.h>
#include <windows.h>

int main() {
    HANDLE hConsoleInput = GetStdHandle(STD_INPUT_HANDLE);
    HANDLE hConsoleOutput = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD prev_mode;

    // Сохраняем предыдущий режим консоли и устанавливаем режим для мыши
    if (!GetConsoleMode(hConsoleInput, &prev_mode)) {
        printf("Failed to get console mode.\n");
        return 1;
    }
    if (!SetConsoleMode(hConsoleInput, ENABLE_MOUSE_INPUT | ENABLE_EXTENDED_FLAGS | ENABLE_PROCESSED_INPUT)) {
        printf("Failed to set console mode for mouse input.\n");
        return 1;
    }

    // Открываем файл и выводим его содержимое на консоль
    FILE *file = fopen("text.txt", "r");
    if (!file) {
        printf("Error opening file.\n");
        return 1;
    }

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        printf("%s", line);
    }
    fclose(file);

    INPUT_RECORD inpBuf;
    DWORD actLen;


    while (1) {
        // Чтение событий с консоли
        if (!ReadConsoleInput(hConsoleInput, &inpBuf, 1, &actLen)) {
            printf("ReadConsoleInput failed.\n");
            break;
        }

        // Проверка на тип события MOUSE_EVENT
        if (inpBuf.EventType == MOUSE_EVENT) {
            MOUSE_EVENT_RECORD mer = inpBuf.Event.MouseEvent;

            // Проверка на нажатие левой кнопки мыши
            if (mer.dwButtonState == FROM_LEFT_1ST_BUTTON_PRESSED) {
                COORD pos = mer.dwMousePosition;

                CHAR_INFO charInfo;
                SMALL_RECT readRect = {pos.X, pos.Y, pos.X, pos.Y};
                COORD bufferSize = {1, 1};
                COORD bufferCoord = {0, 0};

                // Чтение символа по координатам щелчка
                if (ReadConsoleOutput(hConsoleOutput, &charInfo, bufferSize, bufferCoord, &readRect)) {
                    if (charInfo.Char.AsciiChar != ' ') {  // Игнорируем пробелы
                        // Изменяем регистр символа
                        if (charInfo.Char.AsciiChar >= 'a' && charInfo.Char.AsciiChar <= 'z') {
                            charInfo.Char.AsciiChar = charInfo.Char.AsciiChar - 'a' + 'A';  // Перевод в верхний регистр
                        } else if (charInfo.Char.AsciiChar >= 'A' && charInfo.Char.AsciiChar <= 'Z') {
                            charInfo.Char.AsciiChar = charInfo.Char.AsciiChar - 'A' + 'a';  // Перевод в нижний регистр
                        }

                        // Изменяем цвет символа на ярко-красный
                        charInfo.Attributes = FOREGROUND_RED | FOREGROUND_INTENSITY;

                        // Запись измененного символа обратно в консоль
                        WriteConsoleOutput(hConsoleOutput, &charInfo, bufferSize, bufferCoord, &readRect);
                    }
                }
            }
            // Завершение программы по правому клику мыши
            else if (mer.dwButtonState == RIGHTMOST_BUTTON_PRESSED) {
                printf("\nExiting program...\n");
                break;
            }
        }
    }

    // Восстановление предыдущего режима консоли
    SetConsoleMode(hConsoleInput, prev_mode);
    return 0;
}
