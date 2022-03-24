//
// eumath.hpp
// Floating-point Math Classfile for EuObject
// "class Dbl" uses integers and doubles.
// It includes "math.h" library routines for double floating-point numbers.

#include "obj.hpp"

namespace eu
{
        extern "C" {
                //#include "be_eu.h" // See: "obj.hpp"
		#include "be_math.h" // See: "eumath.hpp"
        }

        class Dbl : public base_class
        {
        friend class Atom;
        friend class Object;
        private:
                long const type() { return E_DBL; }
                Dbl(object ob) { obj = ob; }
        public:
                Dbl() { obj = NOVALUE; } // default constructor
                ~Dbl() { DeRefObj(); obj = NOVALUE; } // default destructor
                Dbl(const Dbl& x) { obj = x.obj; RefObj(); } // copy constructor
                //Dbl& operator= (const Dbl& x) { DeRefObj(); obj = x.obj; RefObj(); return *this; } // copy assignment
                //Dbl (Dbl&& x) { obj = x.obj; x.obj = NOVALUE; } // move constructor
                //Dbl& operator= (Dbl&& x) { DeRefObj(); obj = x.obj; x.obj = NOVALUE; return *this; } // move assignment
                
                Dbl(d_ptr ptr) { ++(ptr->ref); obj = MAKE_DBL(ptr); }
                Dbl(double d) { obj = NewDouble(d); }
                void NewDbl(double d) { DeRefObj(); obj = NewDouble(d); }
                double GetDbl(void) { if(IS_DBL_OR_SEQUENCE(obj) && IS_ATOM_DBL(obj)) { return DBL_PTR(obj)->dbl; } else { RTFatal("Expected a double, in 'GetDbl()'"); return 0.0; } }
                
                Dbl operator + (const Dbl& param) { Dbl ret; ret.obj = Dadd(DBL_PTR(obj), DBL_PTR(param.obj)); return ret; }
                Dbl operator - (const Dbl& param) { Dbl ret; ret.obj = Dminus(DBL_PTR(obj), DBL_PTR(param.obj)); return ret; }
                Dbl operator * (const Dbl& param) { Dbl ret; ret.obj = Dmultiply(DBL_PTR(obj), DBL_PTR(param.obj)); return ret; }
                Dbl operator / (const Dbl& param) { Dbl ret; ret.obj = Ddivide(DBL_PTR(obj), DBL_PTR(param.obj)); return ret; }
                
                friend Dbl A_remainder(Dbl a, Dbl b);
                friend Dbl A_power(Dbl a, Dbl b);
		friend Dbl A_sqrt(Dbl a);
                friend Dbl A_sin(Dbl a);
                friend Dbl A_cos(Dbl a);
                friend Dbl A_tan(Dbl a);
                friend Dbl A_arctan(Dbl a);
                friend Dbl A_log(Dbl a);
                friend Dbl A_floor(Dbl a);
        };
        // Regular double functions:
        Dbl A_remainder(Dbl a, Dbl b) { Dbl ret; if(IS_ATOM_INT(a.obj) && IS_ATOM_INT(b.obj)) { ret.obj = eremainder(a.obj, b.obj); return ret; } else if (not (IS_SEQUENCE(a.obj) or IS_SEQUENCE(b.obj))) { Dbl s(NEW_DOUBLE(a.obj)), t(NEW_DOUBLE(b.obj)); ret.obj = Dremainder(DBL_PTR(s.obj), DBL_PTR(t.obj)); return ret; } RTFatal("Expected Dbls, but found a Sequence, in 'A_remainder()'"); return ret; }
        Dbl A_power(Dbl a, Dbl b) { Dbl ret; if(IS_ATOM_INT(a.obj) && IS_ATOM_INT(b.obj)) { ret.obj = power(a.obj, b.obj); return ret; } else if (not (IS_SEQUENCE(a.obj) or IS_SEQUENCE(b.obj))) { Dbl s(NEW_DOUBLE(a.obj)), t(NEW_DOUBLE(b.obj)); ret.obj = Dpower(DBL_PTR(s.obj), DBL_PTR(t.obj)); return ret; } RTFatal("Expected Dbls, but found a Sequence, in 'A_power()'"); return ret; }
        // Unary Ops
        Dbl A_sqrt(Dbl a) { Dbl ret; if(IS_ATOM_INT(a.obj)) { ret.obj = e_sqrt(a.obj); return ret; } else if (IS_ATOM_DBL(a.obj)) { ret.obj = De_sqrt(DBL_PTR(a.obj)); return ret; } RTFatal("Expected an Dbl, but found a Sequence, in 'A_sqrt()'"); return ret; }
        Dbl A_sin(Dbl a) { Dbl ret; if(IS_ATOM_INT(a.obj)) { ret.obj = e_sin(a.obj); return ret; } else if (IS_ATOM_DBL(a.obj)) { ret.obj = De_sin(DBL_PTR(a.obj)); return ret; } RTFatal("Expected an Dbl, but found a Sequence, in 'A_sin()'"); return ret; }
        Dbl A_cos(Dbl a) { Dbl ret; if(IS_ATOM_INT(a.obj)) { ret.obj = e_cos(a.obj); return ret; } else if (IS_ATOM_DBL(a.obj)) { ret.obj = De_cos(DBL_PTR(a.obj)); return ret; } RTFatal("Expected an Dbl, but found a Sequence, in 'A_cos()'"); return ret; }
        Dbl A_tan(Dbl a) { Dbl ret; if(IS_ATOM_INT(a.obj)) { ret.obj = e_tan(a.obj); return ret; } else if (IS_ATOM_DBL(a.obj)) { ret.obj = De_tan(DBL_PTR(a.obj)); return ret; } RTFatal("Expected an Dbl, but found a Sequence, in 'A_tan()'"); return ret; }
        Dbl A_arctan(Dbl a) { Dbl ret; if(IS_ATOM_INT(a.obj)) { ret.obj = e_arctan(a.obj); return ret; } else if (IS_ATOM_DBL(a.obj)) { ret.obj = De_arctan(DBL_PTR(a.obj)); return ret; } RTFatal("Expected an Dbl, but found a Sequence, in 'A_arctan()'"); return ret; }
        Dbl A_log(Dbl a) { Dbl ret; if(IS_ATOM_INT(a.obj)) { ret.obj = e_log(a.obj); return ret; } else if (IS_ATOM_DBL(a.obj)) { ret.obj = De_log(DBL_PTR(a.obj)); return ret; } RTFatal("Expected an Dbl, but found a Sequence, in 'A_log()'"); return ret; }
        Dbl A_floor(Dbl a) { Dbl ret; if(IS_ATOM_INT(a.obj)) { ret.obj = e_floor(a.obj); return ret; } else if (IS_ATOM_DBL(a.obj)) { ret.obj = De_floor(DBL_PTR(a.obj)); return ret; } RTFatal("Expected an Dbl, but found a Sequence, in 'A_floor()'"); return ret; }
}
