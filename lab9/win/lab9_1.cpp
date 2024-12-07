#include <Windows.h>
#include <stdio.h>
#include <tchar.h>
#include <iostream>

#define BUF_SIZE 1024
#define UNICODE // Включение поддержки Unicode

LPCWSTR SemaphoreName = L"MySemaphore";        // Имя семафора
TCHAR SharedMemoryName[] = TEXT("MySharedMemory"); // Имя разделяемой памяти

int main() {
    // Устанавливаем кодировку UTF-8 для корректного вывода в консоль
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    // Создание семафора
    HANDLE hSemaphore = CreateSemaphore(NULL, 0, 1, reinterpret_cast<LPCSTR>(SemaphoreName));
    if (!hSemaphore) {
        wprintf(L"error semaphor: %d\n", GetLastError());
        return 1;
    }

    // Создание разделяемой памяти
    HANDLE hMapFile = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, BUF_SIZE, SharedMemoryName);
    if (!hMapFile) {
        wprintf(L"error of delenie: %d\n", GetLastError());
        CloseHandle(hSemaphore);
        return 1;
    }

    // Отображение разделяемой памяти в адресное пространство процесса
    LPTSTR pBuf = (LPTSTR)MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, 0);
    if (!pBuf) {
        wprintf(L"error of memory: %d\n", GetLastError());
        CloseHandle(hMapFile);
        CloseHandle(hSemaphore);
        return 1;
    }

    wprintf(L"Delenie 0x%p\n", pBuf);

    // Задержка 10 секунд для эмуляции обработки
    Sleep(10000);

    // Запись первого сообщения в разделяемую память
    TCHAR firstMessage[] = TEXT("nigga");
    CopyMemory((PVOID)pBuf, firstMessage, (_tcslen(firstMessage) + 1) * sizeof(TCHAR));
    wprintf(L"Message: %s\n", firstMessage);

    // Освобождение семафора
    ReleaseSemaphore(hSemaphore, 1, NULL);

    // Запрос дополнительной памяти через VirtualAlloc
    LPVOID additionalMemory = VirtualAlloc(NULL, 1000, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (!additionalMemory) {
        wprintf(L"Error VirtualAlloc: %d\n", GetLastError());
    } else {
        wprintf(L"Add new memory 0x%p\n", additionalMemory);

        // Запись символов в выделенную память с шагом 400 байт
        char alphabet[] = "abcdefghijklmnopqrstuvwxyz";
        int step = 400;
        for (int i = 0; i < 15; ++i) {
            char value = alphabet[i % 26];
            LPBYTE address = (LPBYTE)additionalMemory + (i * step);
            *address = value;

            wprintf(L"Symbol '%c' address: 0x%p\n", value, address);
        }
    }

    // Завершение работы программы
    wprintf(L"\nenter to continiu...");
    getchar();

    // Освобождение ресурсов
    if (additionalMemory) VirtualFree(additionalMemory, 0, MEM_RELEASE);
    UnmapViewOfFile(pBuf);
    CloseHandle(hMapFile);
    CloseHandle(hSemaphore);

    return 0;
}
