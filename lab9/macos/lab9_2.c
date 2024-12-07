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
    key_t key = ftok("shmfile", 65); // Тот же ключ, что и в первой программе
    int shmid = shmget(key, SHM_SIZE, 0666); // Получаем доступ к разделяемой памяти
    if (shmid == -1) {
        perror("shmget");
        exit(1);
    }

    char *shared_mem = (char *)shmat(shmid, NULL, 0); // Подключаем разделяемую память
    if (shared_mem == (char *)-1) {
        perror("shmat");
        exit(1);
    }

    printf("Shared memory attached. Address: %p\n", shared_mem);

    // Читаем данные из разделяемой памяти
    printf("Initial data in shared memory: %s\n", shared_mem);

    // Открываем семафор
    sem_t *sem = sem_open(SEM_NAME, 0);
    if (sem == SEM_FAILED) {
        perror("sem_open");
        exit(1);
    }

    // Ждем семафор
    printf("Waiting for semaphore...\n");
    sem_wait(sem);

    // Читаем данные из разделяемой памяти после семафора
    printf("Data after semaphore: %s\n", shared_mem);

    // Завершаем работу
    sem_close(sem);
    shmdt(shared_mem);
    printf("Process 2 finished.\n");
    return 0;
}
