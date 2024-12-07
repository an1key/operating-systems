#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdint.h> // Для intptr_t

char alphabet[] = "abcdefghijklmnopqrst";

void set_color(int color) {
    printf("\033[1;%dm", 30 + color); // Цвета 30-37
}

// Функция для установки позиции курсора
void set_position(int row, int col) {
    printf("\033[%d;%dH", row, col);
}

// Функция для восстановления стандартного цвета
void reset_color() {
    printf("\033[0m");
}

void* thread(void* arg) {
    int thread_id = (intptr_t)arg; // Безопасное преобразование указателя в целое число
    int k, j;

    if (thread_id == 1 || thread_id == 3) {
        pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
    }


    int color = thread_id + 1; // Цвет текста
    int repeat_count = thread_id * 2; // Количество повторений

    for (k = 0; k < 20; k++) {
        set_position(k + 1, 2 * thread_id*thread_id);
        set_color(color);

        for (int i = 0; i < repeat_count; ++i) {
            printf("%c", alphabet[k]);
            fflush(stdout);
        }

        if (k == 12 && thread_id == 3) {
            pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
            pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);
        }

        if (k == 15 && thread_id == 3) {
            pthread_testcancel();
            usleep(1000000 + 750000 * thread_id); // Задержка
            pthread_testcancel();
        } else {
            usleep(1000000 + 500000 * (3 - thread_id)); // Задержка
        }
    }
    return NULL;
}

int main() {
    int k, rc;
    pthread_t tid1, tid2, tid3;

    printf("\033[2J\n"); // Очистка экрана

    pthread_create(&tid1, NULL, thread, (void*)(intptr_t)1);
    pthread_create(&tid2, NULL, thread, (void*)(intptr_t)2);
    pthread_create(&tid3, NULL, thread, (void*)(intptr_t)3);

    for (k = 0; k <= 20; k++) {

        if (k == 5) {
            rc = pthread_cancel(tid1);
            if (rc != 0) {
                set_position(25, 1);
                set_color(4);
                printf("Error: first thread cancel failed");
            } else {
                set_position(25, 1);
                set_color(4);
                printf("Trying to cancel first thread");
            }
        }

        if (k == 10) {
            rc = pthread_cancel(tid3);
            if (rc != 0) {
                set_position(25, 1);
                set_color(4);
                printf("Error: third thread cancel failed");
            } else {
                set_position(25, 1);
                set_color(4);
                printf("Third thread cancel is successful");
            }
        }

        usleep(1500000); // Задержка
    }

    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);
    pthread_join(tid3, NULL);

    set_position(25, 1);
    set_color(7);
    printf("Program finished.\n");
    return 0;
}
