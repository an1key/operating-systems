#include <stdio.h>
#include <windows.h>
#include <conio.h>

void clear_line(HANDLE hConsole) {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    
    DWORD consoleWidth = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    // Получаем позицию курсора и очищаем строку
    COORD pos = { 0, csbi.dwCursorPosition.Y };  // Начало текущей строки
    DWORD charsWritten;
    FillConsoleOutputCharacter(hConsole, ' ', consoleWidth, pos, &charsWritten);
    
    // Возвращаем курсор в начало строки
    SetConsoleCursorPosition(hConsole, pos);
}

void print_centered(HANDLE hConsole, const char* message, WORD color) {
    // Устанавливаем цвет текста
    SetConsoleTextAttribute(hConsole, color);
    
    // Очищаем строку и выводим сообщение
    clear_line(hConsole);
    
    // Получаем ширину консоли
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    int consoleWidth = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    int consoleHeight = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
    // Определяем позицию для центрирования текста
    int msgLen = strlen(message);
    int xPos = (consoleWidth - msgLen) / 2;
    int yPos = consoleHeight / 2;
    // Устанавливаем курсор и выводим сообщение
    COORD pos = { (SHORT)xPos, (SHORT)yPos };
    SetConsoleCursorPosition(hConsole, pos);
    printf("%s", message);

    // Возвращаем цвет по умолчанию
    SetConsoleTextAttribute(hConsole, csbi.wAttributes);
}

int main() {
    const char* filename = "./testfile.txt";
    HANDLE hFile;
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    // Пытаемся открыть файл с возможностью совместного чтения
    hFile = CreateFile(filename, GENERIC_READ | GENERIC_WRITE, 0 , NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    if (hFile == INVALID_HANDLE_VALUE) {
        DWORD error = GetLastError();
        if (error == ERROR_FILE_NOT_FOUND) {
            print_centered(hConsole, "File not found. Exiting...", FOREGROUND_RED);
			getch();
            return 1;
        } else if (error == ERROR_SHARING_VIOLATION) {
            print_centered(hConsole, "File is locked by another instance. Waiting for access...", FOREGROUND_RED);
        } else {
            print_centered(hConsole, "Failed to open file. Exiting...", FOREGROUND_RED);
			getch();
            return 1;
        }

        // Ожидание разблокировки файла
        while (hFile == INVALID_HANDLE_VALUE) {
            Sleep(1000);  // Ждём 1 секунду перед повторной попыткой
            hFile = CreateFile(filename, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

            if (hFile != INVALID_HANDLE_VALUE) {
                print_centered(hConsole, "File unlocked and opened successfully!", FOREGROUND_GREEN);
            }
        }
    } else {
        print_centered(hConsole, "File opened successfully!", FOREGROUND_GREEN);
    }

    // Заблокировать первые 100 байт файла
    if (!LockFile(hFile, 0, 0, 100, 0)) {
        print_centered(hConsole, "Failed to lock file. Exiting...", FOREGROUND_RED);
        CloseHandle(hFile);
		getch();
        return 1;
    }

    print_centered(hConsole, "File locked successfully!", FOREGROUND_GREEN);

    // Чтение данных из файла
    char buffer[256];
    DWORD bytesRead;
    BOOL result = ReadFile(hFile, buffer, sizeof(buffer) - 1, &bytesRead, NULL);

    if (result && bytesRead > 0) {
        buffer[bytesRead] = '\0';  // Завершаем строку
        print_centered(hConsole, buffer, FOREGROUND_BLUE);
    } else {
        print_centered(hConsole, "Error reading file.", FOREGROUND_RED);
    }
	getch();
    // Разблокировка файла
    UnlockFile(hFile, 0, 0, 100, 0);
    print_centered(hConsole, "File unlocked successfully!", FOREGROUND_GREEN);
    CloseHandle(hFile);
	getch();
      // Ожидание нажатия клавиши перед выходом
    return 0;
}
