#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define NUM_THREADS 3
#define NUM_LETTERS 20

// Массив латинских букв
char letters[NUM_LETTERS] = "ABCDEFGHIJKLMNOPQRST";

// Мьютекс и условные переменные для синхронизации
pthread_mutex_t mutex;
pthread_cond_t cond[NUM_THREADS];

// Для синхронизации нитей
int step[3] = {-1, -1, -1};

// Функция для установки цвета текста
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

// Общая процедура вывода
void* thread_func(void* arg) {
    int thread_id = *(int*)arg;
    int row = thread_id + 1; // Позиция вывода
    int color = thread_id + 1; // Цвет текста
    int repeat_count = thread_id * 2; // Количество повторений

    // Настройка отмены нитей
    if (thread_id == 1 || thread_id == 3) {
        pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
    }

    for (int k = 0; k < NUM_LETTERS; ++k) {
        pthread_mutex_lock(&mutex);

        // Установка позиции и цвета
        set_position(row, k * repeat_count + 1);
        set_color(color);

        // Вывод буквы
        for (int i = 0; i < repeat_count; ++i) {
            printf("%c", letters[k]);
            fflush(stdout);
        }

        reset_color();
        pthread_mutex_unlock(&mutex);

        usleep((rand() % 1000 + 500) * 1000); // Случайная задержка

        step[thread_id - 1] = k;
        if (thread_id == 3 && k == 12) {
            pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
            pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);
        }

        // Сигнализируем главной нити, что третья нить достигла 10-го шага
        if (thread_id == 3 && k == 10) {
            pthread_cond_signal(&cond[2]);
        }

        // Сигнализируем главной нити, что первая нить достигла 5-го шага
        if (thread_id == 1 && k == 5) {
            pthread_cond_signal(&cond[0]);
        }

        // Сигнализируем главной нити, что третья нить достигла 16-го шага
        if (thread_id == 3 && k == 16) {
            pthread_cond_signal(&cond[2]);
            pthread_cancel(pthread_self()); // Прямое вызовы отмены самой себя
        }
    }

    return NULL;
}

int main() {
    pthread_t threads[NUM_THREADS];
    int thread_ids[NUM_THREADS] = {1, 2, 3};
    pthread_mutex_init(&mutex, NULL);
    for (int i = 0; i < NUM_THREADS; ++i) {
        pthread_cond_init(&cond[i], NULL);
    }

    // Очистка экрана
    printf("\033[2J");

    // Запуск дополнительных нитей
    for (int i = 0; i < NUM_THREADS; ++i) {
        pthread_create(&threads[i], NULL, thread_func, &thread_ids[i]);
    }

    // Главная нить: ожидание сигналов и попытка отмены
    pthread_mutex_lock(&mutex);

    // Ожидаем, пока первая нить не достигнет 5-го шага
    while (step[0] < 5) {
        pthread_cond_wait(&cond[0], &mutex);
    }
    pthread_cancel(threads[0]);
    set_position(5, 1);
    printf("Попытка отмены первой нити.");

    // Ожидаем, пока третья нить не достигнет 10-го шага
    while (step[2] < 10) {
        pthread_cond_wait(&cond[2], &mutex);
    }
    pthread_cancel(threads[2]);
    set_position(6, 1);
    printf("Попытка отмены третьей нити.");

    // Ожидаем, пока третья нить не достигнет 16-го шага
    while (step[2] < 16) {
        pthread_cond_wait(&cond[2], &mutex);
    }
    // Выводим сообщение о завершении работы третьей нити
    set_position(7, 1);
    printf("Третья нить достигла 16-го шага и будет отменена.");

    pthread_cancel(threads[2]);

    // Подождем завершения всех нитей
    pthread_mutex_unlock(&mutex);

    // Ожидание завершения нитей
    for (int i = 0; i < NUM_THREADS; ++i) {
        pthread_join(threads[i], NULL);
    }

    pthread_mutex_destroy(&mutex);
    for (int i = 0; i < NUM_THREADS; ++i) {
        pthread_cond_destroy(&cond[i]);
    }

    // Восстановление позиции и цвета
    set_position(10, 1);
    reset_color();
    printf("Работа завершена.\n");

    return 0;
}
