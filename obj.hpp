// Copyright (c) 2022 James Cook
// obj.hpp
// Classfile for EuObject
//
// 32/64-bit using macro BITS64 for 64-bit

#pragma once

// stdarg.h is included in common.h via a macro, below.
#define USE_STDARG_H
//#include <stdarg.h> /* va_list, va_start, va_arg, va_end */
//#include "pch.h"

#define MIN_SAFE_CHAR 32
#define MAX_SAFE_CHAR 127

#define TYPE_CHECK_INTEGER(val) (((val) > NOVALUE) && ((val) < TOO_BIG_INT))
#define IS_DOUBLE_TO_INT(temp_dbl) (((eu::eudouble)((eu::elong)temp_dbl)) == temp_dbl && temp_dbl <= MAXINT_DBL && temp_dbl >= MININT_DBL)
#define GET_DOUBLE(ob) (eu::eudouble)(IS_ATOM_INT(ob) ? (eu::eudouble)ob : DBL_PTR(ob)->dbl)
#define NEW_DOUBLE(ob) (eu::object)(IS_ATOM_INT(ob) ? eu::NewDouble((eu::eudouble)ob) : eu::RefRet(ob))
#define GET_READONLY_ATOM_VAL(ob) (eu::object)(IS_ATOM_INT(ob) ? ob : eu::DoubleToInt(ob))

// Cast an Object to another very similar type.
// Types need to be the same size.
#define CASTING_PTR(T, ptr_ob) (*((T*)ptr_ob))
#define CASTING_OBJECT(T, ob) (*((T*)&ob))

namespace eu
{
        extern "C" {
                #include "be_eu.h" // See: "obj.hpp"
        #ifdef USE_MATH_H
                #include "be_math.h" // See: "eumath.hpp"
        #endif
        #ifdef USE_STANDARD_LIBRARY
                #include "be_funcs.h"
        #endif
        }

// typedef int integer;
// typedef struct d atom;
// typedef struct s1 sequence;

        elong is_seq_string(object ob, elong minChar = MIN_SAFE_CHAR, elong maxChar = MAX_SAFE_CHAR)
        {
                object_ptr elem;
                elong len;
                ob = (object)SEQ_PTR(ob);
                elem = ((s1_ptr)ob)->base;
                len = ((s1_ptr)ob)->length;
                while (len-- > 0)
                {
                        ob = *(++elem);
                        if (IS_DBL_OR_SEQUENCE(ob) && IS_ATOM_DBL(ob))
                        {
                                ob = DoubleToInt(ob);
                        }
                        if (!IS_ATOM_INT(ob) || (ob > maxChar) || (ob < minChar))
                        {
                                return FALSE;
                        }
                }
                return TRUE;
        }

        // Assumes ob is an atom, and not an encoded pointer to s1.
        object RefRet(object ret)
        {
                RefDS(ret);
                return ret;
        }

        // TODO: Go through Euphoria's documentation and impliment every routine (function and procedure)
        int Version(void) { return 0; } // Version is still in Alpha.
        void Abort(elong error_level) { UserCleanup(error_level); }
#ifdef BITS64
        void print(object obj, long long int stringflag = 0, long long int debugflag = 0, const char atomformat[] = "%.18Lg", long long int forceint = 0) // could be 18 or 21
#else
        void print(object obj, int stringflag = 0, int debugflag = 0, const char atomformat[] = "%.15g", int forceint = 0) // could be 15 or 17
#endif
        {
                if (IS_ATOM_INT(obj) or IS_ATOM_DBL(obj))
                {
                        //object d = DoubleToInt(obj);
                        eudouble a = GET_DOUBLE(obj);
                        if (forceint) {
                                printf(atomformat, (elong)a);
                        }
                        else {
                                printf(atomformat, a);
                        }
                        if (((eudouble)((elong)a)) == a)
                        {
                                if (debugflag >= 2 && a <= MAX_BITWISE_DBL && a >= MIN_BITWISE_DBL)
                                {
                        #ifdef BITS64
                                        printf("(0x%llx)", (eulong)a);
                        #else
                                        printf("(0x%x)", (eulong)a);
                        #endif
                                }
                                if (stringflag >= 1)
                                {
                                        char ch = doChar(obj);
                                        if ((ch <= MAX_SAFE_CHAR) && (ch >= MIN_SAFE_CHAR))
                                        {
                                                printf("'%c'", ch);
                                        }
                                }
                        }
                }
                else if (IS_SEQUENCE(obj))
                {
                        size_t len = (size_t)SEQ_PTR(obj)->length;
                        if ((stringflag >= 2) && is_seq_string(obj, MIN_SAFE_CHAR, MAX_SAFE_CHAR))
                        {
                                char * str = (char *)malloc(len + 1);
                                MakeCString(str, obj);
                                printf("\"%s\"", str);
                                free(str);
                        }
                        else
                        {
                                object_ptr ob = SEQ_PTR(obj)->base;
                                printf("{");
                                if (len > 0) {
                                        if (debugflag >= 1) {
                                                printf("[1]");
                                        }
                                        print(*(++ob), stringflag, debugflag, atomformat, forceint);
                                        for (elong i = 2; i <= len; i++) {
                                                printf(", ");
                                                if (debugflag >= 1) {
                        #ifdef BITS64
                                                        printf("[%lli]", i);
                        #else
                                                        printf("[%i]", i);
                        #endif
                                                }
                                                print(*(++ob), stringflag, debugflag, atomformat, forceint);
                                        }
                                }
                                printf("}");
                        }
                }
                else
                {
                        printf("[NOVALUE]");
                }
        }
#ifdef BITS64
        void println(object obj, long long int stringflag = 0, long long int debugflag = 0, const char atomformat[] = "%.18Lg", long long int forceint = 0)
#else
        void println(object obj, int stringflag = 0, int debugflag = 0, const char atomformat[] = "%.15g", int forceint = 0)
#endif
        {
                print(obj, stringflag, debugflag, atomformat, forceint);
                printf("\n");
        }
        //here.

        class base_class // used to be inherited by other classes.
        {
        friend class Object;
        friend class Integer;
        friend class Atom;
        friend class Sequence;
        friend class Dbl;
        private: // used by friend functions/classes, not inherited.
                elong const type() { return 0; }
        protected: // protected in order to be inherited by other classes.
                object obj;
                //object GetObject() { return obj; }
                void RefObj()
                {
                        if (IS_DBL_OR_SEQUENCE(obj))
                        {
                #ifndef DONE_DEBUGGING
                                d_ptr a = DBL_PTR(obj);
                                s1_ptr s = SEQ_PTR(obj);
                        #ifdef BITS64
                                printf("Before RefObj(0x%llx:0x%llx):\n", (unsigned long long)this, (unsigned long long)a);
                        #else
                                printf("Before RefObj(0x%x:0x%x):\n", (unsigned long)this, (unsigned long)a);
                        #endif
                                ShowDebug();
                #endif
                                RefDS(obj);
                #ifndef DONE_DEBUGGING
                        #ifdef BITS64
                                printf("After RefObj(0x%llx:0x%llx): ref=%lld\n", (unsigned long)this, (unsigned long)a, a->ref);
                        #else
                                printf("After RefObj(0x%x:0x%x): ref=%d\n", (unsigned long)this, (unsigned long)a, a->ref);
                        #endif
                #endif
                        }
                }
                void DeRefObj()
                {
                        if (IS_DBL_OR_SEQUENCE(obj))
                        {
                #ifndef DONE_DEBUGGING
                                d_ptr a = DBL_PTR(obj);
                                s1_ptr s = SEQ_PTR(obj);
                        #ifdef BITS64
                                printf("Before DeRefObj(0x%llx:0x%llx):\n", (unsigned long long)this, (unsigned long long)a);
                        #else
                                printf("Before DeRefObj(0x%x:0x%x):\n", (unsigned long)this, (unsigned long)a);
                        #endif
                                ShowDebug();
                #endif
                                DeRefDS(obj)
                #ifndef DONE_DEBUGGING
                        #ifdef BITS64
                                printf("After DeRefObj(0x%llx:0x%llx): ref=%lld\n", (unsigned long)this, (unsigned long)a, a->ref);
                        #else
                                printf("After DeRefObj(0x%x:0x%x): ref=%d\n", (unsigned long)this, (unsigned long)a, a->ref);
                        #endif
                #endif
                        }
                }
#ifndef DONE_DEBUGGING
        public:
                void ShowDebug() {
                        elong etype = IS_DBL_OR_SEQUENCE(obj) + IS_SEQUENCE(obj);
                        elong ref = 0;
                        elong len = 0;
                        s1_ptr ptr = NULL;
                        if (etype > 0)
                        {
                                ptr = SEQ_PTR(obj);
                                ref = ptr->ref;
                                if (etype > 1)
                                {
                                        etype += is_seq_string(obj, MININT_VAL, MAXINT_VAL) + is_seq_string(obj, 1, 127);
                                        len = ptr->length;
                                }
                        }
        #ifdef BITS64
                        printf("DEBUG: 0x%llx:0x%llx etype=%lld, ref=%lld, length=%lld, value=", (unsigned long long)this, (unsigned long long)ptr, etype, ref, len);
        #else
                        printf("DEBUG: 0x%x:0x%x etype=%d, ref=%d, length=%d, value=", (unsigned long)this, (unsigned long)ptr, etype, ref, len);
        #endif // BITS64
                        println(1, 2);
                }
#else
        public:
#endif // DONE_DEBUGGING
                base_class() { obj = NOVALUE; } // default constructor
                ~base_class() { DeRefObj(); obj = NOVALUE; } // default destructor
                base_class(const base_class& x) { obj = x.obj; RefObj(); } // copy constructor
                base_class& operator= (const base_class& x) { DeRefObj(); obj = x.obj; RefObj(); return *this; } // copy assignment
        #ifdef __GNUC__
                base_class (base_class&& x) { obj = x.obj; x.obj = NOVALUE; } // move constructor
                base_class& operator= (base_class&& x) { DeRefObj(); obj = x.obj; x.obj = NOVALUE; return *this; } // move assignment
        #endif
                //base_class(const object& x) { obj = x; RefObj(); }
                //base_class(object& x) { obj = x; RefObj(); }
                //base_class(const object x) { obj = x; RefObj(); }
                base_class(object x) { obj = x; RefObj(); } // RefObj() here might be a memory leak.
                //object Copy() { RefObj(); return obj; }
                //void Delete() { DeRefObj(); }
                elong get_etype() { if (obj == NOVALUE) return 0; if (IS_ATOM_INT(obj)) return E_INTEGER; if (IS_ATOM_DBL(obj)) return E_ATOM; if (IS_SEQUENCE(obj)) return E_SEQUENCE; return E_OBJECT; }
                bool is_initialized() { return obj != NOVALUE; }
                object GetValue() { RefObj(); return obj; }
                base_class& operator= (const object& x)
                {
                        DeRefObj();
                        obj = x;
                        RefObj(); // Possibly not a memory leak.
                        return *this;
                }
#ifdef BITS64
                void print(long long int stringflag = 0, long long int debugflag = 0, const char atomformat[] = "%.18Lg", long long int forceint = 0)
#else
                void print(int stringflag = 0, int debugflag = 0, const char atomformat[] = "%.15g", int forceint = 0)
#endif
                {
                        eu::print(obj, stringflag, debugflag, atomformat, forceint);
                }
#ifdef BITS64
                void println(long long int stringflag = 0, long long int debugflag = 0, const char atomformat[] = "%.18Lg", long long int forceint = 0)
#else
                void println(int stringflag = 0, int debugflag = 0, const char atomformat[] = "%.15g", int forceint = 0)
#endif
                {
                        eu::println(obj, stringflag, debugflag, atomformat, forceint);
                }


                // friend object seq(elong n, ... );
        };
#ifdef USE_STDARG_H
        object seq(elong n, ... ) {
                // Make a sequence with 'n' elements, each element must be an object.
                va_list vl; // vl is "vee" (v) followed by an "el" (l)
                va_start(vl, n);
        // If C++11 then:
                if (n == NOVALUE)
                {
                        int count = -1;
                        object val;
                        va_list vl_count;
                        /* count number of arguments: */
                        va_copy(vl_count, vl);
                        do {
                                count++;
                                val = va_arg(vl_count, object);
                        } while (val != NOVALUE);
                        va_end(vl_count);
                        n = count;
                }
        // EndIf C++11.
                if (IS_ATOM_INT(n))
                {
                        object ob;
                        object_ptr obj_ptr;
                        s1_ptr ptr;
                        ptr = NewS1(n); // ref==1
                        obj_ptr = ptr->base;
                #ifdef DONE_DEBUGGING
                        while (n-- > 0)
                #else
                        for (elong i = 1; i <= n; i++) // Euphoric 'i' starts at one (1).
                #endif
                        {
                                ob = va_arg(vl, object);
                                if (!(IS_ATOM_INT(ob) || IS_DBL_OR_SEQUENCE(ob)))
                                {
                                        RTFatal("Expected a number, then that number of objects, as parameters to 'seq()'");
                                        return NOVALUE;
                                }
                                // (*(base_class*)&ob).RefObj(); // memory leak.
                                *(++obj_ptr) = ob;
                        }
                        va_end(vl);
                        ob = MAKE_SEQ(ptr);
                        // (*(base_class*)&ob).RefObj(); // memory leak.
                        return ob; // return value.
                }
                RTFatal("Expected a number or NOVALUE as the first parameter of 'seq()'");
                return NOVALUE;
        }
#endif
        class Integer : public base_class
        {
        friend class Atom;
        friend class Object;
        friend class Sequence;
        private:
                elong const type() { return E_INTEGER; }
        public:
                //Integer(object ob) { obj = ob; }
                Integer() { obj = NOVALUE; } // default constructor
                ~Integer() { DeRefObj(); obj = NOVALUE; } // default destructor
                Integer(const Integer& x) { obj = x.obj; RefObj(); } // copy constructor
                //Integer& operator= (const Integer& x) { DeRefObj(); obj = x.obj; RefObj(); return *this; } // copy assignment
                //Integer (Integer&& x) { obj = x.obj; x.obj = NOVALUE; } // move constructor
                //Integer& operator= (Integer&& x) { DeRefObj(); obj = x.obj; x.obj = NOVALUE; return *this; } // move assignment

                Integer(integer val) { obj = TYPE_CHECK_INTEGER(val) ? val : NOVALUE; }
                void NewInteger(elong val) { DeRefObj(); obj = TYPE_CHECK_INTEGER(val) ? val : NOVALUE; }
                elong GetInteger(void) { return TYPE_CHECK_INTEGER(obj) ? obj : NOVALUE; }

#ifdef USE_STANDARD_LIBRARY
                friend Integer IntegerRandom32(Integer a);
#endif

        };

        class Atom : public base_class
        {
        friend class Object;
        private:
                elong const type() { return E_ATOM; }
        public:
                Atom(object ob) { obj = ob; }
                Atom() { obj = NOVALUE; } // default constructor
                ~Atom() { DeRefObj(); obj = NOVALUE; } // default destructor
                Atom(const Atom& x) { obj = x.obj; RefObj(); } // copy constructor
                //Atom& operator= (const Atom& x) { DeRefObj(); obj = x.obj; RefObj(); return *this; } // copy assignment
                //Atom (Atom&& x) { obj = x.obj; x.obj = NOVALUE; } // move constructor
                //Atom& operator= (Atom&& x) { DeRefObj(); obj = x.obj; x.obj = NOVALUE; return *this; } // move assignment

                //Atom(d_ptr ptr) { ++(ptr->ref); obj = MAKE_DBL(ptr); }
                Atom(eudouble d) { obj = IS_DOUBLE_TO_INT(d) ? (object)d : NewDouble(d); }
                //Atom(integer val) { obj = TYPE_CHECK_INTEGER(val) ? val : NewDouble((eudouble)val); }

                void NewAtom(eudouble d) { DeRefObj(); obj = IS_DOUBLE_TO_INT(d) ? (object)d : NewDouble(d); }
                void NewAtom(integer i) { DeRefObj(); obj = TYPE_CHECK_INTEGER(i) ? i : NewDouble((eudouble)i); }
                void NewAtom(eulong u) { DeRefObj(); obj = TYPE_CHECK_INTEGER(u) ? u : NewDouble((eudouble)u); }
                eudouble GetAtomDbl(void) { if(IS_ATOM_INT(obj)) { return (eudouble)obj; } else if(IS_ATOM_DBL(obj)) { return DBL_PTR(obj)->dbl; } else { RTFatal("Expected an Atom, but found a Sequence, in 'GetAtomDbl()'"); return (eudouble)0; } }
                elong GetAtomInt(void) { if (IS_ATOM_INT(obj)) { return obj; } else if (IS_ATOM_DBL(obj)) { return (elong)(DBL_PTR(obj)->dbl); } RTFatal("Expected an Atom, but found a Sequence, in 'GetAtomInt()'"); return 0; }
                eulong GetAtomUnsignedInt(void) { if (IS_ATOM_INT(obj)) { return (eulong)obj; } else if (IS_ATOM_DBL(obj)) { return (eulong)(DBL_PTR(obj)->dbl); } RTFatal("Expected an Atom, but found a Sequence, in 'GetAtomUnsignedInt()'"); return 0; }

                // TODO: Try to convert Atoms to Integers first, then call the appropriate functions.
                elong TryDoubleToInt(void) { if (IS_DBL_OR_SEQUENCE(obj) && IS_ATOM_DBL(obj)) { object ob = DoubleToInt(obj); if (ob != obj) { DeRefDS(obj) obj = ob; return 1; } return 2; } return 0; }

                Atom operator + (const Atom& param) { Atom ret; if(IS_ATOM_INT(obj) && IS_ATOM_INT(param.obj)) { ret.obj = add(obj, param.obj); return ret; } ret.obj = (object)NewDouble(GET_DOUBLE(obj) + GET_DOUBLE(param.obj)); return ret; }
                Atom operator - (const Atom& param) { Atom ret; if(IS_ATOM_INT(obj) && IS_ATOM_INT(param.obj)) { ret.obj = minus(obj, param.obj); return ret; } ret.obj = (object)NewDouble(GET_DOUBLE(obj) - GET_DOUBLE(param.obj)); return ret; }
                Atom operator * (const Atom& param) { Atom ret; if(IS_ATOM_INT(obj) && IS_ATOM_INT(param.obj)) { ret.obj = multiply(obj, param.obj); return ret; } ret.obj = (object)NewDouble(GET_DOUBLE(obj) * GET_DOUBLE(param.obj)); return ret; }
                Atom operator / (const Atom& param) { Atom ret; if(IS_ATOM_INT(obj) && IS_ATOM_INT(param.obj)) { ret.obj = divide(obj, param.obj); return ret; } else { Atom a(NEW_DOUBLE(obj)), b(NEW_DOUBLE(param.obj)); ret.obj = Ddivide(DBL_PTR(a.obj), DBL_PTR(b.obj)); return ret; } }

                // Boolean atom functions: (To be made into operator's)
                object A_equals(Atom b) { if(IS_ATOM_INT(obj) && IS_ATOM_INT(b.obj)) { return equals(obj, b.obj); } else if (not (IS_SEQUENCE(obj) or IS_SEQUENCE(b.obj))) { Atom s(NEW_DOUBLE(obj)), t(NEW_DOUBLE(b.obj)); return Dequals(DBL_PTR(s.obj), DBL_PTR(t.obj)); } RTFatal("Expected Atoms, but found a Sequence, in 'A_equals()'"); return NOVALUE; }
                object A_lessThan(Atom b) { if(IS_ATOM_INT(obj) && IS_ATOM_INT(b.obj)) { return less(obj, b.obj); } else if (not (IS_SEQUENCE(obj) or IS_SEQUENCE(b.obj))) { Atom s(NEW_DOUBLE(obj)), t(NEW_DOUBLE(b.obj)); return Dless(DBL_PTR(s.obj), DBL_PTR(t.obj)); } RTFatal("Expected Atoms, but found a Sequence, in 'A_lessThan()'"); return NOVALUE; }
                object A_greaterThan(Atom b) { if(IS_ATOM_INT(obj) && IS_ATOM_INT(b.obj)) { return greater(obj, b.obj); } else if (not (IS_SEQUENCE(obj) or IS_SEQUENCE(b.obj))) { Atom s(NEW_DOUBLE(obj)), t(NEW_DOUBLE(b.obj)); return Dgreater(DBL_PTR(s.obj), DBL_PTR(t.obj)); } RTFatal("Expected Atoms, but found a Sequence, in 'A_greaterThan()'"); return NOVALUE; }
                object A_notEqual(Atom b) { if(IS_ATOM_INT(obj) && IS_ATOM_INT(b.obj)) { return noteq(obj, b.obj); } else if (not (IS_SEQUENCE(obj) or IS_SEQUENCE(b.obj))) { Atom s(NEW_DOUBLE(obj)), t(NEW_DOUBLE(b.obj)); return Dnoteq(DBL_PTR(s.obj), DBL_PTR(t.obj)); } RTFatal("Expected Atoms, but found a Sequence, in 'A_notEqual()'"); return NOVALUE; }
                object A_lessThanOrEqual(Atom b) { if(IS_ATOM_INT(obj) && IS_ATOM_INT(b.obj)) { return lesseq(obj, b.obj); } else if (not (IS_SEQUENCE(obj) or IS_SEQUENCE(b.obj))) { Atom s(NEW_DOUBLE(obj)), t(NEW_DOUBLE(b.obj)); return Dlesseq(DBL_PTR(s.obj), DBL_PTR(t.obj)); } RTFatal("Expected Atoms, but found a Sequence, in 'A_lessThanOrEqual()'"); return NOVALUE; }
                object A_greaterThanOrEqual(Atom b) { if(IS_ATOM_INT(obj) && IS_ATOM_INT(b.obj)) { return greatereq(obj, b.obj); } else if (not (IS_SEQUENCE(obj) or IS_SEQUENCE(b.obj))) { Atom s(NEW_DOUBLE(obj)), t(NEW_DOUBLE(b.obj)); return Dgreatereq(DBL_PTR(s.obj), DBL_PTR(t.obj)); } RTFatal("Expected Atoms, but found a Sequence, in 'A_greaterThanOrEqual()'"); return NOVALUE; }
                object A_logicaland(Atom b) { if(IS_ATOM_INT(obj) && IS_ATOM_INT(b.obj)) { return Band(obj, b.obj); } else if (not (IS_SEQUENCE(obj) or IS_SEQUENCE(b.obj))) { Atom s(NEW_DOUBLE(obj)), t(NEW_DOUBLE(b.obj)); return Dand(DBL_PTR(s.obj), DBL_PTR(t.obj)); } RTFatal("Expected Atoms, but found a Sequence, in 'A_logicaland()'"); return NOVALUE; }
                object A_logicalor(Atom b) { if(IS_ATOM_INT(obj) && IS_ATOM_INT(b.obj)) { return Bor(obj, b.obj); } else if (not (IS_SEQUENCE(obj) or IS_SEQUENCE(b.obj))) { Atom s(NEW_DOUBLE(obj)), t(NEW_DOUBLE(b.obj)); return Dor(DBL_PTR(s.obj), DBL_PTR(t.obj)); } RTFatal("Expected Atoms, but found a Sequence, in 'A_logicalor()'"); return NOVALUE; }
                object A_logicalxor(Atom b) { if(IS_ATOM_INT(obj) && IS_ATOM_INT(b.obj)) { return Bxor(obj, b.obj); } else if (not (IS_SEQUENCE(obj) or IS_SEQUENCE(b.obj))) { Atom s(NEW_DOUBLE(obj)), t(NEW_DOUBLE(b.obj)); return Dxor(DBL_PTR(s.obj), DBL_PTR(t.obj)); } RTFatal("Expected Atoms, but found a Sequence, in 'A_logicalxor()'"); return NOVALUE; }
                object A_logicalnot() { if(IS_ATOM_INT(obj)) { return unot(obj); } else if (IS_ATOM_DBL(obj)) { return Dnot(DBL_PTR(obj)); } RTFatal("Expected an Atom, but found a Sequence, in 'A_logicalnot()'"); return NOVALUE; }
                Atom A_uminus(Atom a) { Atom ret; if(IS_ATOM_INT(a.obj)) { ret.obj = uminus(a.obj); return ret; } else if (IS_ATOM_DBL(a.obj)) { ret.obj = Duminus(DBL_PTR(a.obj)); return ret; } RTFatal("Expected an Atom, but found a Sequence, in 'A_uminus()'"); return ret; }

                friend Atom A_not_bits(Atom a);
                friend Atom A_and_bits(Atom a, Atom b);
                friend Atom A_or_bits(Atom a, Atom b);
                friend Atom A_xor_bits(Atom a, Atom b);

#ifdef USE_STANDARD_LIBRARY
                friend Atom AtomRandom32(Atom a);
#endif

        };
        // 32-bit atom functions:
        Atom A_not_bits(Atom a) { Atom ret; if(IS_ATOM_INT(a.obj)) { ret.obj = not_bits(a.obj); return ret; } else if (IS_ATOM_DBL(a.obj)) { ret.obj = Dnot_bits(DBL_PTR(a.obj)); return ret; } RTFatal("Expected an Atom, but found a Sequence, in 'A_and_bits()'"); return ret; }
        Atom A_and_bits(Atom a, Atom b) { Atom ret; if(IS_ATOM_INT(a.obj) && IS_ATOM_INT(b.obj)) { ret.obj = and_bits(a.obj, b.obj); return ret; } else if (not (IS_SEQUENCE(a.obj) or IS_SEQUENCE(b.obj))) { Atom s(NEW_DOUBLE(a.obj)), t(NEW_DOUBLE(b.obj)); ret.obj = Dand_bits(DBL_PTR(s.obj), DBL_PTR(t.obj)); return ret; } RTFatal("Expected Atoms, but found a Sequence, in 'A_and_bits()'"); return ret; }
        Atom A_or_bits(Atom a, Atom b) { Atom ret; if(IS_ATOM_INT(a.obj) && IS_ATOM_INT(b.obj)) { ret.obj = or_bits(a.obj, b.obj); return ret; } else if (not (IS_SEQUENCE(a.obj) or IS_SEQUENCE(b.obj))) { Atom s(NEW_DOUBLE(a.obj)), t(NEW_DOUBLE(b.obj)); ret.obj = Dor_bits(DBL_PTR(s.obj), DBL_PTR(t.obj)); return ret; } RTFatal("Expected Atoms, but found a Sequence, in 'A_or_bits()'"); return ret; }
        Atom A_xor_bits(Atom a, Atom b) { Atom ret; if(IS_ATOM_INT(a.obj) && IS_ATOM_INT(b.obj)) { ret.obj = xor_bits(a.obj, b.obj); return ret; } else if (not (IS_SEQUENCE(a.obj) or IS_SEQUENCE(b.obj))) { Atom s(NEW_DOUBLE(a.obj)), t(NEW_DOUBLE(b.obj)); ret.obj = Dxor_bits(DBL_PTR(s.obj), DBL_PTR(t.obj)); return ret; } RTFatal("Expected Atoms, but found a Sequence, in 'A_xor_bits()'"); return ret; }

        class Sequence : public base_class
        {
        friend class Object;
        private:
                elong const type() { return E_SEQUENCE; }
        public:
                Sequence(object ob) { obj = ob; }
                Sequence() { obj = NOVALUE; } // default constructor
                ~Sequence() { DeRefObj(); obj = NOVALUE; } // default destructor
                Sequence(const Sequence& x) { obj = x.obj; RefObj(); } // copy constructor
                //Sequence& operator= (const Sequence& x) { DeRefObj(); obj = x.obj; RefObj(); return *this; } // copy assignment
                //Sequence (Sequence&& x) { obj = x.obj; x.obj = NOVALUE; } // move constructor
                //Sequence& operator= (Sequence&& x) { DeRefObj(); obj = x.obj; x.obj = NOVALUE; return *this; } // move assignment
                Sequence(s1_ptr ptr) { ++(ptr->ref); obj = MAKE_SEQ(ptr); }
                //Sequence& operator= (const object& x)
                //{
                //        DeRefObj();
                //        obj = x;
                //        RefObj();
                //        return *this;
                //}
                Sequence(const char * str) { obj = NewString(str); }
                void newString(const char * str) { DeRefObj(); obj = NewString(str); } // newString(), named that way to avoid naming conflicts.
                char * GetCharStr() {
                        if (IS_DBL_OR_SEQUENCE(obj) && IS_SEQUENCE(obj) && is_seq_string(obj, 1, 255)) {
                                elong len = SEQ_PTR(obj)->length;
                                char * str = (char *)malloc(len + 1);
                                MakeCString(str, obj);
                                return str;
                        }
                        RTFatal("Expected a string Sequence, but found an Object, in 'GetCharStr()'");
                        return NULL;
                }
                void ScreenOutput(FILE *f) { char * out_string = GetCharStr(); screen_output(f, out_string); }
                elong length() { if (IS_DBL_OR_SEQUENCE(obj) && IS_SEQUENCE(obj)) { return SEQ_PTR(obj)->length; } else { RTFatal("Expected a Sequence, but found an Atom, in 'length()'"); return -1; } }
                void E_slice(Sequence src, object start, object end) { // make "obj = src[start..end]", assign src[start..end] to this "obj"
                        if (IS_DBL_OR_SEQUENCE(obj) && IS_SEQUENCE(obj) &&
                        IS_DBL_OR_SEQUENCE(src.obj) && IS_SEQUENCE(src.obj)) {
                                rhs_slice_target = &obj; // DeRef()'s "obj" in next statement below.
                                RHS_Slice((s1_ptr)src.obj, start, end); // takes all object's.
                                rhs_slice_target = NULL;
                        }
                        else {
                                RTFatal("Expected target and argument Sequences in 'E_slice()'");
                        }
                }
                object E_slice_func(object start, object end) { // make "ret = obj[start..end]"
                        if (IS_DBL_OR_SEQUENCE(obj) && IS_SEQUENCE(obj)) {
                                object ret = NOVALUE; // needs to be NOVALUE
                                rhs_slice_target = &ret;
                                RHS_Slice((s1_ptr)obj, start, end);
                                rhs_slice_target = NULL;
                                return ret;
                        }
                        else {
                                RTFatal("Expected this class to be a Sequence in 'E_slice_func()'");
                                return NOVALUE;
                        }
                }
                void E_assign_to_slice(object start, object end, object val) { // make "slice[start..end] = val", val should be the same length as "end - start + 1"
                        if (IS_DBL_OR_SEQUENCE(obj) && IS_SEQUENCE(obj)) {
                                assign_slice_seq = (s1_ptr *)&obj;
                                AssignSlice(start, end, (s1_ptr)val);
                                assign_slice_seq = NULL;
                        }
                        else {
                                RTFatal("Expected target and argument Sequences in 'E_assign_to_slice()'");
                        }
                }
                
                object E_at(elong i); // use (1 to length) or (-1 to -length) // make "obj = seq[index]"
                void E_assign_to_at(elong i, object val); // make "seq[index] = obj"

                void repeat(object item, object repcount)
                {
                        obj = Repeat(item, repcount);
                }
                void prepend(object a)
                {
                        // Ref(a) // possible memory leak.
                        Prepend(&obj, obj, a);
                }
                void append(object a)
                {
                        // Ref(a) // possible memory leak.
                        Append(&obj, obj, a);
                }
                object concatN() // join()
                {
                        s1_ptr sp = SEQ_PTR(obj);
                        object ret = NOVALUE;
                        Concat_N(&ret, sp->base + 1, sp->length);
                        return ret;
                }

                friend object S_repeat(object item, object repcount);
                
                friend void S_prepend(object_ptr target_ptr, Sequence& src, object a);
                friend void S_append(object_ptr target_ptr, Sequence& src, object a);
                friend void S_concat(object_ptr target_ptr, object a, object b);
                friend void S_concatN(object_ptr target_ptr, Sequence& sources);
                
                friend object S_prepend_func(Sequence& src, object a); // target = prepend(src, a);
                friend object S_append_func(Sequence& src, object a); // target = append(src, a);
                friend object S_concat_func(object a, object b); // target = a & b; a and b can be atoms or sequences
                friend object S_concatN_func(Sequence& sources); // concatenate all elements of source, store in target.

                friend elong E_find(object a, Sequence b); // pos = find(a, b);
                friend elong E_find_from(object a, Sequence b, object c); // pos = find_from(a, b, start_from);
                friend elong E_match(Sequence a, Sequence b); // pos = match(a, b);
                friend elong E_match_from(Sequence a, Sequence b, object c); // pos = match_from(a, b, start_from);
        };

        elong E_compare(object a, object b) { return compare(a, b); }

        elong E_find(object a, Sequence b) { return find(a, (s1_ptr)b.obj); }
        elong E_match(Sequence a, Sequence b) { return e_match((s1_ptr)a.obj, (s1_ptr)b.obj); }

        elong E_find_from(object a, Sequence b, object c) { return find_from(a, (s1_ptr)b.obj, c); }
        elong E_match_from(Sequence a, Sequence b, object c) { return e_match_from((s1_ptr)a.obj, (s1_ptr)b.obj, c); }

        object Sequence::E_at(elong i) { // use (1 to length) or (-1 to -length)
                object ret;
                if (IS_DBL_OR_SEQUENCE(obj) && IS_SEQUENCE(obj) && TYPE_CHECK_INTEGER(i)) {
                        s1_ptr sp = SEQ_PTR(obj);
                        if (i < 0) {
                                ++i += sp->length;
                        }
                        if ((i >= 1) && (i <= sp->length)) {
                                ret = sp->base[i];
                                Ref(ret); // Not a possible memory leak.
                        }
                        else {
                                RTFatal("Expected a valid index number in 'E_at()'");
                        }
                }
                else {
                        RTFatal("Expected a Sequence and a valid index number in 'E_at()'");
                }
                return ret;
        }
        void Sequence::E_assign_to_at(elong i, object val) { // make "seq[index] = obj"
                if (IS_DBL_OR_SEQUENCE(obj) && IS_SEQUENCE(obj) && TYPE_CHECK_INTEGER(i)) {
                        s1_ptr sp = SEQ_PTR(obj);
                        if (i < 0) {
                                ++i += sp->length;
                        }
                        if ((i >= 1) && (i <= sp->length)) {
                                if (!UNIQUE(sp)) {
                                        sp = (s1_ptr)SequenceCopy(sp);
                                        obj = MAKE_SEQ(sp); // store new value to obj
                                }
                                // Ref(val); // possible memory leak, use "s.GetValue()" on classes to avoid memory leak.
                                DeRef(sp->base[i])
                                sp->base[i] = val;
                        }
                        else {
                                RTFatal("Expected a valid index number in 'E_assign_to_at()'");
                        }
                }
                else {
                        RTFatal("Expected a Sequence and a valid index number in 'E_assign_to_at()'");
                }
        }
        object S_repeat(object item, object repcount)
        {
                object ret;
                ret = Repeat(item, repcount);
                return ret;
        }
        void S_prepend(object_ptr target_ptr, Sequence& src, object a)
        {
                // Ref(a) // possible memory leak.
                Prepend(target_ptr, src.obj, a);
        }
        object S_prepend_func(Sequence& src, object a) // target = prepend(src, a);
        {
                object ret = NOVALUE;
                S_prepend(&ret, src, a);
                return ret;
        }
        void S_append(object_ptr target_ptr, Sequence& src, object a)
        {
                // Ref(a) // possible memory leak.
                Append(target_ptr, src.obj, a);
        }
        object S_append_func(Sequence& src, object a) // target = append(src, a);
        {
                object ret = NOVALUE;
                S_append(&ret, src, a);
                return ret;
        }
        void S_concat(object_ptr target_ptr, object a, object b)
        {
                bool is_seq_a = IS_DBL_OR_SEQUENCE(a) && IS_SEQUENCE(a);
                bool is_seq_b = IS_DBL_OR_SEQUENCE(b) && IS_SEQUENCE(b);
                if (is_seq_a && (!is_seq_b))
                {
                        // Ref(b) // possible memory leak.
                        Append(target_ptr, a, b);
                }
                else if ((!is_seq_a) && is_seq_b)
                {
                        // Ref(a) // possible memory leak.
                        Prepend(target_ptr, b, a);
                }
                else
                {
                        Concat(target_ptr, a, (s1_ptr)b); // deref's target
                        //RefDS(target.obj); // possible memory leak.
                }
        }
        object S_concat_func(object a, object b) // target = a & b; a and b can be atoms or sequences
        {
                object ret = NOVALUE;
                S_concat(&ret, a, b);
                return ret;
        }
        void S_concatN(object_ptr target_ptr, Sequence& sources)
        {
                s1_ptr source_ptr = SEQ_PTR(sources.obj);
                Concat_N(target_ptr, source_ptr->base + 1, source_ptr->length);
        }
        object S_concatN_func(Sequence& sources) // concatenate all elements of source, store in target.
        {
                object ret = NOVALUE;
                S_concatN(&ret, sources);
                return ret;
        }

        class Object : public base_class
        {
        friend class Sequence;
        friend class Integer;
        friend class Atom;
        private:
                elong const type() { return E_OBJECT; }
        public:
                Object(object ob) { obj = ob; }
                Object() { obj = NOVALUE; } // default constructor
                ~Object() { DeRefObj(); obj = NOVALUE; } // default destructor
                Object(const Object& x) { obj = x.obj; RefObj(); } // copy constructor
                //Object& operator= (const Object& x) { DeRefObj(); obj = x.obj; RefObj(); return *this; } // copy assignment
                //Object(Object&& x) { obj = x.obj; x.obj = NOVALUE; } // move constructor
                //Object& operator= (Object&& x) { DeRefObj(); obj = x.obj; x.obj = NOVALUE; return *this; } // move assignment


                char GetChar() { return doChar(obj); } // aborts if type is sequence.

        };

#ifdef USE_MATH_H
        class Dbl : public base_class
        {
        friend class Atom;
        friend class Object;
        private:
                elong const type() { return 1; }
        public:
                Dbl(object ob) { obj = ob; }
                Dbl() { obj = NOVALUE; } // default constructor
                ~Dbl() { DeRefObj(); obj = NOVALUE; } // default destructor
                Dbl(const Dbl& x) { obj = x.obj; RefObj(); } // copy constructor
                //Dbl& operator= (const Dbl& x) { DeRefObj(); obj = x.obj; RefObj(); return *this; } // copy assignment
                //Dbl (Dbl&& x) { obj = x.obj; x.obj = NOVALUE; } // move constructor
                //Dbl& operator= (Dbl&& x) { DeRefObj(); obj = x.obj; x.obj = NOVALUE; return *this; } // move assignment

                //Dbl(d_ptr ptr) { ++(ptr->ref); obj = MAKE_DBL(ptr); }
                Dbl(eudouble d) { obj = NewDouble(d); }
                void NewDbl(eudouble d) { DeRefObj(); obj = NewDouble(d); }
                eudouble GetDbl(void) { if(IS_ATOM_INT(obj)) { return (eudouble)obj; } else if(IS_ATOM_DBL(obj)) { return DBL_PTR(obj)->dbl; } else { RTFatal("Expected a double or integer, in 'GetDbl()'"); return 0.0; } }

                Dbl operator + (const Dbl& param) { Dbl ret; if(IS_ATOM_INT(obj) && IS_ATOM_INT(param.obj)) { ret.obj = add(obj, param.obj); return ret; } else { Dbl a(NEW_DOUBLE(obj)), b(NEW_DOUBLE(param.obj)); ret.obj = Dadd(DBL_PTR(a.obj), DBL_PTR(b.obj)); return ret; } }
                Dbl operator - (const Dbl& param) { Dbl ret; if(IS_ATOM_INT(obj) && IS_ATOM_INT(param.obj)) { ret.obj = minus(obj, param.obj); return ret; } else { Dbl a(NEW_DOUBLE(obj)), b(NEW_DOUBLE(param.obj)); ret.obj = Dminus(DBL_PTR(a.obj), DBL_PTR(b.obj)); return ret; } }
                Dbl operator * (const Dbl& param) { Dbl ret; if(IS_ATOM_INT(obj) && IS_ATOM_INT(param.obj)) { ret.obj = multiply(obj, param.obj); return ret; } else { Dbl a(NEW_DOUBLE(obj)), b(NEW_DOUBLE(param.obj)); ret.obj = Dmultiply(DBL_PTR(a.obj), DBL_PTR(b.obj)); return ret; } }
                Dbl operator / (const Dbl& param) { Dbl ret; if(IS_ATOM_INT(obj) && IS_ATOM_INT(param.obj)) { ret.obj = divide(obj, param.obj); return ret; } else { Dbl a(NEW_DOUBLE(obj)), b(NEW_DOUBLE(param.obj)); ret.obj = Ddivide(DBL_PTR(a.obj), DBL_PTR(b.obj)); return ret; } }

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
        // Regular eudouble functions:
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
#endif // USE_MATH_H
#ifdef USE_STANDARD_LIBRARY

        //object Date(); // Already defined in "be_funcs.h" and "be_funcs.c"

        // Random functions (on Windows, it requires the "EWINDOWS" to be defined, such as "#define EWINDOWS")

        Integer IntegerRandom32(Integer a)
        {
                object ob;
                if (!IS_ATOM_INT(a.obj))
                {
                        RTFatal("Expected an Integer in IntegerRandom()");
                }
                ob = Random32((long)a.obj);
                return CASTING_OBJECT(Integer, ob);
        }

        Atom AtomRandom32(Atom a)
        {
                if (IS_ATOM_INT(a.obj))
                {
                        object ob;
                        ob = Random32((long)a.obj);
                        return CASTING_OBJECT(Atom, ob);
                }
                if (IS_DBL_OR_SEQUENCE(a.obj) && IS_ATOM_DBL(a.obj))
                {
                        object ob;
                        ob = DRandom32(DBL_PTR(a.obj));
                        return CASTING_OBJECT(Atom, ob);
                }
                else
                {
                        RTFatal("Expected an Atom in AtomRandom()");
                        return NOVALUE;
                }
        }

        Sequence GetRand32()
        {
                // always returns a two (2) element sequence
                object ob = get_rand32();
                return CASTING_OBJECT(Sequence, ob);
        }

        void SetRand32(object x)
        {
                x = set_rand32(x); // ret value is x, ignore.
        }
#endif // USE_STANDARD_LIBRARY
}
