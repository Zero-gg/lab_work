//
// Created by Dmitriy K on 07.11.2025.
//

#include "funcs2.h"
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

// Главное меню программы
// Предоставляет пользователю выбор действий: генерация отчета или поиск по предмету.
void Main_Menu2() {
    // Проверка наличия файлов перед запуском меню
    if (!Check_File_Availability()) {
        return;
    }

    int choice;
    bool menu_flag = true;
    while (true) {
        if (menu_flag) {
            system("cls");
            printf("=== Программа для формирования сведений на основе двух внешних файлов ===\n");
            printf("1. Сформировать сведения о приёме экзаменов по преподавателям.\n");
            printf("2. Избирательный вывод сведений по предмету на экран.\n");
            printf("0. Закрыть программу.\n");
            menu_flag = false;
        }

        printf("\nВведите номер операции, которую хотите провести:\n");
        choice = safe_input_int(0, 2);

        if (choice == -1) {
            return;
        }

        system("cls");

        switch (choice) {
            case 1:
                File_Report();
                menu_flag = true;
                break;
            case 2:
                Display_Subject_Data();
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

// Вспомогательная функция для получения токена с учетом пустых полей
// Заменяет стандартный strtok, который пропускает пустые поля (например, ";;").
// Эта функция корректно возвращает пустую строку, если между разделителями нет данных.
char* get_token(char** str, const char* delim) {
    if (*str == NULL) return NULL;
    char* token = *str;
    char* end = strpbrk(token, delim);
    if (end) {
        *end = '\0';
        *str = end + 1;
    } else {
        *str = NULL;
    }
    return token;
}

/**
 * [ПРОХОД 1: ИНДЕКСАЦИЯ]
 * Читает файл exams.txt и создает в памяти индекс для быстрого поиска.
 * Индекс представляет собой карту (map), где:
 * - Ключ: Название предмета (std::string)
 * - Значение: Список позиций (смещений) в файле, где находятся записи об этом предмете.
 * Это позволяет не читать файл целиком каждый раз при поиске.
 */
void Build_Exam_Index(std::map<std::string, std::vector<long>>& index) {
    FILE* exams_file = fopen(FILENAME_EXAMS.c_str(), "rb");
    if (!exams_file) return;

    char line[512]; // Буфер для чтения строки из файла (512 байт достаточно для типичной строки CSV)
    long current_offset;
    current_offset = ftell(exams_file);

    while (fgets(line, sizeof(line), exams_file) != NULL) {
        char line_copy[512]; // Копия строки для безопасной обработки (strtok/get_token модифицирует строку)
        strcpy(line_copy, line);

        // Используем get_token вместо strtok
        char* ptr = line_copy;
        char* subject_token = get_token(&ptr, ";");

        // очистка от лишних символов  и добавление индекса:
        // ключ = предмет
        // значение = смещение строки
        if (subject_token && *subject_token) {
            Clean_Subject_Key(subject_token); // Очистка
            index[subject_token].push_back(current_offset);
        }
        // обновление смещения после прочтения строки
        current_offset = ftell(exams_file);
    }
    fclose(exams_file);
}

/**
 * [ПРОХОД 2: ГЕНЕРАЦИЯ ОТЧЕТА]
 * Генерирует сводный отчет, объединяя данные из teachers.txt и exams.txt.
 * Алгоритм работы:
 * 1. Строится индекс экзаменов (Build_Exam_Index).
 * 2. Читается файл преподавателей. Для каждого преподавателя ищутся экзамены по его предмету (LEFT JOIN).
 * 3. Если экзамены найдены, они выводятся в отчет. Если нет - выводится "НЕТ ДАННЫХ".
 * 4. После обработки всех преподавателей, ищутся "экзамены-сироты" (предметы, которые есть в экзаменах, но нет у преподавателей).
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
    fprintf(report_file, "%-43s | %-25s | %-25s | %-15s | %-12s\n",
            "ПРЕДМЕТ", "ПРЕПОДАВАТЕЛЬ", "ДОЛЖНОСТЬ", "ГРУППА", "ДАТА");
    fprintf(report_file, "----------------------------------------------------------------------------------------------------------------------------------\n");


    //printf("[ЭТАП 2] Генерация отчета (Проход 2: Преподаватели и сироты)...\n");

    char t_line[512]; // Буфер для чтения строки преподавателя (512 байт с запасом)
    char e_line[512]; // Буфер для чтения строки экзамена

    std::set<std::string> subjects_with_teachers;
    int total_records = 0;

    // --- ЧАСТЬ А: Left Join (Преподаватели + Совпадения) ---
    while (fgets(t_line, sizeof(t_line), teachers_file) != NULL) {
        char t_copy[512]; // Копия строки преподавателя для парсинга
        strcpy(t_copy, t_line);

        // Используем get_token для корректной обработки пустых полей
        char* ptr = t_copy;
        char* surname = get_token(&ptr, ";");
        char* post = get_token(&ptr, ";");
        char* subject = get_token(&ptr, ";");

        // Если строка пустая или некорректная - пропускаем
        if ((!surname || !*surname) && (!post || !*post) && (!subject || !*subject)) continue;

        // --- Подготовка полей для вывода ---
        // Если поле пустое, заменяем на NO_DATA
        const char *out_surname = (surname && *surname) ? surname : NO_DATA.c_str();
        const char *out_post = (post && *post) ? post : NO_DATA.c_str();
        const char *out_subject = (subject && *subject) ? subject : NO_DATA.c_str();

        // Ключ для поиска - обязательно очищен!
        char search_key[256]; // Буфер для ключа поиска (предмета). 256 байт достаточно для названия.
        if (subject && *subject) {
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
                    char e_copy[512]; // Копия строки экзамена для парсинга
                    strcpy(e_copy, e_line);

                    char* ptr = e_copy;
                    char* temp_subject = get_token(&ptr, ";"); // 1. Subject (Пропускаем)
                    char* ex_group = get_token(&ptr, ";");     // 2. Группа
                    char* ex_date = get_token(&ptr, ";");      // 3. Дата

                    const char *out_group = (ex_group && *ex_group) ? ex_group : NO_DATA.c_str();
                    const char *out_date = (ex_date && *ex_date) ? ex_date : NO_DATA.c_str();

                    if (ex_date) ((char*)out_date)[strcspn(out_date, "\r\n")] = 0;

                    fprintf(report_file, "%-43s | %-25s | %-25s | %-15s | %-12s\n",
                            search_key, out_surname, out_post, out_group, out_date);
                    total_records++;
                }
            }
        } else {
            // --- СЛУЧАЙ Б: НЕТ СОВПАДЕНИЯ (Teacher Orphan) ---
            // Вывод преподавателя + NO_DATA для экзаменов
            fprintf(report_file, "%-43s | %-25s | %-25s | %-15s | %-12s\n",
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

                    char* ptr = e_copy;
                    get_token(&ptr, ";"); // Пропускаем предмет
                    char* ex_group = get_token(&ptr, ";");
                    char* ex_date = get_token(&ptr, ";");

                    const char *out_group = (ex_group && *ex_group) ? ex_group : NO_DATA.c_str();
                    const char *out_date = (ex_date && *ex_date) ? ex_date : NO_DATA.c_str();

                    if (ex_date) ((char*)out_date)[strcspn(out_date, "\r\n")] = 0;

                    // Вывод строки с NO_DATA для преподавателей
                    fprintf(report_file, "%-43s | %-25s | %-25s | %-15s | %-12s\n",
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

    printf("\nУспешно! Отчет сохранен в файл '%s'.\n", FILENAME_REPORT.c_str());
    printf("Файл находится в той же папке, что и программа.\n");
    printf("Всего записей в отчете: %d\n", total_records);
    Pause_And_Return();
}

    // Очистка ключа (названия предмета) от лишних символов.
    // Удаляет символы переноса строки (\r, \n) и пробелы в конце строки.
    // Это необходимо для корректного сравнения строк (ключей map).
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

// Функция для выборочного вывода сведений по предмету на экран.
// Работает в два этапа:
// 1. Сканирует файлы и собирает список всех уникальных предметов.
// 2. Позволяет пользователю выбрать предмет и выводит всю связанную информацию (преподаватели + экзамены).
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
        if (pair.first != NO_DATA) {
            all_subjects.insert(pair.first);
        }
    }

    // Добавляем предметы из преподавателей
    FILE* teachers_scan = fopen(FILENAME_TEACHERS.c_str(), "r");
    if (teachers_scan) {
        char scan_line[512]; // Буфер для сканирования файла преподавателей
        while (fgets(scan_line, sizeof(scan_line), teachers_scan) != NULL) {
            char copy[512]; // Копия строки для извлечения предмета
            strcpy(copy, scan_line);

            // Нам нужен только 3-й токен (Предмет)
            char* ptr = copy;
            char* surname = get_token(&ptr, ";");
            char* post = get_token(&ptr, ";");
            char* subject = get_token(&ptr, ";");

            if (subject) {
                char clean_sub[256]; // Буфер для очищенного названия предмета
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

    // Переносим в вектор для доступа по индексу
    std::vector<std::string> subject_list(all_subjects.begin(), all_subjects.end());

    for (size_t i = 0; i < subject_list.size(); ++i) {
        printf("%d. %s\n", (int)(i + 1), subject_list[i].c_str());
    }
    printf("==================================\n");

    // ---------------------------------------------------------
    // ЭТАП 2: ЗАПРОС И ПОИСК
    // ---------------------------------------------------------

    printf("\nВведите НОМЕР предмета из списка выше для поиска:\n");
    int choice = safe_input_int(1, (int)subject_list.size());

    if (choice == -1) {
        return;
    }

    // Получаем строку предмета по индексу (choice - 1)
    std::string selected_subject = subject_list[choice - 1];
    const char* search_key = selected_subject.c_str();

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
    printf("%-43s | %-25s | %-25s | %-15s | %-12s\n",
        "ПРЕДМЕТ", "ПРЕПОДАВАТЕЛЬ", "ДОЛЖНОСТЬ", "ГРУППА", "ДАТА");
    printf("----------------------------------------------------------------------------------------------------------------------------------\n");


    bool record_found = false; // Флаг, нашли ли мы хоть что-то
    char t_line[512]; // Буфер для чтения строки преподавателя при поиске
    char e_line[512]; // Буфер для чтения строки экзамена при поиске

    // --- А. ИЩЕМ ПРЕПОДАВАТЕЛЕЙ ПО ЭТОМУ ПРЕДМЕТУ ---
    while (fgets(t_line, sizeof(t_line), teachers_file) != NULL) {
        char t_copy[512]; // Копия строки преподавателя
        strcpy(t_copy, t_line);

        char* ptr = t_copy;
        char* surname = get_token(&ptr, ";");
        char* post = get_token(&ptr, ";");
        char* subject = get_token(&ptr, ";");

        if (subject) {
            char current_key[256]; // Буфер для текущего предмета из файла
            strcpy(current_key, subject);
            Clean_Subject_Key(current_key);

            // Сравниваем предмет из файла с введенным
            if (strcmp(current_key, search_key) == 0) {

                const char *out_surname = (surname && *surname) ? surname : NO_DATA.c_str();
                const char *out_post = (post && *post) ? post : NO_DATA.c_str();

                // Проверяем, есть ли экзамены для этого предмета
                if (exam_index.count(search_key)) {
                    std::vector<long>& offsets = exam_index[search_key];

                    for (long offset : offsets) {
                        fseek(exams_file, offset, SEEK_SET);
                        if (fgets(e_line, sizeof(e_line), exams_file) != NULL) {
                            char e_copy[512]; // Копия строки экзамена
                            strcpy(e_copy, e_line);

                            // Парсинг экзамена (надежный метод)
                            char* ptr_e = e_copy;
                            char* temp_sub = get_token(&ptr_e, ";");
                            char* ex_group = get_token(&ptr_e, ";");
                            char* ex_date = get_token(&ptr_e, ";");

                            const char *out_group = (ex_group && *ex_group) ? ex_group : NO_DATA.c_str();
                            const char *out_date = (ex_date && *ex_date) ? ex_date : NO_DATA.c_str();
                            if (ex_date) ((char*)out_date)[strcspn(out_date, "\r\n")] = 0;

                            printf("%-43s | %-25s | %-25s | %-15s | %-12s\n",
                                    search_key, out_surname, out_post, out_group, out_date);
                            record_found = true;
                        }
                    }
                } else {
                    // Преподаватель есть, экзаменов нет
                    printf("%-43s | %-25s | %-25s | %-15s | %-12s\n",
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

                    char* ptr = e_copy;
                    char* temp_sub = get_token(&ptr, ";");
                    char* ex_group = get_token(&ptr, ";");
                    char* ex_date = get_token(&ptr, ";");

                    const char *out_group = (ex_group && *ex_group) ? ex_group : NO_DATA.c_str();
                    const char *out_date = (ex_date && *ex_date) ? ex_date : NO_DATA.c_str();
                    if (ex_date) ((char*)out_date)[strcspn(out_date, "\r\n")] = 0;

                    printf("%-43s | %-25s | %-25s | %-15s | %-12s\n",
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

// Проверка доступности необходимых файлов (teachers.txt и exams.txt).
// Возвращает true, если оба файла существуют и доступны для чтения.
// В противном случае выводит сообщение об ошибке и возвращает false.
bool Check_File_Availability() {
    // 1. Пытаемся открыть оба файла в режиме чтения
    FILE* teachers_file = fopen(FILENAME_TEACHERS.c_str(), "r");
    FILE* exams_file = fopen(FILENAME_EXAMS.c_str(), "r");

    // 2. Проверяем успешность открытия
    bool teachers_exist = (teachers_file != NULL);
    bool exams_exist = (exams_file != NULL);

    // 3. Сразу закрываем файлы, так как нам нужно только проверить их наличие
    if (teachers_file) fclose(teachers_file);
    if (exams_file) fclose(exams_file);

    // 4. Если хотя бы одного файла нет - сообщаем об ошибке
    if (!teachers_exist || !exams_exist) {
        printf("\nОШИБКА: Не найдены необходимые файлы с данными!\n");
        printf("Пожалуйста, убедитесь, что файлы '%s' и '%s' находятся в папке с программой.\n", 
               FILENAME_TEACHERS.c_str(), FILENAME_EXAMS.c_str());
        
        printf("\nНажмите Enter для выхода из программы.\n");
        int c;
        while ((c = getchar()) != '\n' && c != EOF);
        
        return false;
    }

    return true;
}

// Функция паузы перед возвратом в меню.
// Ожидает нажатия клавиши Enter пользователем.
void Pause_And_Return() {
    printf("\nНажмите Enter для возврата в главное меню.\n");
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

/**
 * Функция безопасного ввода целого числа.
 * Защищает программу от краха при вводе текста вместо цифр и от переполнения буфера.
 *
 * param min_val Минимальное допустимое значение.
 * param max_val Максимальное допустимое значение.
 * return Введенное число или -1 при ошибке потока.
 */
int safe_input_int(int min_val, int max_val) {
    char line[32]; // Буфер для чтения строки (32 байта достаточно для любого числа int)
    while (true) {
        if (!fgets(line, sizeof(line), stdin)) {
            return -1; // EOF
        }

        // --- ЗАЩИТА ОТ ПЕРЕПОЛНЕНИЯ БУФЕРА ---
        // Если пользователь ввел строку длиннее, чем вмещает line (32 байта),
        // то символ новой строки '\n' не будет прочитан.
        // Оставшиеся символы останутся во входном потоке (stdin) и "сломают" следующий ввод.
        if (strchr(line, '\n') == NULL) {
            // Очищаем поток ввода: читаем и выбрасываем символы, пока не встретим конец строки
            int c;
            while ((c = getchar()) != '\n' && c != EOF);
            
            printf("Ошибка: слишком длинный ввод. Пожалуйста, введите число от %d до %d.\n", min_val, max_val);
            continue;
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

// Проверка, является ли символ буквой кириллицы в кодировке Windows-1251.
// Включает основной диапазон (0xC0-0xFF) и буквы Ё/ё.
bool isCyrillic(unsigned char c) {
    return (c >= 0xC0 && c <= 0xFF) || c == 0xA8 || c == 0xB8;
}

// Проверка корректности названия предмета.
// Предмет не может быть пустым, короче 2 символов и должен содержать только кириллицу и пробелы.
bool isValidSubject(const char* str) {
    if (str[0] == '\0') {
        printf("! Ошибка: Поле не может быть пустым.\n");
        return false; // Поле не может быть пустым
    }
    if (strlen(str) < 2) {
        printf("! Ошибка: Название предмета слишком короткое (минимум 2 символа).\n");
        return false; 
    }
    for (int i = 0; str[i] != '\0'; i++) {
        unsigned char c = (unsigned char)str[i];
        // Разрешены только буквы и пробелы
        if (!isCyrillic(c) && c != ' ') {
            printf("! Ошибка: Недопустимый символ '%c'. Название предмета должно содержать только русские буквы и пробелы.\n", c);
            return false; // Найден недопустимый символ
        }
    }
    return true; // Все символы в порядке
}

/**
 * НОРМАЛИЗАЦИЯ (Предмет)
 * Приводит строку к формату "Sentence case": Первая буква заглавная, остальные строчные.
 * Это необходимо для унификации данных, чтобы "математика", "МАТЕМАТИКА" и "Математика" считались одним предметом.
 */
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

