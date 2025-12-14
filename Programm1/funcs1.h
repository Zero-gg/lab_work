//
// Created by Dmitriy K on 21.10.2025.
//

#ifndef PROGRAMM1_FUNCS_H
#define PROGRAMM1_FUNCS_H

#include <string>

// Константа для размера буфера ввода
const int MAX_STRING_SIZE = 256;

// Структура для записи полей преподавателя
struct Teacher {
    std::string surname; // Фамилия преподавателя
    std::string post;    // Должность
    std::string subject; // Название предмета
};

// Структура для записи полей экзаменов
struct Exam {
    std::string subject; // Название предмета
    std::string date;    // Дата проведения экзамена
    std::string group;   // Номер группы
};

// === ПРОТОТИПЫ ФУНКЦИЙ ===
void Main_Menu1();
void Append_Teachers();
void Append_Exams();
void Rewrite_Teachers();
void Rewrite_Exams();
void Pause_And_Return();

// === Функции-валидаторы ===
bool isValidSurname(const char* str);
bool isValidPost(const char* str);
bool isValidSubject(const char* str);
bool isValidDate(const char* str);
bool isValidGroupNumber(const char* str);

// === Функции для нормализации введённых пользователем данных ===
void normalizeSurname(char* str);
void normalizeSentenceCase(char* str);
void normalizeGroupNumber(char* str);


// === Безопасный ввод целого числа (защита от букв) ===
int safe_input_int(int min_val, int max_val);

// === Безопасный ввод строки (защита от переполнения буфера) ===
// Возвращает false, если ввод был прерван (EOF)
bool safe_input_string(char* buffer, int size);

// === Удаление пробелов в начале и конце строки ===
void trim(char* str);

// === Функции ввода данных (возвращают false, если введено STOP) ===
bool InputTeacher(Teacher* t);
bool InputExam(Exam* e);

#endif //PROGRAMM1_FUNCS_H