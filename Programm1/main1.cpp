#include <locale>
#include <cstdio>
#include <cstring>
#include <cctype>
#include <cstdlib>
#include "funcs1.h"

int main()
{
    // Перевод локализации на русский язык
    setlocale(LC_ALL, "Russian");
    // Установка кодировки Windows-1251 для консоли (для корректного отображения кириллицы)
    system("chcp 1251 > nul");
    // Очистка экрана от лишней информации
    system("cls");
    Main_Menu1();

    return 0;
}