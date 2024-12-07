#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <process.h>
#include <conio.h>
#include <stdint.h> // Для intptr_t

HANDLE consoleout;
HANDLE Semaphore, Semaphore2;
CRITICAL_SECTION CriticalSection;
int Delay1, Delay2;
char Buffer[12];
char names[3][14] = {
        {"Novosibirsk"},
        {"Semipalatink"},
        {"Ekaterinburg"}};

int n = 0;

void SetWindow(int Width, int Height) {
    COORD coord;
    coord.X = Width;
    coord.Y = Height;

    SMALL_RECT Rect;
    Rect.Top = 0;
    Rect.Left = 0;
    Rect.Bottom = Height - 1;
    Rect.Right = Width - 1;

    SetConsoleScreenBufferSize(consoleout, coord);
    SetConsoleWindowInfo(consoleout, TRUE, &Rect);
}

DWORD WINAPI Write(void* arg) {
    int r = (intptr_t)arg - 1; // Безопасное преобразование указателя в целое число

    while (1) {
        WaitForSingleObject(Semaphore, INFINITE);

        for (n = 0; n < 6; n++) {
            Buffer[n] = names[r][n];
        }
        Sleep(100 * Delay1);

        for (n = 6; n < 12; n++) {
            Buffer[n] = names[r][n];
        }
        ReleaseSemaphore(Semaphore2, 1, NULL);
        Sleep(2000 + rand() % 1001);
    }
    return 0;
}

DWORD WINAPI Read(void* arg) {
    COORD pos;
    char buff[12];
    int lenBuff;

    for (int i = 0; i < 24; i++) {
        WaitForSingleObject(Semaphore2, INFINITE);
        pos.X = 20 * (intptr_t)arg; // Безопасное преобразование
        pos.Y = i + 1;

        EnterCriticalSection(&CriticalSection);
        SetConsoleCursorPosition(consoleout, pos);
        switch ((intptr_t)arg) {
            case 1:
                SetConsoleTextAttribute(consoleout, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
                break;
            case 2:
                SetConsoleTextAttribute(consoleout, FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
                break;
            case 3:
                SetConsoleTextAttribute(consoleout, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
                break;
        }
        lenBuff = sprintf(buff, "%s", Buffer);
        lenBuff = ((intptr_t)arg > 1) ? lenBuff - 1 : lenBuff;
        WriteFile(consoleout, buff, lenBuff, NULL, NULL);
        LeaveCriticalSection(&CriticalSection);

        ReleaseSemaphore(Semaphore, 1, NULL);
        Sleep(100 * Delay2);
    }
    return 0;
}

int main(int args, char* argv[]) { // Корректный прототип main
    system("cls");
    consoleout = GetStdHandle(STD_ERROR_HANDLE);
    SetWindow(120, 30);
    HANDLE writeThread[3];
    HANDLE readThread[3];
    Delay1 = 20;
    Delay2 = 3;

    InitializeCriticalSection(&CriticalSection);

    if (args >= 3) {
        sscanf(argv[1], "%d", &Delay1);
        sscanf(argv[2], "%d", &Delay2);
    }

    Semaphore = CreateSemaphore(NULL, 1, 1, NULL);
    Semaphore2 = CreateSemaphore(NULL, 0, 1, NULL);

    for (int i = 0; i < 3; i++) {
        writeThread[i] = CreateThread(NULL, 4096, Write, (void*)(intptr_t)(i + 1), 0, NULL);
        readThread[i] = CreateThread(NULL, 4096, Read, (void*)(intptr_t)(i + 1), 0, NULL);
    }

    getchar();

    DeleteCriticalSection(&CriticalSection);

    for (int i = 0; i < 3; i++) {
        CloseHandle(writeThread[i]);
        CloseHandle(readThread[i]);
    }

    return 0;
}

