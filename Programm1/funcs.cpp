//
// Created by Dmitriy K on 21.10.2025.
//

#include "funcs.h"
#include <iostream>
#include <cstring>
#include <cstdio>
#include <cctype>
#include <locale>

using namespace std;

// Именованные константы
#define FILENAME_TEACHERS "teachers.txt"
#define FILENAME_EXAMS "exams.txt"

void ClearScreen() {
    system("cls");
}

void Main_Menu1() {
    int choice;
    bool menu_flag = true;
    while (true) {
        if (menu_flag) {
            ClearScreen();
            printf("=== Программа для записи данных о преподавателях и об экзаменах ===\n");
            printf("1. Открыть файл преподавателей на перезапись. Очистить его от предыдущих записей и внести новые.\n");
            printf("2. Открыть файл преподавателей на дозапись. Добавить новые поля к уже существующим.\n");
            printf("3. Открыть файл экзаменов на перезапись. Очистить его от предыдущих записей и внести новые.\n");
            printf("4. Открыть файл экзаменов на дозапись. Добавить новые поля к уже существующим.\n");
            printf("0. Закрыть программу.\n");
            menu_flag = false;
        }
        printf("\nВведите номер операции, которую хотите провести:\n");
        choice = safe_input_int();
        switch (choice) {
            case 1:
                ClearScreen();
                Rewrite_Teachers();
                menu_flag = true;
                break;
            case 2:
                ClearScreen();
                Append_Teachers();
                menu_flag = true;
                break;
            case 3:
                ClearScreen();
                Rewrite_Exams();
                menu_flag = true;
                break;
            case 4:
                ClearScreen();
                Append_Exams();
                menu_flag = true;
                break;
            case 0:
                ClearScreen();
                printf("Выход из программы.\n");
                return;
            default:
                printf("Неверный ввод. Введите корректный номер операции.\n");
        }
    }
}

void Rewrite_Teachers() {
    FILE *output_file = fopen(FILENAME_TEACHERS, "w");
    if (!output_file) {
        printf("Ошибка открытия файла.\n");
        return;
    }
    Teacher teacher;
    char buf[256];
     bool stop_flag = false; //флаг для остановки цикла по "стоп-слову"

    printf("\n=== Перезапись файла преподавателей ===\n");
    printf("(для остановки программы введите STOP/СТОП)\n\n");

    while (true) {

        // --- Фамилия ---
        while (true) {
            printf("Фамилия: ");
            // fgets для безопасного чтения строки
            fgets(buf, sizeof(buf), stdin);
            // удаляем символ переноса строки '\n', добавленный fgets
            buf[strcspn(buf, "\n")] = 0;

            if (strcmp(buf, "STOP") == 0 || strcmp(buf, "СТОП") == 0) {
                stop_flag = true;
                break;
            }
            if (buf[0] == '\0') {
                printf("Введённое поле было пустым. Присвоено значение \"НЕТ ДАННЫХ\".\n");
                teacher.surname = "НЕТ ДАННЫХ";
                break;
            }
            // Проверка на спецсимволы и пустоту
            if (!isValidSurname(buf)) {
                printf("Ошибка: фамилия может содержать только буквы (А-я) и дефис (-). Введите поле заново.\n");
                continue;
            }
            normalizeSurname(buf);
            teacher.surname = buf;
            break;
        }
        if (stop_flag) break;

        // --- Должность ---
        while (true) {
            printf("Должность: ");
            fgets(buf, sizeof(buf), stdin);
            buf[strcspn(buf, "\n")] = 0;

            if (strcmp(buf, "STOP") == 0 || strcmp(buf, "СТОП") == 0) {
                stop_flag = true;
                break;
            }

            if (buf[0] == '\0') {
                printf("! Поле было пустым. Присвоено значение \"НЕТ ДАННЫХ\".\n");
                teacher.post = "НЕТ ДАННЫХ";
                break;
            }

            if (!isValidPost(buf)) {
                printf("! Ошибка: Должность может содержать только буквы (А-я) и пробелы. Введите поле заново.\n");
                continue;
            }

            // Ввод корректен
            normalizeSentenceCase(buf);
            teacher.post = buf;
            break;
        }
        if (stop_flag) break;

        // --- Предмет ---
        while (true) {
            printf("Предмет: ");
            fgets(buf, sizeof(buf), stdin);
            buf[strcspn(buf, "\n")] = 0;

            if (strcmp(buf, "STOP") == 0 || strcmp(buf, "СТОП") == 0) {
                stop_flag = true;
                break;
            }

            if (buf[0] == '\0') {
                printf("! Поле было пустым. Присвоено значение \"НЕТ ДАННЫХ\".\n");
                teacher.subject = "НЕТ ДАННЫХ";
                break;
            }

            if (!isValidSubject(buf)) {
                printf("! Ошибка: Предмет может содержать только буквы (А-я) и пробелы. Введите поле заново.\n");
                continue;
            }
            // Ввод корректен
            normalizeSentenceCase(buf);
            teacher.subject = buf;
            break;
        }
        if (stop_flag) break;

        fprintf(output_file, "%s;%s;%s\n", teacher.surname.c_str(), teacher.post.c_str(), teacher.subject.c_str());
        printf("Запись успешно добавлена!\n\n");

    }
    fclose(output_file);
    printf("\nЗапись данных в файл завершена!\n");
    Pause_And_Return();
}

void Append_Teachers() {
    FILE *output_file = fopen(FILENAME_TEACHERS, "a");
    if (!output_file) {
        printf("Ошибка открытия файла. Возможно он ещё не был создан. Выполните пункт 1.\n");
        return;
    }
    Teacher teacher;
    char buf[256];
    bool stop_flag = false;

    printf("\n=== Дозапись файла преподавателей ===\n");
    printf("(для остановки программы введите STOP/СТОП).\n\n");

    while (true) {

        // --- Фамилия ---
        while (true) {
            printf("Фамилия: ");
            // fgets для безопасного чтения строки
            fgets(buf, sizeof(buf), stdin);
            // удаляем символ переноса строки '\n', добавленный fgets
            buf[strcspn(buf, "\n")] = 0;

            if (strcmp(buf, "STOP") == 0 || strcmp(buf, "СТОП") == 0) {
                stop_flag = true;
                break;
            }
            if (buf[0] == '\0') {
                printf("Введённое поле было пустым. Присвоено значение \"НЕТ ДАННЫХ\".\n");
                teacher.surname = "НЕТ ДАННЫХ";
                break;
            }
            // Проверка на спецсимволы и пустоту
            if (!isValidSurname(buf)) {
                printf("Ошибка: фамилия может содержать только буквы (А-я) и дефис (-). Введите поле заново.\n");
                continue;
            }
            normalizeSurname(buf);
            teacher.surname = buf;
            break;
        }
        if (stop_flag) break;

        // --- Должность ---
        while (true) {
            printf("Должность: ");
            fgets(buf, sizeof(buf), stdin);
            buf[strcspn(buf, "\n")] = 0;

            if (strcmp(buf, "STOP") == 0 || strcmp(buf, "СТОП") == 0) {
                stop_flag = true;
                break;
            }

            if (buf[0] == '\0') {
                printf(" Поле было пустым. Присвоено значение \"НЕТ ДАННЫХ\".\n");
                teacher.post = "НЕТ ДАННЫХ";
                break;
            }

            if (!isValidPost(buf)) {
                printf(" Ошибка: Должность может содержать только буквы (А-я) и пробелы. Введите поле заново.\n");
                continue;
            }

            // Ввод корректен
            normalizeSentenceCase(buf);
            teacher.post = buf;
            break;
        }
        if (stop_flag) break;

        // --- Предмет ---
        while (true) {
            printf("Предмет: ");
            fgets(buf, sizeof(buf), stdin);
            buf[strcspn(buf, "\n")] = 0;

            if (strcmp(buf, "STOP") == 0 || strcmp(buf, "СТОП") == 0) {
                stop_flag = true;
                break;
            }

            if (buf[0] == '\0') {
                printf("Поле было пустым. Присвоено значение \"НЕТ ДАННЫХ\".\n");
                teacher.subject = "НЕТ ДАННЫХ";
                break;
            }

            if (!isValidSubject(buf)) {
                printf("Ошибка: Предмет может содержать только буквы (А-я) и пробелы. Введите поле заново.\n");
                continue;
            }
            // Ввод корректен
            normalizeSentenceCase(buf);
            teacher.subject = buf;
            break;
        }
        if (stop_flag) break;

        fprintf(output_file, "%s;%s;%s\n", teacher.surname.c_str(), teacher.post.c_str(), teacher.subject.c_str());
        printf("Запись успешно добавлена!\n\n");
    }

    fclose(output_file);
    printf("\nВсе новые данные были добавлены в файл!\n");
    Pause_And_Return();
}


void Rewrite_Exams() {
    FILE *output_file = fopen(FILENAME_EXAMS, "w");
    if (!output_file) {
        printf("Ошибка открытия файла.\n");
        return;
    }

    Exam exam;
    char buf[256];
    bool stop_flag = false;

    printf("\nВведите данные экзаменов (для остановки программы введите STOP/СТОП)\n\n");

    while (true) {

        // --- Предмет ---
        while (true) {
            printf("Предмет (введите данное поле без сокращений до аббревиатур): ");
            fgets(buf, sizeof(buf), stdin);
            buf[strcspn(buf, "\n")] = 0;

            if (strcmp(buf, "STOP") == 0 || strcmp(buf, "СТОП") == 0) {
                stop_flag = true;
                break;
            }
            if (buf[0] == '\0') {
                printf("Введённое поле было пустым. Присвоено значение \"НЕТ ДАННЫХ\".\n");
                exam.subject = "НЕТ ДАННЫХ";
                break;
            }

            if (!isValidSubject(buf)) {
                printf("Ошибка: название предмета может содержать только буквы (А-я) и пробелы. Введите поле заново.\n");
                continue;
            }
            normalizeSentenceCase(buf);
            exam.subject = buf;
            break;
        }
        if (stop_flag) break;

        // --- Группа ---
        while (true) {
            printf("Номер группы: ");
            fgets(buf, sizeof(buf), stdin);
            buf[strcspn(buf, "\n")] = 0;

            if (strcmp(buf, "STOP") == 0 || strcmp(buf, "СТОП") == 0) {
                stop_flag = true;
                break;
            }

            if (buf[0] == '\0') {
                printf(" Поле было пустым. Присвоено значение \"НЕТ ДАННЫХ\".\n");
                exam.group = "НЕТ ДАННЫХ";
                break;
            }

            if (!isValidGroupNumber(buf)) {
                printf("Ошибка: Номер группы может содержать только буквы (А-я), цифры (0-9) и дефис (-). Введите поле заново.\n");
                continue;
            }
            normalizeGroupNumber(buf);
            exam.group = buf;
            break;
        }
        if (stop_flag) break;

        // --- Дата ---
        while (true) {
            printf("Дата (в формате ДД.ММ.ГГГГ): ");
            fgets(buf, sizeof(buf), stdin);
            buf[strcspn(buf, "\n")] = 0;

            if (strcmp(buf, "STOP") == 0 || strcmp(buf, "СТОП") == 0) {
                stop_flag = true;
                break;
            }

            if (buf[0] == '\0') {
                printf(" Поле было пустым. Присвоено значение \"НЕТ ДАННЫХ\".\n");
                exam.date = "НЕТ ДАННЫХ";
                break;
            }

            if (!isValidDate(buf)) {
                printf("Ошибка: Дата записана в неверном формате. Введите поле заново.\n");
                continue;
            }
            exam.date = buf;
            break;
        }
        if (stop_flag) break;

        fprintf(output_file, "%s;%s;%s\n", exam.subject.c_str(), exam.group.c_str(), exam.date.c_str());
        printf("Запись успешно добавлена!\n\n");
    }

    fclose(output_file);
    printf("\nЗапись данных в файл завершена!\n");
    Pause_And_Return();
}

void Append_Exams() {
    FILE *output_file = fopen("exams.txt", "a");
    if (!output_file) {
        printf("Ошибка открытия файла. Возможно он ещё не был создан. Выполните пункт 1.\n");
        return;
    }
    Exam exam;
    char buf[256];
    bool stop_flag = false;

    printf("\nВведите данные экзаменов для добавления (для остановки программы введите STOP/СТОП)\n\n");

    while (true) {

        // --- Предмет ---
        while (true) {
            printf("Предмет: ");
            fgets(buf, sizeof(buf), stdin);
            buf[strcspn(buf, "\n")] = 0;

            if (strcmp(buf, "STOP") == 0 || strcmp(buf, "СТОП") == 0) {
                stop_flag = true;
                break;
            }
            if (buf[0] == '\0') {
                printf("Введённое поле было пустым. Присвоено значение \"НЕТ ДАННЫХ\".\n");
                exam.subject = "НЕТ ДАННЫХ";
                break;
            }

            if (!isValidSubject(buf)) {
                printf("Ошибка: название предмета может содержать только буквы (А-я) и пробелы. Введите поле заново.\n");
                continue;
            }
            normalizeSentenceCase(buf);
            exam.subject = buf;
            break;
        }
        if (stop_flag) break;

        // --- Группа ---
        while (true) {
            printf("Номер группы: ");
            fgets(buf, sizeof(buf), stdin);
            buf[strcspn(buf, "\n")] = 0;

            if (strcmp(buf, "STOP") == 0 || strcmp(buf, "СТОП") == 0) {
                stop_flag = true;
                break;
            }

            if (buf[0] == '\0') {
                printf(" Поле было пустым. Присвоено значение \"НЕТ ДАННЫХ\".\n");
                exam.group = "НЕТ ДАННЫХ";
                break;
            }

            if (!isValidGroupNumber(buf)) {
                printf("Ошибка: Номер группы может содержать только буквы (А-я), цифры (0-9) и дефис (-). Введите поле заново.\n");
                continue;
            }
            normalizeGroupNumber(buf);
            exam.group = buf;
            break;
        }
        if (stop_flag) break;

        // --- Дата ---
        while (true) {
            printf("Дата (в формате ДД.ММ.ГГГГ): ");
            fgets(buf, sizeof(buf), stdin);
            buf[strcspn(buf, "\n")] = 0;

            if (strcmp(buf, "STOP") == 0 || strcmp(buf, "СТОП") == 0) {
                stop_flag = true;
                break;
            }

            if (buf[0] == '\0') {
                printf(" Поле было пустым. Присвоено значение \"НЕТ ДАННЫХ\".\n");
                exam.date = "НЕТ ДАННЫХ";
                break;
            }

            if (!isValidDate(buf)) {
                printf("Ошибка: Дата записана в неверном формате. Введите поле заново.\n");
                continue;
            }
            exam.date = buf;
            break;
        }
        if (stop_flag) break;

        fprintf(output_file, "%s;%s;%s\n", exam.subject.c_str(), exam.group.c_str(), exam.date.c_str());
        printf("Запись успешно добавлена!\n\n");
    }

    fclose(output_file);
    printf("\nВсе новые данные были добавлены в файл!\n");
    Pause_And_Return();
}

// Проверка поля фамилии на правильность
bool isValidSurname(const char* str) {
    if (str[0] == '\0') {
        return false; // Поле не может быть пустым
    }
    for (int i = 0; str[i] != '\0'; i++) {
        // (unsigned char) - это ВАЖНО для корректной работы isalpha
        // с кириллицей в кодировке 1251
        unsigned char c = (unsigned char)str[i];

        // Если символ - НЕ буква И НЕ дефис
        if (!isalpha(c) && c != '-') {
            return false; // Найден недопустимый символ
        }
    }
    return true; // Если все символы в порядке
}

// Проверка поля должности на правильность
bool isValidPost(const char* str) {
    if (str[0] == '\0') {
        return false;
    }
    for (int i = 0; str[i] != '\0'; i++) {
        unsigned char c = (unsigned char)str[i];
        if (!isalpha(c) && c != ' ') {
            return false;
        }
    }
    return true;
}

// Проверка поля предмета на правильность
bool isValidSubject(const char* str) {
    if (str[0] == '\0') {
        return false;
    }
    for (int i = 0; str[i] != '\0'; i++) {
        unsigned char c = (unsigned char)str[i];
        if (!isalpha(c) && c != ' ') {
            return false;
        }
    }
    return true;
}

// Проверка поля даты на правильность
bool isValidDate(const char* str) {
    if (str == NULL || strlen(str) != 10) {
        return false; // Дата должна быть ровно 10 символов: "ДД.ММ.ГГГГ"
    }

    // 1. Проверяем формат: точки и цифры должны быть на своих местах
    for (int i = 0; i < 10; i++) {
        if (i == 2 || i == 5) { // Позиции для точек
            if (str[i] != '.') {
                return false;
            }
        } else { // Позиции для цифр
            if (!isdigit((unsigned char)str[i])) {
                return false;
            }
        }
    }

    int day, month, year;

    // 2. Разбираем строку на числа
    // Мы уже проверили формат, так что sscanf сработает безопасно
    if (sscanf(str, "%d.%d.%d", &day, &month, &year) != 3) {
        return false; // Если что-то пошло не так при разборе
    }

    // 3. Проверяем базовые диапазоны

    // Установим разумный диапазон лет, например с 1900 по 2099
    if (year < 1900 || year > 2099) {
        return false;
    }
    if (month < 1 || month > 12) {
        return false;
    }
    if (day < 1 || day > 31) {
        return false;
    }

    // 4. Проверяем дни в месяце (с учетом високосного года)

    // --- Проверка на високосный год ---
    // Год високосный, если он делится на 4,
    // но не делится на 100, ИЛИ он делится на 400.
    bool is_leap = (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);

    // --- Проверка Февраля ---
    if (month == 2) {
        int max_days = is_leap ? 29 : 28;
        if (day > max_days) {
            return false;
        }
    }
    // --- Проверка 30-дневных месяцев (Апрель, Июнь, Сентябрь, Ноябрь) ---
    else if (month == 4 || month == 6 || month == 9 || month == 11) {
        if (day > 30) {
            return false;
        }
    }
    return true; // Все проверки пройдены!
}

// Проверка поля номера группы на правильность
bool isValidGroupNumber(const char* str) {
    if (str[0] == '\0') {
        return false; // Поле не может быть пустым
    }

    for (int i = 0; str[i] != '\0'; i++) {
        unsigned char c = (unsigned char)str[i];

        // Если символ - НЕ буква, НЕ цифра И НЕ дефис
        if (!isalnum(c) && c != '-') {
            return false; // Найден недопустимый символ (пробел, точка, @ и т.д.)
        }
    }
    return true; // Все символы в порядке
}

// Функция для безопасного выбора поля в главном меню
int safe_input_int() {
    char line[32];
    while (true) {
        if (!fgets(line, sizeof(line), stdin)) {
            return -1; // EOF
        }

        // удаляем \r\n
        line[strcspn(line, "\r\n")] = 0;

        // пустая строка — повторить ввод
        if (line[0] == '\0') continue;

        // проверяем, что строка содержит только цифры
        bool valid = true;
        for (int i = 0; line[i]; ++i) {
            if (!isdigit((unsigned char)line[i]) && !(i == 0 && line[i] == '-')) {
                valid = false;
                break;
            }
        }

        if (!valid) {
            printf("Ошибка: введите число.\n");
            continue;
        }

        return atoi(line);
    }
}

// Функция для приведения поля (Фамилия) к правильному формату
void normalizeSurname(char* str) {
    if (str[0] == '\0') {
        return;
    }
    // 1. Делаем первый символ заглавным
    str[0] = toupper((unsigned char)str[0]);
    // 2. Все остальные символы делаем строчными
    for (int i = 1; str[i] != '\0'; i++) {
        // Проверяем, что это не дефис, прежде чем приводить к нижнему регистру
        if (str[i] != '-') {
            str[i] = tolower((unsigned char)str[i]);
        }
    }
}

// Функция для приведения строк (Должность, Предмет) к правильному формату
// (Первая буква заглавная, все остальные строчные)
void normalizeSentenceCase(char* str) {
    if (str[0] == '\0') {
        return;
    }
    // 1. Делаем первый символ заглавным
    str[0] = toupper((unsigned char)str[0]);
    // 2. Все остальные символы (включая первые буквы других слов) делаем строчными,
    //    но пропускаем пробелы
    for (int i = 1; str[i] != '\0'; i++) {
        unsigned char c = (unsigned char)str[i];

        // Если это пробел, оставляем его как есть
        if (c == ' ') {
            continue;
        }
        // Все остальные символы (буквы) приводим к нижнему регистру
        str[i] = tolower(c);
    }
}

// Функция для приведения номера группы к верхнему регистру
void normalizeGroupNumber(char* str) {
    if (str[0] == '\0') {
        return;
    }

    for (int i = 0; str[i] != '\0'; i++) {
        // toupper() безопасно применить к цифрам и дефисам -
        // она их просто проигнорирует.
        str[i] = toupper((unsigned char)str[i]);
    }
}

//Функция для постановки программы на паузу
void Pause_And_Return() {
    printf("\nНажмите Enter для возврата в главное меню.\n");
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
    ClearScreen();
}
