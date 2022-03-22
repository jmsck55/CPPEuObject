//
// test.cpp

#include <iostream>

#include "eu.hpp"
#include "test.hpp"

// using namespace eu;
using namespace std;

#define COUNT(n) n

#ifdef DONE_DEBUGGING
#define SHOW_DEBUG(x)
#else
void myShowDebug(eu::base_class* x)
{
    x->ShowDebug();
    puts("[break-point]\n");
}
#define SHOW_DEBUG(x) myShowDebug((eu::base_class*)&x)
#endif

int main()
{
    double dbl;
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
    SHOW_DEBUG(i1);
    i2 = 1000;
    i2.println();
    SHOW_DEBUG(i2);
    a1 = 98.6;
    a1.println();
    SHOW_DEBUG(a1);
    a2 = -1e6;
    a2.println(0,0,"%e");
    SHOW_DEBUG(a2);
    
    puts("\n-- examples of sequences:");
    s1 = eu::seq(COUNT(8), 2, 3, 5, 7, 11, 13, 17, 19);
    s1.println();
    SHOW_DEBUG(s1);
    s2 = eu::seq(COUNT(5), 1, 2, eu::seq(COUNT(3), 3, 3, 3), 4, eu::seq(COUNT(2), 5, eu::seq(COUNT(1), 6)));
    s2.println();
    SHOW_DEBUG(s2);
    s3 = eu::seq(COUNT(3), eu::seq(COUNT(2), NewString("jon"), NewString("smith")), 52389, NewDouble(97.25));
    s3.println(2, 1);
    SHOW_DEBUG(s3);
    s4 = eu::seq(COUNT(0));
    s4.println();
    SHOW_DEBUG(s4);
    
    puts("\n-- Numbers can also be entered in hexadecimal. For example:");
    a1 = 0xFE;
    a1.println(0,0,"#%X",1);
    SHOW_DEBUG(a1);
    a2 = 0xA000;
    a2.println(0,0,"#%X",1);
    SHOW_DEBUG(a2);
    a3 = (double)68718428168;
    a3.println(0,0,"%f");
    SHOW_DEBUG(a3);
    a4 = -0x10; // -16
    a4.println();
    SHOW_DEBUG(a4);

    puts("\n-- More examples:");
    dbl = ((double)1/(double)7);
    a1 = dbl;
    SHOW_DEBUG(a1);
    a2 = a1;
    SHOW_DEBUG(a2);
    
    //s1.NewStr("Hi");
    s1 = "Hi";
    SHOW_DEBUG(s1);
    s2 = s1; // convert a "Sequence" to an "Object".
    SHOW_DEBUG(s2);
    s2 = eu::S_repeat(*(eu::Object*)&s1, (object)2);
    SHOW_DEBUG(s2);
    SHOW_DEBUG(s1);
    if (true)
    {
        eu::Sequence tmp = s2;
        SHOW_DEBUG(s1);
    }
    SHOW_DEBUG(s1);
    
    str = s1.GetCharStr();
    cout << str << endl;
    free(str);
    s1.print(2);
    printf("\n");
    s2.print(1);
    printf("\n");
    s2.print(2);
    printf("\n");
    cout << "Enter a character to exit." << endl;
    cin >> ch;

    return 0;
}
