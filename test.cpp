//
// test.cpp

#include <iostream>

#include "eu.hpp"
#include "test.hpp"

using namespace eu;
using namespace std;

int main()
{
    char ch;
    char *str;
    Object a, b, c, d;
    Sequence s, t;
    s.NewStr("Hi");
    b = *(Object*)&s; // convert a "Sequence" to an "Object".
    t = S_repeat(s.obj, (object)2);
    
    if (b.obj == NOVALUE)
    {
        cout << "'b' is an uninitialized Sequence" << endl;
    }
    else
    {
        cout << "'b' is an Sequence" << endl;
    }
    
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
