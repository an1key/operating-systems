#define _WIN32_WINNT 0x0500
#include <windows.h>
#include <stdio.h>

void CreateGrandChildProcess(const char* processName, const char* grandChildName, const char* parentName) {
    STARTUPINFO si = { sizeof(STARTUPINFO) };
    PROCESS_INFORMATION pi;
    char cmdLine[256];
    snprintf(cmdLine, sizeof(cmdLine), "%s %s %s", processName, grandChildName, parentName);

    if (CreateProcess(NULL, cmdLine, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
        CloseHandle(pi.hThread);
    } else {
        printf("Failed to create grandchild process %s\n", grandChildName);
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Child process requires a name argument\n");
        return 1;
    }

    const char* childName = argv[1];
    printf("Child process %s started\n", childName);

    CreateGrandChildProcess("grandchild.exe", "GrandChild1", childName);

    for (int i = 1; i <= 20; i++) {
        printf("%s step %d\n", childName, i);
        Sleep(1100); // Slightly different delay
    }

    return 0;
}
