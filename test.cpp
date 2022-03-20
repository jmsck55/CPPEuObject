//
// test.cpp

#include <iostream>

#include "eu.hpp"
#include "test.hpp"

// using namespace eu;
using namespace std;

int main()
{
    char ch;
    char *str;
    eu::Object a, b, c, d;
    eu::Sequence s, t;
    s.NewStr("Hi");
    b = *(eu::Object*)&s; // convert a "Sequence" to an "Object".
    t = a.S_repeat(b, (object)2);
    
    

    str = s.GetCharStr();
    cout << str << endl;
    free(str);
    s.Print(1);
    printf("\n");
    b.Print(1);
    printf("\n");
    t.Print(1);
    printf("\n");
    cout << "Enter a character to exit." << endl;
    cin >> ch;

    return 0;
}
