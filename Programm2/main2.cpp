#include <iostream>
#include <locale>
#include <cstdio>
#include <string>
#include <cstring>
#include <vector>
#include <map>

#include "funcs2.h"

using namespace std;

int main() {
    // Перевод локализации на русский язык
    setlocale(LC_ALL, "Russian");
    // Установка кодировки Windows-1251 для консоли (для корректного отображения кириллицы)
    system("chcp 1251 > nul");
    // Очистка экрана от лишней информации
    system("cls");

    Main_Menu2();

    return 0;
}