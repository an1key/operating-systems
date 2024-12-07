#include <Windows.h>
#include <stdio.h>
#include <tchar.h>

#define BUF_SIZE 1024
#define UNICODE // Включение поддержки Unicode

LPCWSTR SemaphoreName = L"MySemaphore";        // Имя семафора
TCHAR SharedMemoryName[] = TEXT("MySharedMemory"); // Имя разделяемой памяти

int main() {
    // Устанавливаем кодировку UTF-8 для корректного вывода в консоль
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    // Открытие семафора
    HANDLE hSemaphore = OpenSemaphore(SEMAPHORE_ALL_ACCESS, FALSE, reinterpret_cast<LPCSTR>(SemaphoreName));
    if (!hSemaphore) {
        wprintf(L"error semaphor: %d\n", GetLastError());
        return 1;
    }

    // Открытие разделяемой памяти
    HANDLE hMapFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, SharedMemoryName);
    if (!hMapFile) {
        wprintf(L"Error of delenie: %d\n", GetLastError());
        CloseHandle(hSemaphore);
        return 1;
    }

    // Отображение разделяемой памяти в адресное пространство процесса
    LPTSTR pBuf = (LPTSTR)MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, BUF_SIZE);
    if (!pBuf) {
        wprintf(L"error of menmorry %d\n", GetLastError());
        CloseHandle(hMapFile);
        CloseHandle(hSemaphore);
        return 1;
    }

    wprintf(L"memory address: 0x%p\n", pBuf);

    // Чтение первого сообщения из разделяемой памяти
    wprintf(L"First message read...\n");
    wprintf(L"First message: %s\n", pBuf);

    // Ожидание семафора
    wprintf(L"waiting...\n");
    WaitForSingleObject(hSemaphore, INFINITE);

    // Чтение второго сообщения из разделяемой памяти
    wprintf(L"Second message read...\n");
    wprintf(L"Second message: %s\n", pBuf);

    // Освобождение ресурсов
    UnmapViewOfFile(pBuf);
    CloseHandle(hMapFile);
    CloseHandle(hSemaphore);

    wprintf(L"\nEnter to continue");
    getchar();

    return 0;
}
