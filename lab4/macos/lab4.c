#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/file.h>
#include <sys/types.h>
#include <sys/stat.h>

// Функция для центрирования текста
void centerText(const char *text, int color) {
    printf("\033[2J"); // Очистить экран
    printf("\033[10;20H"); // Установить курсор в середину экрана
    printf("\033[%dm%s\033[0m\n", color, text); // Установить цвет и вывести текст
}

// Основная программа
int main() {
    const char *filename = "file.txt";
    int fd;
    struct flock lock;

    // Открытие файла
    fd = open(filename, O_RDWR);
    if (fd == -1) {
        centerText("Не удалось открыть файл", 31); // Красный текст
        perror("Ошибка открытия файла");
        exit(EXIT_FAILURE);
    }

    // Установка блокировки
    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;

    // Попытка установить блокировку
    if (fcntl(fd, F_SETLK, &lock) == -1) {
        if (errno == EACCES || errno == EAGAIN) {
            centerText("Блокировка не удалась, ожидание...", 33); // Желтый текст
            if (fcntl(fd, F_SETLKW, &lock) == -1) {
                centerText("Не удалось установить блокировку", 31); // Красный текст
                perror("Ошибка установки блокировки");
                close(fd);
                exit(EXIT_FAILURE);
            }
        } else {
            centerText("Ошибка установки блокировки", 31); // Красный текст
            perror("Ошибка блокировки");
            close(fd);
            exit(EXIT_FAILURE);
        }
    }
    centerText("Блокировка установлена успешно", 32); // Зеленый текст

    // Чтение данных из файла
    char buffer[1024];
    ssize_t bytes_read;
    lseek(fd, 0, SEEK_SET); // Установить позицию в начало файла
    while ((bytes_read = read(fd, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[bytes_read] = '\0';
        printf("%s", buffer);
    }

    // Задержка
    sleep(7);

    // Снятие блокировки
    lock.l_type = F_UNLCK;
    if (fcntl(fd, F_SETLK, &lock) == -1) {
        centerText("Ошибка снятия блокировки", 31); // Красный текст
        perror("Ошибка снятия блокировки");
    } else {
        centerText("Блокировка снята", 32); // Зеленый текст
    }

    close(fd);
    return 0;
}
