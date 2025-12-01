//
// Created by Dmitriy K on 07.11.2025.
//

#include "funcs.h"
#include <iostream>
#include <cstdio>
#include <string>
#include <cstring>
#include <vector>
#include <map>
#include <set>
#include <cstdlib>
#include <cctype>


const std::string FILENAME_TEACHERS = "teachers.txt";
const std::string FILENAME_EXAMS = "exams.txt";
const std::string FILENAME_REPORT = "report.txt";
const std::string NO_DATA = "НЕТ ДАННЫХ";

void Main_Menu2() {


    int choice;
    bool menu_flag = true;
    while (true) {
        if (menu_flag) {
            ClearScreen();
            printf("=== Программа для формирования сведений на основе двух внешних файлов ===\n");
            printf("1. Сформировать сведения о приёме экзаменов по преподавателям.\n");
            printf("2. Избирательный вывод сведений по предмету на экран.\n");
            printf("0. Закрыть программу.\n");
            menu_flag = false;
        }

        printf("\nВведите номер операции, которую хотите провести:\n");
        choice = safe_input_int();

        switch (choice) {
            case 1:
                ClearScreen();
                File_Report();
                menu_flag = true;
                break;
            case 2:
                ClearScreen();
                Display_Subject_Data();
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

/**
 * [ПРОХОД 1]
 * Читает файл exams.txt и создает в памяти индекс.
 * Индекс: [Ключ: Предмет] -> [Значение: список смещений (long) в файле]
 */
void Build_Exam_Index(std::map<std::string, std::vector<long>>& index) {
    FILE* exams_file = fopen(FILENAME_EXAMS.c_str(), "rb");
    if (!exams_file) return;

    char line[512];
    long current_offset;
    current_offset = ftell(exams_file);

    while (fgets(line, sizeof(line), exams_file) != NULL) {
        char line_copy[512];
        strcpy(line_copy, line);

        // разделение строк по символу ;
        char *subject_token = strtok(line_copy, ";");

        // очистка от лишних символов  и добавление индекса:
        // ключ = предмет
        // значение = смещение строки
        if (subject_token) {
            Clean_Subject_Key(subject_token); // Очистка
            index[subject_token].push_back(current_offset);
        }
        // обновление смещения после прочтения строки
        current_offset = ftell(exams_file);
    }
    fclose(exams_file);
}

/**
 * [ПРОХОД 2]
 * Генерирует связанный отчет, используя метод индексации.
 */
void File_Report() {
    // 1. Создание Индекса Экзаменов
    std::map<std::string, std::vector<long>> exam_index;
    //printf("\n[ЭТАП 1] Создание индекса экзаменов...\n");
    Build_Exam_Index(exam_index);

    if (exam_index.empty()) {
        printf("! ОШИБКА: Индекс экзаменов пуст или файл не найден. Вероятно отсутствуют необходимые файлы.\n");
        Pause_And_Return();
        return;
    }

    // 2. Открытие файлов (с проверкой)
    FILE* teachers_file = fopen(FILENAME_TEACHERS.c_str(), "r");
    FILE* exams_file = fopen(FILENAME_EXAMS.c_str(), "rb");
    FILE* report_file = fopen(FILENAME_REPORT.c_str(), "w");

    if (!teachers_file || !exams_file || !report_file) {
        printf("! ОШИБКА: Не удалось открыть один из файлов для отчета.\n");
        if (teachers_file) fclose(teachers_file);
        if (exams_file) fclose(exams_file);
        if (report_file) fclose(report_file);
        Pause_And_Return();
        return;
    }

    // --- Заголовок ---
    fprintf(report_file, "%-40s | %-25s | %-25s | %-15s | %-12s\n",
            "ПРЕДМЕТ", "ПРЕПОДАВАТЕЛЬ", "ДОЛЖНОСТЬ", "ГРУППА", "ДАТА");
    fprintf(report_file, "-------------------------------------------------------------------------------------------------------------------------------\n");


    //printf("[ЭТАП 2] Генерация отчета (Проход 2: Преподаватели и сироты)...\n");

    char t_line[512];
    char e_line[512];

    std::set<std::string> subjects_with_teachers;
    int total_records = 0;

    // --- ЧАСТЬ А: Left Join (Преподаватели + Совпадения) ---
    while (fgets(t_line, sizeof(t_line), teachers_file) != NULL) {
        char t_copy[512];
        strcpy(t_copy, t_line);

        char *surname = strtok(t_copy, ";");
        char *post = strtok(NULL, ";");
        char *subject = strtok(NULL, ";");

        if (!surname && !post && !subject) continue;

        // --- Подготовка полей для вывода ---
        const char *out_surname = surname ? surname : NO_DATA.c_str();
        const char *out_post = post ? post : NO_DATA.c_str();
        const char *out_subject = subject ? subject : NO_DATA.c_str();

        // Ключ для поиска - обязательно очищен!
        char search_key[256];
        if (subject) {
            strcpy(search_key, subject);
            Clean_Subject_Key(search_key);
        } else {
            strcpy(search_key, NO_DATA.c_str());
        }

        subjects_with_teachers.insert(search_key);

        // 5. Поиск в индексе экзаменов
        if (exam_index.count(search_key)) {

            // --- СЛУЧАЙ А: СОВПАДЕНИЕ (INNER JOIN) ---
            std::vector<long>& offsets = exam_index[search_key];

            for (long offset : offsets) {
                fseek(exams_file, offset, SEEK_SET);

                if (fgets(e_line, sizeof(e_line), exams_file) != NULL) {
                    char e_copy[512];
                    strcpy(e_copy, e_line);

                    char *temp_subject = strtok(e_copy, ";"); // 1. Subject (Пропускаем)

                    char *ex_group = NULL;
                    char *ex_date = NULL;

                    if (temp_subject) {
                        ex_group = strtok(NULL, ";"); // 2. Группа
                        if (ex_group) {
                            ex_date = strtok(NULL, ";"); // 3. Дата
                        }
                    }

                    const char *out_group = ex_group ? ex_group : NO_DATA.c_str();
                    const char *out_date = ex_date ? ex_date : NO_DATA.c_str();

                    if (ex_date) ((char*)out_date)[strcspn(out_date, "\r\n")] = 0;

                    fprintf(report_file, "%-40s | %-25s | %-25s | %-15s | %-12s\n",
                            search_key, out_surname, out_post, out_group, out_date);
                    total_records++;
                }
            }
        } else {
            // --- СЛУЧАЙ Б: НЕТ СОВПАДЕНИЯ (Teacher Orphan) ---
            // Вывод преподавателя + NO_DATA для экзаменов
             fprintf(report_file, "%-40s | %-25s | %-25s | %-15s | %-12s\n",
                    search_key, out_surname, out_post, NO_DATA.c_str(), NO_DATA.c_str());
            total_records++;
        }
    }

    // --- ЧАСТЬ Б: Orphan Exams (Экзамены без преподавателей) ---
    //printf("[ЭТАП 3] Поиск экзаменов без преподавателей...\n");

    for (const auto& pair : exam_index) {
        const std::string& exam_subject = pair.first;

        // Если предмет экзамена НЕ найден в наборе преподаваемых предметов
        if (subjects_with_teachers.find(exam_subject) == subjects_with_teachers.end()) {

            // --- СЛУЧАЙ В: ЭКЗАМЕН-СИРОТА ---
            for (long offset : pair.second) {
                fseek(exams_file, offset, SEEK_SET);

                if (fgets(e_line, sizeof(e_line), exams_file) != NULL) {
                    char e_copy[512];
                    strcpy(e_copy, e_line);

                    strtok(e_copy, ";");
                    char *ex_group = strtok(NULL, ";");
                    char *ex_date = strtok(NULL, ";");

                    const char *out_group = ex_group ? ex_group : NO_DATA.c_str();
                    const char *out_date = ex_date ? ex_date : NO_DATA.c_str();

                    if (ex_date) ((char*)out_date)[strcspn(out_date, "\r\n")] = 0;

                    // Вывод строки с NO_DATA для преподавателей
                    fprintf(report_file, "%-40s | %-25s | %-25s | %-15s | %-12s\n",
                            exam_subject.c_str(), NO_DATA.c_str(), NO_DATA.c_str(), out_group, out_date);
                    total_records++;
                }
            }
        }
    }

    // --- Закрытие ---
    fclose(teachers_file);
    fclose(exams_file);
    fclose(report_file);

    printf("\nОтчет '%s' успешно сгенерирован!\n", FILENAME_REPORT.c_str());
    printf("   Всего записей в отчете: %d\n", total_records);
    Pause_And_Return();
}

    void Clean_Subject_Key(char* key) {
        if (!key) return;

        // 1. Удаляем \r (возврат каретки) и \n (новая строка)
        key[strcspn(key, "\r\n")] = 0;

        // 2. Удаляем все пробелы и табуляции в конце строки
        int len = strlen(key);
        while (len > 0 && (key[len - 1] == ' ' || key[len - 1] == '\t')) {
            key[len - 1] = '\0';
            len--;
        }
    }

void Display_Subject_Data() {
    // ---------------------------------------------------------
    // ЭТАП 1: СБОР И ВЫВОД ДОСТУПНЫХ ПРЕДМЕТОВ
    // ---------------------------------------------------------

    // 1. Создаем Индекс Экзаменов (чтобы получить предметы из exams.txt)
    std::map<std::string, std::vector<long>> exam_index;
    Build_Exam_Index(exam_index);

    if (exam_index.empty()) {
        printf("! ВНИМАНИЕ: Индекс экзаменов пуст или файл не найден.\n");
        // Продолжаем, так как могут быть предметы только у преподавателей
    }

    // 2. Используем set для автоматической сортировки и удаления дубликатов
    std::set<std::string> all_subjects;

    // Добавляем предметы из экзаменов
    for (const auto& pair : exam_index) {
        all_subjects.insert(pair.first);
    }

    // Добавляем предметы из преподавателей
    FILE* teachers_scan = fopen(FILENAME_TEACHERS.c_str(), "r");
    if (teachers_scan) {
        char scan_line[512];
        while (fgets(scan_line, sizeof(scan_line), teachers_scan) != NULL) {
            char copy[512];
            strcpy(copy, scan_line);

            // Нам нужен только 3-й токен (Предмет)
            char *surname = strtok(copy, ";");
            char *post = strtok(NULL, ";");
            char *subject = strtok(NULL, ";");

            if (subject) {
                char clean_sub[256];
                strcpy(clean_sub, subject);
                Clean_Subject_Key(clean_sub); // Обязательно чистим!

                // Добавляем в set (дубликаты сами отсеются)
                if (strlen(clean_sub) > 0 && strcmp(clean_sub, NO_DATA.c_str()) != 0) {
                    all_subjects.insert(clean_sub);
                }
            }
        }
        fclose(teachers_scan);
    }

    // 3. Вывод списка пользователю
    printf("\n=== СПИСОК ДОСТУПНЫХ ПРЕДМЕТОВ ===\n");
    if (all_subjects.empty()) {
        printf("Предметы не найдены.\n");
        Pause_And_Return();
        return;
    }

    int count = 1;
    for (const auto& subj : all_subjects) {
        printf("%d. %s\n", count++, subj.c_str());
    }
    printf("==================================\n");

    // ---------------------------------------------------------
    // ЭТАП 2: ЗАПРОС И ПОИСК
    // ---------------------------------------------------------

    char input_subject[256];

    // Цикл для корректного ввода предмета с проверкой
    while (true) {
        printf("\nВведите название ПРЕДМЕТА из списка выше для поиска:\n> ");

        if (fgets(input_subject, sizeof(input_subject), stdin) == NULL) {
            return;
        }

        // Очистка введенного ключа
        Clean_Subject_Key(input_subject);

        // Проверка на пустой ввод
        if (strlen(input_subject) == 0) {
            printf("Поиск отменен: Введено пустое название.\n");
            Pause_And_Return();
            return;
        }

        // Проверка на допустимые символы (только буквы А-я и пробелы)
        if (!isValidSubject(input_subject)) {
            printf("! Ошибка: Название предмета может содержать только буквы (А-я) и пробелы. Попробуйте снова.\n");
            continue;
        }

        // Если проверка пройдена, нормализуем и выходим из цикла
        normalizeSentenceCase(input_subject);
        break;
    }

    const char* search_key = input_subject;

    // 4. Открытие файлов для поиска
    FILE* teachers_file = fopen(FILENAME_TEACHERS.c_str(), "r");
    FILE* exams_file = fopen(FILENAME_EXAMS.c_str(), "rb"); // Бинарный для fseek!

    if (!teachers_file || !exams_file) {
        printf("! ОШИБКА: Не удалось открыть файлы для чтения.\n");
        if (teachers_file) fclose(teachers_file);
        if (exams_file) fclose(exams_file);
        Pause_And_Return();
        return;
    }

    printf("\nРЕЗУЛЬТАТЫ ПОИСКА ПО ЗАПРОСУ: '%s'\n", search_key);
    // Заголовок таблицы
    printf("%-40s | %-25s | %-25s | %-15s | %-12s\n",
           "ПРЕДМЕТ", "ПРЕПОДАВАТЕЛЬ", "ДОЛЖНОСТЬ", "ГРУППА", "ДАТА");
    printf("-------------------------------------------------------------------------------------------------------------------------------\n");


    bool record_found = false; // Флаг, нашли ли мы хоть что-то
    char t_line[512];
    char e_line[512];

    // --- А. ИЩЕМ ПРЕПОДАВАТЕЛЕЙ ПО ЭТОМУ ПРЕДМЕТУ ---
    while (fgets(t_line, sizeof(t_line), teachers_file) != NULL) {
        char t_copy[512];
        strcpy(t_copy, t_line);

        char *surname = strtok(t_copy, ";");
        char *post = strtok(NULL, ";");
        char *subject = strtok(NULL, ";");

        if (subject) {
            char current_key[256];
            strcpy(current_key, subject);
            Clean_Subject_Key(current_key);

            // Сравниваем предмет из файла с введенным
            if (strcmp(current_key, search_key) == 0) {

                const char *out_surname = surname ? surname : NO_DATA.c_str();
                const char *out_post = post ? post : NO_DATA.c_str();

                // Проверяем, есть ли экзамены для этого предмета
                if (exam_index.count(search_key)) {
                    std::vector<long>& offsets = exam_index[search_key];

                    for (long offset : offsets) {
                        fseek(exams_file, offset, SEEK_SET);
                        if (fgets(e_line, sizeof(e_line), exams_file) != NULL) {
                            char e_copy[512];
                            strcpy(e_copy, e_line);

                            // Парсинг экзамена (надежный метод)
                            char *temp_sub = strtok(e_copy, ";");
                            char *ex_group = NULL;
                            char *ex_date = NULL;
                            if (temp_sub) {
                                ex_group = strtok(NULL, ";");
                                if (ex_group) ex_date = strtok(NULL, ";");
                            }

                            const char *out_group = ex_group ? ex_group : NO_DATA.c_str();
                            const char *out_date = ex_date ? ex_date : NO_DATA.c_str();
                            if (ex_date) ((char*)out_date)[strcspn(out_date, "\r\n")] = 0;

                            printf("%-40s | %-25s | %-25s | %-15s | %-12s\n",
                                    search_key, out_surname, out_post, out_group, out_date);
                            record_found = true;
                        }
                    }
                } else {
                    // Преподаватель есть, экзаменов нет
                    printf("%-40s | %-25s | %-25s | %-15s | %-12s\n",
                            search_key, out_surname, out_post, NO_DATA.c_str(), NO_DATA.c_str());
                    record_found = true;
                }
            }
        }
    }

    // --- Б. ИЩЕМ ЭКЗАМЕНЫ-СИРОТЫ (Если преподавателей по этому предмету не нашли вообще) ---
    // Логика: если мы прошли весь teachers.txt и не вывели этот предмет,
    // но он есть в exam_index, значит это экзамен без преподавателя.

    // Проверка: есть ли этот ключ в индексе экзаменов
    if (exam_index.count(search_key)) {
        // Мы могли уже вывести их выше в блоке "А" (если преподаватель нашелся).
        // Но если record_found == false, значит преподавателей не было,
        // а экзамены в индексе есть. Выводим их.

        if (!record_found) {
             std::vector<long>& offsets = exam_index[search_key];
             for (long offset : offsets) {
                fseek(exams_file, offset, SEEK_SET);
                if (fgets(e_line, sizeof(e_line), exams_file) != NULL) {
                    char e_copy[512];
                    strcpy(e_copy, e_line);

                    char *temp_sub = strtok(e_copy, ";");
                    char *ex_group = NULL;
                    char *ex_date = NULL;
                    if (temp_sub) {
                        ex_group = strtok(NULL, ";");
                        if (ex_group) ex_date = strtok(NULL, ";");
                    }

                    const char *out_group = ex_group ? ex_group : NO_DATA.c_str();
                    const char *out_date = ex_date ? ex_date : NO_DATA.c_str();
                    if (ex_date) ((char*)out_date)[strcspn(out_date, "\r\n")] = 0;

                    printf("%-40s | %-25s | %-25s | %-15s | %-12s\n",
                           search_key, NO_DATA.c_str(), NO_DATA.c_str(), out_group, out_date);
                    record_found = true;
                }
             }
        }
    }

    // Итог
    if (!record_found) {
        printf("Информации по предмету '%s' не найдено.\n", search_key);
    }

    fclose(teachers_file);
    fclose(exams_file);
    Pause_And_Return();
}

bool Check_File_Availability() {
    FILE* teachers_file = NULL;
    FILE* exams_file = NULL;

    // 1. Пытаемся открыть оба файла в режиме "чтения" ("r")
    teachers_file = fopen(FILENAME_TEACHERS.c_str(), "r");
    exams_file = fopen(FILENAME_EXAMS.c_str(), "r");

    // 2. Анализируем, что удалось открыть
    bool teachers_exist = (teachers_file != NULL);
    bool exams_exist = (exams_file != NULL);

    // 3. Закрываем файлы, если они открылись
    if (teachers_exist) {
        fclose(teachers_file);
    }
    if (exams_exist) {
        fclose(exams_file);
    }

    // 4. Проверки на то, какого файла не достаёт
    // Случай 1: Оба файла есть
    if (teachers_exist && exams_exist) {
        printf("\nСтатус: Оба файла (%s и %s) найдены.\n", FILENAME_TEACHERS.c_str(), FILENAME_EXAMS.c_str());
        printf("Можно приступать к работе с файлами.\n");
        Pause_And_Return();
        return true;
    }
    // Случай 2: Ни одного файла нет
    else if (!teachers_exist && !exams_exist) {
        printf("\nОШИБКА: Файлы %s и %s не найдены.\n", FILENAME_TEACHERS.c_str(), FILENAME_EXAMS.c_str());
        printf("Пожалуйста, скопируйте оба файла (%s и %s) в каталог программы.\n", FILENAME_TEACHERS.c_str(), FILENAME_EXAMS.c_str());
        Pause_And_Return();
        return false;
    }
    // Случай 3: Нет файла преподавателей
    else if (!teachers_exist) {
        printf("\nОШИБКА: Файл %s не найден.\n", FILENAME_TEACHERS.c_str());
        printf("Файл %s найден. Пожалуйста, добавьте недостающий файл %s.\n", FILENAME_EXAMS.c_str(), FILENAME_TEACHERS.c_str());
        Pause_And_Return();
        return false;
    }
    // Случай 4: Нет файла экзаменов
    else {
        printf("\nОШИБКА: Файл %s не найден.\n", FILENAME_EXAMS.c_str());
        printf("Файл %s найден. Пожалуйста, добавьте недостающий файл %s.\n", FILENAME_TEACHERS.c_str(), FILENAME_EXAMS.c_str());
        Pause_And_Return();
        return false;
    }
}

void Pause_And_Return() {
    printf("\nНажмите Enter для возврата в главное меню.\n");
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void ClearScreen() {
    fflush(stdout);
    system("cls");
}

int safe_input_int() {
    char line[32];
    while (true) {
        if (!fgets(line, sizeof(line), stdin)) {
            return -1; // EOF
        }

        // удаляем \r\n
        line[strcspn(line, "\r\n")] = 0;

        // пустая строка — просто игнорируем без сообщения
        if (line[0] == '\0') {
            continue;
        }

        // проверяем, что строка содержит только цифры (или минус в начале)
        bool valid = true;
        for (int i = 0; line[i]; ++i) {
            if (!isdigit((unsigned char)line[i]) && !(i == 0 && line[i] == '-')) {
                valid = false;
                break;
            }
        }

        if (!valid) {
            printf("Введите номер операции, которую хотите провести:\n");
            continue;
        }

        return atoi(line);
    }
}

// Проверка поля предмета на правильность
bool isValidSubject(const char* str) {
    if (str[0] == '\0') {
        return false; // Поле не может быть пустым
    }
    for (int i = 0; str[i] != '\0'; i++) {
        unsigned char c = (unsigned char)str[i];
        // Разрешены только буквы и пробелы
        if (!isalpha(c) && c != ' ') {
            return false; // Найден недопустимый символ
        }
    }
    return true; // Все символы в порядке
}

// Функция для приведения строк (Предмет) к правильному формату
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

