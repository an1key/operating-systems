#define _WIN32_WINNT 0x0500
#include <windows.h>
#include <stdio.h>

void CreateChildProcess(const char* processName, const char* childName, HANDLE hJob) {
    STARTUPINFO si = { sizeof(STARTUPINFO) };
    PROCESS_INFORMATION pi;
    char cmdLine[256];
    snprintf(cmdLine, sizeof(cmdLine), "%s %s", processName, childName);

    if (CreateProcess(NULL, cmdLine, NULL, NULL, FALSE, CREATE_SUSPENDED, NULL, NULL, &si, &pi)) {
        AssignProcessToJobObject(hJob, pi.hProcess);
        ResumeThread(pi.hThread);
        CloseHandle(pi.hThread);
    } else {
        printf("Failed to create child process %s\n", childName);
    }
}

int main() {
    HANDLE hJob1 = CreateJobObject(NULL, NULL);
	HANDLE hJob2 = CreateJobObject(NULL, NULL);
    if (!hJob1 || !hJob2) {
        printf("Failed to create job object\n");
        return 1;
    }

    printf("Parent process started\n");
    CreateChildProcess("child1.exe", "Child1", hJob1);
    CreateChildProcess("child2.exe", "Child2", hJob2);

    for (int i = 1; i <= 20; i++) {
        printf("Parent step %d\n", i);
        Sleep(1000);

        if (i == 7) {
            printf("Terminating Child1\n");
            TerminateJobObject(hJob1, 0);
        }

        if (i == 11) {
            printf("Terminating Child2 and its children\n");
            TerminateJobObject(hJob2, 0);
        }
    }

    CloseHandle(hJob1);
	CloseHandle(hJob2);
    return 0;
}
