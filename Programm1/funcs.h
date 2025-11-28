//
// Created by Dmitriy K on 21.10.2025.
//

#ifndef UNTITLED3_FUNCS_H
#define UNTITLED3_FUNCS_H

#include <string>

// Структура для записи полей преподавателя
struct Teacher {
    std::string surname;
    std::string post;
    std::string subject;
};

// Структура для записи полей экзаменов
struct Exam {
    std::string subject;
    std::string date;
    std::string group;
};

// === ПРОТОТИПЫ ФУНКЦИЙ ===
void Main_Menu1();
void Append_Teachers();
void Append_Exams();
void Rewrite_Teachers();
void Rewrite_Exams();
void Pause_And_Return();
void ClearScreen();

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


// === Безопасный ввод символа в меню ===
int safe_input_int();

#endif //UNTITLED3_FUNCS_H