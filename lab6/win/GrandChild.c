#include <stdio.h>
#include <windows.h>

int main(int argc, char* argv[]) {
    if (argc < 3) {
        printf("Grandchild process requires name and parent name arguments\n");
        return 1;
    }

    const char* grandChildName = argv[1];
    const char* parentName = argv[2];

    printf("Grandchild process %s (Parent: %s) started\n", grandChildName, parentName);

    for (int i = 1; i <= 20; i++) {
        printf("Grandchild %s step %d (Parent: %s)\n", grandChildName, i, parentName);
        Sleep(1200); // Slightly different delay
    }

    return 0;
}
