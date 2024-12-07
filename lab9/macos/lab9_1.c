#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <fcntl.h>
#include <unistd.h>

#define SHM_SIZE 1024
#define SEM_NAME "/my_semaphore"

int main() {
    key_t key = ftok("shmfile", 65); // Уникальный ключ
    int shmid = shmget(key, SHM_SIZE, 0666 | IPC_CREAT); // Создаем разделяемую память
    if (shmid == -1) {
        perror("shmget");
        exit(1);
    }

    char *shared_mem = (char *)shmat(shmid, NULL, 0); // Подключаем разделяемую память
    if (shared_mem == (char *)-1) {
        perror("shmat");
        exit(1);
    }

    printf("Shared memory created. Address: %p\n", shared_mem);

    // Создание семафора
    sem_t *sem = sem_open(SEM_NAME, O_CREAT, 0644, 0);
    if (sem == SEM_FAILED) {
        perror("sem_open");
        exit(1);
    }

    // Ждем 10-15 секунд
    printf("Waiting before writing to shared memory...\n");
    sleep(10);

    // Запись текста в разделяемую память
    char message[] = "Hello from process 1!";
    strcpy(shared_mem, message);
    printf("Data written to shared memory: %s\n", message);

    // Уведомление второго процесса через семафор
    sem_post(sem);

    // Выделение дополнительной памяти
    printf("Allocating additional memory...\n");
    char *extra_mem = malloc(1000);
    if (!extra_mem) {
        perror("malloc");
        exit(1);
    }

    for (int i = 0; i < 15; i++) {
        size_t offset = i * 400;
        if (offset >= 1000) break;
        extra_mem[offset] = 'a' + i;
        printf("Wrote '%c' at address %p\n", extra_mem[offset], &extra_mem[offset]);
    }

    // Завершаем работу
    sem_close(sem);
    shmdt(shared_mem);
    printf("Process 1 finished.\n");
    return 0;
}
