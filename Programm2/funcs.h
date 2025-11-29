//
// Created by Dmitriy K on 07.11.2025.
//

#ifndef PROGRAMM2_FUNCS_H
#define PROGRAMM2_FUNCS_H

#include <string>
#include <vector>
#include <map>

struct Teacher {
    std::string surname;
    std::string post;
    std::string subject;
};

struct Exam {
    std::string subject;
    std::string date;
    std::string group;
};

// === ПРОТОТИПЫ ФУНКЦИЙ ===
void Main_Menu2();
bool Check_File_Availability();
void File_Report();
void Pause_And_Return();
void Build_Exam_Index(std::map<std::string, std::vector<long>>& index);
void Clean_Subject_Key(char* key);
void Display_Subject_Data();
void ClearScreen();

// === Проверка и нормализация ввода ===
bool isValidSubject(const char* str);
void normalizeSentenceCase(char* str);

// === Безопасный ввод символа в меню ===
int safe_input_int();

#endif //PROGRAMM2_FUNCS_H
