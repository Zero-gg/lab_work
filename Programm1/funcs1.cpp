//
// Created by Dmitriy K on 21.10.2025.
//

#include "funcs1.h"
#include <cstring>
#include <cstdio>
#include <cctype>
#include <locale>
#include <cstdlib>

// Именованные константы
// "teachers.txt" - файл для создания записей преподавателей по полям "Фамилия";"Должность";"Предмет"
// "exams.txt" - файл для создания записей экзаменов по полям "Предмет";"Группа";"Дата"
const std::string FILENAME_TEACHERS = "teachers.txt";
const std::string FILENAME_EXAMS = "exams.txt";

// Обработка всех возможных вариантов написания стоп-слова (регистронезависимая)
// Функция вручную переводит символы в верхний регистр, корректно обрабатывая кириллицу в кодировке Windows-1251
bool isStopWord(const char* str) {
    // Создаем копию строки для перевода в верхний регистр
    char temp[MAX_STRING_SIZE];
    strncpy(temp, str, sizeof(temp));
    temp[sizeof(temp) - 1] = '\0';

    // Переводим в верхний регистр (работает и для латиницы, и для кириллицы CP1251)
    for (int i = 0; temp[i]; i++) {
        unsigned char c = (unsigned char)temp[i];
        if (c >= 'a' && c <= 'z') {
            temp[i] = c - 'a' + 'A';
        } else if (c >= 0xE0 && c <= 0xFF) { // Кириллица 'а'-'я' в CP1251
            temp[i] = c - 0x20;
        } else if (c == 0xB8) { // 'ё' -> 'Ё'
            temp[i] = 0xA8;
        }
    }

    if (strcmp(temp, "STOP") == 0) return true;
    if (strcmp(temp, "СТОП") == 0) return true;
    
    return false;
}

void Main_Menu1() {
    int choice;
    bool menu_flag = true;
    while (true) {
        if (menu_flag) {
            system("cls");
            printf("=== Программа для записи данных о преподавателях и об экзаменах ===\n");
            printf("1. Открыть файл преподавателей (teachers.txt) на перезапись. Очистить его от предыдущих записей и внести новые.\n");
            printf("2. Открыть файл преподавателей (teachers.txt) на дозапись. Добавить новые поля к уже существующим.\n");
            printf("3. Открыть файл экзаменов (exams.txt) на перезапись. Очистить его от предыдущих записей и внести новые.\n");
            printf("4. Открыть файл экзаменов (exams.txt) на дозапись. Добавить новые поля к уже существующим.\n");
            printf("0. Закрыть программу.\n");
            menu_flag = false;
        }

        printf("\nВведите номер операции, которую хотите провести:\n");
        choice = safe_input_int(0, 4);
        if (choice == -1) return; // Аварийный выход, если ввод сломался

        system("cls");

        switch (choice) {
            case 1:
                Rewrite_Teachers();
                menu_flag = true;
                break;
            case 2:
                Append_Teachers();
                menu_flag = true;
                break;
            case 3:
                Rewrite_Exams();
                menu_flag = true;
                break;
            case 4:
                Append_Exams();
                menu_flag = true;
                break;
            case 0:
                printf("Выход из программы.\n");
                return;
            default:
                printf("Неверный ввод. Введите корректный номер операции.\n");
        }
    }
}

void Rewrite_Teachers() {
    FILE *output_file = fopen(FILENAME_TEACHERS.c_str(), "w");
    if (!output_file) {
        printf("Ошибка открытия файла.\n");
        return;
    }
    Teacher teacher;

    printf("\n=== Перезапись файла преподавателей ===\n");
    printf("(для остановки ввода и возврата в меню введите STOP или СТОП)\n\n");

    while (InputTeacher(&teacher)) {
        if (teacher.surname == "НЕТ ДАННЫХ" && teacher.post == "НЕТ ДАННЫХ" && teacher.subject == "НЕТ ДАННЫХ") {
            printf("Все поля были оставлены пустыми. Запись не добавлена.\n\n");
        } else {
            fprintf(output_file, "%s;%s;%s\n", teacher.surname.c_str(), teacher.post.c_str(), teacher.subject.c_str());
            printf("Успешно! Данные сохранены в файл \"%s\".\nФайл находится в той же папке, что и программа.\n\n", FILENAME_TEACHERS.c_str());
        }
    }
    fclose(output_file);
    printf("\nЗапись данных в файл завершена!\n");
    Pause_And_Return();
}

void Append_Teachers() {
    // Проверка на наличие новой строки в конце файла.
    // Если файл заканчивается не на символ новой строки, мы должны добавить его,
    // чтобы новая запись не "приклеилась" к предыдущей.
    FILE* check_file = fopen(FILENAME_TEACHERS.c_str(), "r");
    bool need_newline = false;
    if (check_file) {
        fseek(check_file, 0, SEEK_END);
        if (ftell(check_file) > 0) {
            fseek(check_file, -1, SEEK_END);
            if (fgetc(check_file) != '\n') {
                need_newline = true;
            }
        }
        fclose(check_file);
    }

    FILE *output_file = fopen(FILENAME_TEACHERS.c_str(), "a");
    if (!output_file) {
        printf("Ошибка открытия файла. Возможно он ещё не был создан. Выполните пункт 1.\n");
        return;
    }
    if (need_newline) {
        fprintf(output_file, "\n");
    }

    Teacher teacher;

    printf("\n=== Дозапись файла преподавателей ===\n");
    printf("(для остановки программы введите STOP/СТОП).\n\n");

    while (InputTeacher(&teacher)) {
        if (teacher.surname == "НЕТ ДАННЫХ" && teacher.post == "НЕТ ДАННЫХ" && teacher.subject == "НЕТ ДАННЫХ") {
            printf("Все поля пусты. Запись не добавлена.\n\n");
        } else {
            fprintf(output_file, "%s;%s;%s\n", teacher.surname.c_str(), teacher.post.c_str(), teacher.subject.c_str());
            printf("Запись успешно добавлена!\n\n");
        }
    }

    fclose(output_file);
    printf("\nВсе новые данные были добавлены в файл!\n");
    Pause_And_Return();
}


void Rewrite_Exams() {
    FILE *output_file = fopen(FILENAME_EXAMS.c_str(), "w");
    if (!output_file) {
        printf("Ошибка открытия файла.\n");
        return;
    }

    Exam exam;

    printf("\n=== Перезапись файла экзаменов ===\n");
    printf("(для остановки ввода и возврата в меню введите STOP или СТОП)\n\n");

    while (InputExam(&exam)) {
        if (exam.subject == "НЕТ ДАННЫХ" && exam.group == "НЕТ ДАННЫХ" && exam.date == "НЕТ ДАННЫХ") {
            printf("Все поля были оставлены пустыми. Запись не добавлена.\n\n");
        } else {
            fprintf(output_file, "%s;%s;%s\n", exam.subject.c_str(), exam.group.c_str(), exam.date.c_str());
            printf("Успешно! Данные сохранены в файл \"%s\".\nФайл находится в той же папке, что и программа.\n\n", FILENAME_EXAMS.c_str());
        }
    }

    fclose(output_file);
    printf("\nЗапись данных в файл завершена!\n");
    Pause_And_Return();
}

void Append_Exams() {
    // Аналогичная проверка на наличие новой строки, как и для преподавателей
    FILE* check_file = fopen(FILENAME_EXAMS.c_str(), "r");
    bool need_newline = false;
    if (check_file) {
        fseek(check_file, 0, SEEK_END);
        if (ftell(check_file) > 0) {
            fseek(check_file, -1, SEEK_END);
            if (fgetc(check_file) != '\n') {
                need_newline = true;
            }
        }
        fclose(check_file);
    }

    FILE *output_file = fopen(FILENAME_EXAMS.c_str(), "a");
    if (!output_file) {
        printf("Ошибка открытия файла. Возможно он ещё не был создан. Выполните пункт 1.\n");
        return;
    }
    if (need_newline) {
        fprintf(output_file, "\n");
    }

    Exam exam;

    printf("\n=== Дозапись файла экзаменов ===\n");
    printf("(для остановки программы введите STOP/СТОП)\n\n");

    while (InputExam(&exam)) {
        if (exam.subject == "НЕТ ДАННЫХ" && exam.group == "НЕТ ДАННЫХ" && exam.date == "НЕТ ДАННЫХ") {
            printf("Все поля пусты. Запись не добавлена.\n\n");
        } else {
            fprintf(output_file, "%s;%s;%s\n", exam.subject.c_str(), exam.group.c_str(), exam.date.c_str());
            printf("Запись успешно добавлена!\n\n");
        }
    }

    fclose(output_file);
    printf("\nВсе новые данные были добавлены в файл!\n");
    Pause_And_Return();
}

bool isCyrillic(unsigned char c) {
    return (c >= 0xC0 && c <= 0xFF) || c == 0xA8 || c == 0xB8;
}

// Проверка поля фамилии на правильность
bool isValidSurname(const char* str) {
    if (str[0] == '\0') {
        printf("! Ошибка: Поле не может быть пустым.\n");
        return false; 
    }
    if (strlen(str) < 2) {
        printf("! Ошибка: Фамилия слишком короткая (минимум 2 символа).\n");
        return false; 
    }
    
    // Проверка на дефис в начале или конце
    if (str[0] == '-' || str[strlen(str) - 1] == '-') {
        printf("! Ошибка: Фамилия не может начинаться или заканчиваться дефисом.\n");
        return false;
    }

    for (int i = 0; str[i] != '\0'; i++) {
        // (unsigned char) - это ВАЖНО для корректной работы isalpha
        // с кириллицей в кодировке 1251
        unsigned char c = (unsigned char)str[i];

        // Если символ - НЕ буква И НЕ дефис
        if (!isCyrillic(c) && c != '-') {
            printf("! Ошибка: Недопустимый символ '%c'. Фамилия должна содержать только русские буквы и дефис.\n", c);
            return false; 
        }
        
        // Проверка на двойной дефис
        if (c == '-' && str[i+1] == '-') {
            printf("! Ошибка: Недопустимо использование двух дефисов подряд.\n");
            return false;
        }
    }
    return true; // Если все символы в порядке
}

// Проверка поля должности на правильность
bool isValidPost(const char* str) {
    if (str[0] == '\0') {
        printf("! Ошибка: Поле не может быть пустым.\n");
        return false;
    }
    if (strlen(str) < 2) {
        printf("! Ошибка: Название должности слишком короткое (минимум 2 символа).\n");
        return false;
    }

    // Проверка на дефис в начале или конце
    if (str[0] == '-' || str[strlen(str) - 1] == '-') {
        printf("! Ошибка: Должность не может начинаться или заканчиваться дефисом.\n");
        return false;
    }

    for (int i = 0; str[i] != '\0'; i++) {
        unsigned char c = (unsigned char)str[i];
        if (!isCyrillic(c) && c != ' ' && c != '-') {
            printf("! Ошибка: Недопустимый символ '%c'. Должность должна содержать только русские буквы, пробелы и дефисы.\n", c);
            return false;
        }
        // Проверка на двойной дефис
        if (c == '-' && str[i+1] == '-') {
            printf("! Ошибка: Недопустимо использование двух дефисов подряд.\n");
            return false;
        }
    }
    return true;
}

// Проверка поля предмета на правильность
bool isValidSubject(const char* str) {
    if (str[0] == '\0') {
        printf("! Ошибка: Поле не может быть пустым.\n");
        return false;
    }
    if (strlen(str) < 2) {
        printf("! Ошибка: Название предмета слишком короткое (минимум 2 символа).\n");
        return false;
    }
    for (int i = 0; str[i] != '\0'; i++) {
        unsigned char c = (unsigned char)str[i];
        if (!isCyrillic(c) && c != ' ') {
            printf("! Ошибка: Недопустимый символ '%c'. Название предмета должно содержать только русские буквы и пробелы.\n", c);
            return false;
        }
    }
    return true;
}

// Проверка поля даты на правильность
// Проверяет формат (ДД.ММ.ГГГГ), допустимые символы, диапазоны чисел и корректность дней в месяце (включая високосные года)
bool isValidDate(const char* str) {
    if (str == NULL || strlen(str) != 10) {
        printf("! Ошибка: Неверная длина даты. Формат должен быть ДД.ММ.ГГГГ (10 символов).\n");
        return false; 
    }

    // 1. Проверяем формат: точки и цифры должны быть на своих местах
    for (int i = 0; i < 10; i++) {
        if (i == 2 || i == 5) { // Позиции для точек
            if (str[i] != '.') {
                printf("! Ошибка: Разделители должны быть точками (.). Пример: 15.01.2025\n");
                return false;
            }
        } else { // Позиции для цифр
            // Проверка, являются ли остальные символы цифрами
            if (!isdigit((unsigned char)str[i])) {
                printf("! Ошибка: Дата должна содержать только цифры и точки.\n");
                return false;
            }
        }
    }

    int day, month, year;

    // 2. Разбираем строку на числа
    if (sscanf(str, "%d.%d.%d", &day, &month, &year) != 3) {
        printf("! Ошибка: Не удалось распознать числа в дате.\n");
        return false; 
    }

    // 3. Проверяем базовые диапазоны

    // Установим разумный диапазон лет, например с 1900 по 2099
    if (year < 1900 || year > 2099) {
        printf("! Ошибка: Год %d выглядит нереалистично. Пожалуйста, введите год от 1900 до 2099.\n", year);
        return false;
    }
    if (month < 1 || month > 12) {
        printf("! Ошибка: Месяц должен быть от 1 до 12.\n");
        return false;
    }
    if (day < 1 || day > 31) {
        printf("! Ошибка: День должен быть от 1 до 31.\n");
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
            printf("! Ошибка: В феврале %d года не может быть больше %d дней.\n", year, max_days);
            return false;
        }
    }
    // --- Проверка 30-дневных месяцев (Апрель, Июнь, Сентябрь, Ноябрь) ---
    else if (month == 4 || month == 6 || month == 9 || month == 11) {
        if (day > 30) {
            printf("! Ошибка: В %d-м месяце всего 30 дней.\n", month);
            return false;
        }
    }
    return true; // Все проверки пройдены!
}

// Проверка поля номера группы на правильность
bool isValidGroupNumber(const char* str) {
    if (str[0] == '\0') {
        printf("! Ошибка: Поле не может быть пустым.\n");
        return false; 
    }

    // Проверка на дефис в начале или конце
    if (str[0] == '-' || str[strlen(str) - 1] == '-') {
        printf("! Ошибка: Номер группы не может начинаться или заканчиваться дефисом.\n");
        return false;
    }

    bool hasDigit = false;
    for (int i = 0; str[i] != '\0'; i++) {
        unsigned char c = (unsigned char)str[i];

        // Если символ - НЕ русская буква, НЕ цифра И НЕ дефис
        if (!isCyrillic(c) && !isdigit(c) && c != '-') {
            printf("! Ошибка: Недопустимый символ '%c'. Номер группы может содержать только русские буквы, цифры и дефис.\n", c);
            return false; 
        }
        
        // Проверка на двойной дефис
        if (c == '-' && str[i+1] == '-') {
            printf("! Ошибка: Недопустимо использование двух дефисов подряд.\n");
            return false;
        }

        if (isdigit(c)) {
            hasDigit = true;
        }
    }
    
    if (!hasDigit) {
        printf("! Ошибка: Номер группы должен содержать хотя бы одну цифру.\n");
        return false;
    }
    
    return true; 
}

// Функция безопасного ввода целого числа (защита от некорректного ввода)
// Эта функция сначала читает ввод как строку (текст), проверяет, что там только цифры
// в диапазоне допустимых значений [min_val, max_val].
// Также обрабатывает случаи переполнения буфера ввода.
int safe_input_int(int min_val, int max_val) {
    char line[32];
    while (true) {
        if (!fgets(line, sizeof(line), stdin)) {
            return -1; // EOF
        }

        // Проверка на переполнение буфера (если ввод слишком длинный)
        // Если в строке нет символа новой строки, значит ввод был длиннее буфера (или конец файла без перевода строки).
        if (strchr(line, '\n') == NULL) {
            // Очищаем поток ввода от лишних символов, чтобы они не попали в следующий ввод
            int c;
            while ((c = getchar()) != '\n' && c != EOF);
            
            printf("Ошибка: слишком длинный ввод. Пожалуйста, введите число от %d до %d.\n", min_val, max_val);
            continue;
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
            printf("Ошибка: введите целое число, а не текст.\n");
            continue;
        }

        int val = atoi(line);

        // Проверка диапазона
        if (val < min_val || val > max_val) {
            printf("Ошибка: введите число от %d до %d.\n", min_val, max_val);
            continue;
        }

        return val;
    }
}

// Функция для удаления пробелов в начале и конце строки
void trim(char* str) {
    if (!str) return;

    // Удаление пробелов в начале
    int start = 0;
    while (str[start] && isspace((unsigned char)str[start])) {
        start++;
    }

    if (start > 0) {
        int i = 0;
        while (str[start + i]) {
            str[i] = str[start + i];
            i++;
        }
        str[i] = '\0';
    }

    // Удаление пробелов в конце
    int len = strlen(str);
    while (len > 0 && isspace((unsigned char)str[len - 1])) {
        str[--len] = '\0';
    }
}

// Функция безопасного ввода строки
// Читает строку, удаляет \n, и очищает буфер ввода, если строка была слишком длинной.
// Это предотвращает попадание "хвоста" длинной строки в следующий ввод.
// Возвращает false, если произошла ошибка чтения (EOF).
bool safe_input_string(char* buffer, int size) {
    if (!fgets(buffer, size, stdin)) {
        buffer[0] = '\0';
        return false;
    }

    // Ищем символ новой строки
    char* newline = strchr(buffer, '\n');
    if (newline) {
        // Если нашли, заменяем его на конец строки
        *newline = '\0';
    } else {
        // Если не нашли, значит строка не влезла в буфер полностью.
        // Нужно очистить остаток ввода, чтобы он не попал в следующее поле.
        int c;
        while ((c = getchar()) != '\n' && c != EOF);
    }
    
    // Также удаляем возможный \r (для Windows)
    char* cr = strchr(buffer, '\r');
    if (cr) *cr = '\0';

    // Удаляем лишние пробелы по краям
    trim(buffer);
    return true;
}

// Функция нормализации фамилии (приведение к стандарту)
// Нормализация — это приведение данных к единому стандарту.
// Например, если пользователь ввел "иВаНоВ", мы превратим это в "Иванов".
// Это нужно для того, чтобы в файле все записи выглядели аккуратно и одинаково.
void normalizeSurname(char* str) {
    if (str[0] == '\0') {
        return;
    }
    // 1. Делаем первый символ заглавным
    str[0] = toupper((unsigned char)str[0]);
    // 2. Все остальные символы делаем строчными, кроме буквы после дефиса
    for (int i = 1; str[i] != '\0'; i++) {
        if (str[i-1] == '-') {
            str[i] = toupper((unsigned char)str[i]);
        } else if (str[i] != '-') {
            str[i] = tolower((unsigned char)str[i]);
        }
    }
}

// Функция нормализации текста (Должность, Предмет)
// Приводит к формату: Первая буква заглавная, остальные строчные.
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
}

// === Реализация функций ввода ===

bool InputTeacher(Teacher* t) {
    char buf[MAX_STRING_SIZE];
    bool retry = false;

    do {
        retry = false;

        // --- Фамилия ---
        while (true) {
            printf("Введите фамилию преподавателя: ");
            if (!safe_input_string(buf, sizeof(buf))) return false;

            if (isStopWord(buf)) return false;

            if (buf[0] == '\0') {
                printf("! Вы оставили поле пустым. Ему будет присвоено значение \"НЕТ ДАННЫХ\".\n");
                t->surname = "НЕТ ДАННЫХ";
                break;
            }
            // Проверка на спецсимволы и пустоту
            if (!isValidSurname(buf)) {
                printf("Ошибка: фамилия должна быть на русском языке и может содержать 1 дефис в случае с двойными фамилиями. \nПопробуйте еще раз.\n");
                continue;
            }
            normalizeSurname(buf);
            t->surname = buf;
            break;
        }

        // --- Должность ---
        while (true) {
            printf("Введите должность: ");
            if (!safe_input_string(buf, sizeof(buf))) return false;

            if (isStopWord(buf)) return false;

            if (buf[0] == '\0') {
                printf("! Вы оставили поле пустым. Ему будет присвоено значение \"НЕТ ДАННЫХ\".\n");
                t->post = "НЕТ ДАННЫХ";
                break;
            }

            if (!isValidPost(buf)) {
                printf("! Ошибка: должность должна быть на русском языке. Попробуйте еще раз.\n");
                continue;
            }

            // Ввод корректен
            normalizeSentenceCase(buf);
            t->post = buf;
            break;
        }

        // --- Предмет ---
        while (true) {
            printf("Введите название предмета: ");
            if (!safe_input_string(buf, sizeof(buf))) return false;

            if (isStopWord(buf)) return false;

            if (buf[0] == '\0') {
                printf("! Вы оставили поле пустым. Ему будет присвоено значение \"НЕТ ДАННЫХ\".\n");
                t->subject = "НЕТ ДАННЫХ";
                break;
            }

            if (!isValidSubject(buf)) {
                printf("! Ошибка: Название предмета должно быть на русском языке. Попробуйте еще раз.\n");
                continue;
            }
            // Ввод корректен
            normalizeSentenceCase(buf);
            t->subject = buf;
            break;
        }

        // --- Подтверждение ввода ---
        printf("\n--- Проверка введенных данных ---\n");
        printf("Фамилия:  %s\n", t->surname.c_str());
        printf("Должность: %s\n", t->post.c_str());
        printf("Предмет:   %s\n", t->subject.c_str());

        printf("\nВыберите действие:\n");
        printf("1 - Сохранить запись\n");
        printf("2 - Изменить данные\n");
        printf("Ваш выбор: ");

        int choice = safe_input_int(1, 2);
        if (choice == -1) return false; // Ошибка ввода или EOF

        if (choice == 1) {
            return true; // Сохранить
        } else if (choice == 2) {
            retry = true; // Повторить ввод
            printf("\n--- Повторный ввод данных ---\n");
        }

    } while (retry);

    return true;
}

bool InputExam(Exam* e) {
    char buf[MAX_STRING_SIZE];
    bool retry = false;

    do {
        retry = false;

        // --- Предмет ---
        while (true) {
            printf("Введите название предмета (без сокращений): ");
            if (!safe_input_string(buf, sizeof(buf))) return false;

            if (isStopWord(buf)) return false;

            if (buf[0] == '\0') {
                printf("! Вы оставили поле пустым. Ему будет присвоено значение \"НЕТ ДАННЫХ\".\n");
                e->subject = "НЕТ ДАННЫХ";
                break;
            }

            if (!isValidSubject(buf)) {
                printf("! Ошибка: Название предмета должно быть на русском языке. Попробуйте еще раз.\n");
                continue;
            }
            normalizeSentenceCase(buf);
            e->subject = buf;
            break;
        }

        // --- Группа ---
        while (true) {
            printf("Введите номер группы: ");
            if (!safe_input_string(buf, sizeof(buf))) return false;

            if (isStopWord(buf)) return false;

            if (buf[0] == '\0') {
                printf("! Вы оставили поле пустым. Ему будет присвоено значение \"НЕТ ДАННЫХ\".\n");
                e->group = "НЕТ ДАННЫХ";
                break;
            }

            if (!isValidGroupNumber(buf)) {
                printf("! Ошибка: Номер группы должен содержать русские буквы и цифры. Попробуйте еще раз.\n");
                continue;
            }
            normalizeGroupNumber(buf);
            e->group = buf;
            break;
        }

        // --- Дата ---
        while (true) {
            printf("Введите дату экзамена (в формате ДД.ММ.ГГГГ): ");
            if (!safe_input_string(buf, sizeof(buf))) return false;

            if (isStopWord(buf)) return false;

            if (buf[0] == '\0') {
                printf("! Вы оставили поле пустым. Ему будет присвоено значение \"НЕТ ДАННЫХ\".\n");
                e->date = "НЕТ ДАННЫХ";
                break;
            }

            if (!isValidDate(buf)) {
                // Сообщение об ошибке выводится внутри isValidDate
                continue;
            }
            e->date = buf;
            break;
        }

        // --- Подтверждение ввода ---
        printf("\n--- Проверка введенных данных ---\n");
        printf("Предмет: %s\n", e->subject.c_str());
        printf("Группа:  %s\n", e->group.c_str());
        printf("Дата:    %s\n", e->date.c_str());

        printf("\nВыберите действие:\n");
        printf("1 - Сохранить запись\n");
        printf("2 - Изменить данные\n");
        printf("Ваш выбор: ");

        int choice = safe_input_int(1, 2);
        if (choice == -1) return false; // Ошибка ввода или EOF

        if (choice == 1) {
            return true; // Сохранить
        } else if (choice == 2) {
            retry = true; // Повторить ввод
            printf("\n--- Повторный ввод данных ---\n");
        }

    } while (retry);

    return true;
}
