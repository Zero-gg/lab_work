//
// Created by Dmitriy K on 07.11.2025.
//

#ifndef PROGRAMM2_FUNCS_H
#define PROGRAMM2_FUNCS_H

#include <string>
#include <vector>
#include <map>

// Структура для хранения данных о преподавателе
struct Teacher {
    std::string surname; // Фамилия преподавателя
    std::string post;    // Должность
    std::string subject; // Название предмета
};

// Структура для хранения данных об экзамене
struct Exam {
    std::string subject; // Название предмета
    std::string date;    // Дата проведения экзамена
    std::string group;   // Номер группы
};

// === Основные функции программы ===
void Main_Menu2();
bool Check_File_Availability();
void File_Report();
void Pause_And_Return();
void Build_Exam_Index(std::map<std::string, std::vector<long>>& index);
void Clean_Subject_Key(char* key);
void Display_Subject_Data();

// === Функции валидации и нормализации ===
bool isValidSubject(const char* str);
void normalizeSentenceCase(char* str);

// === Вспомогательные функции ввода ===
int safe_input_int(int min_val, int max_val);

#endif //PROGRAMM2_FUNCS_H
