//
// test.cpp

#include <iostream>

#include "eu.hpp"
#include "test.hpp"

// using namespace eu;
using namespace std;

#define COUNT(n) n

int main()
{
    char ch;
    char *str;
    eu::Integer i1, i2;
    eu::Atom a1, a2, a3, a4;
    eu::Object a, b, c, d;
    eu::Sequence s1, s2, s3, s4;
    
// 2. Language Definition

    puts("\n-- examples of atoms:");
    i1 = 0;
    i1.println();
    i2 = 1000;
    i2.println();
    a1 = 98.6;
    a1.println();
    a2 = -1e6;
    a2.println(0,0,"%e");
    
    puts("\n-- examples of sequences:");
    s1 = sq(COUNT(8), 2, 3, 5, 7, 11, 13, 17, 19);
    s1.println();
    s2 = sq(COUNT(5), 1, 2, sq(COUNT(3), 3, 3, 3), 4, sq(COUNT(2), 5, sq(COUNT(1), 6)));
    s2.println();
    s3 = sq(COUNT(3), sq(COUNT(2), NewString("jon"), NewString("smith")), 52389, NewDouble(97.25));
    s3.println(2, 1);
    s4 = sq(COUNT(0));
    s4.println();
    
    puts("\n-- Numbers can also be entered in hexadecimal. For example:");
    a1 = 0xFE;
    a1.println(0,0,"#%X",1);
    a2 = 0xA000;
    a2.println(0,0,"#%X",1);
    a3 = (double)68718428168;
    a3.println(0,0,"%f");
    a4 = -0x10; // -16
    a4.println();

//cin >> ch;
//return 0;
    
    puts("\n-- More examples:");
    
    s1.NewStr("Hi");
    b = s1.GetObject(); // convert a "Sequence" to an "Object".
    s2 = eu::S_repeat(b, (object)2);
    
    str = s1.GetCharStr();
    cout << str << endl;
    free(str);
    s1.print(2);
    printf("\n");
    b.print(1);
    printf("\n");
    s2.print(2);
    printf("\n");
    cout << "Enter a character to exit." << endl;
    cin >> ch;

    return 0;
}
