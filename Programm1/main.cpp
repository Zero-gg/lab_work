#include <iostream>
#include <locale>
#include <cstdio>
#include <cstring>
#include <cctype>
#include "funcs.h"

using namespace std;

int main()
{
    //Перевод локализации на русский язык
    setlocale(LC_ALL, "Russian");
    system("chcp 1251 > nul");
    //Очистка экрана от лишней информации
    system("cls");
    Main_Menu1();

    return 0;
}
