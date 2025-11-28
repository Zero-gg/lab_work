#include <iostream>
#include <locale>
#include <cstdio>
#include <string>
#include <cstring>
#include <vector>
#include <map>

#include "funcs.h"

using namespace std;

int main() {
    setlocale(LC_ALL, "Russian");
    system("chcp 1251 > nul");
    system("cls");

    Main_Menu2();

    return 0;
}
