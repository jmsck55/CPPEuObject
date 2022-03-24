//
// obj.hpp
// Classfile for EuObject

#pragma once

#include <stdarg.h>     /* va_list, va_start, va_arg, va_end */
#include "common.h"

#define MIN_SAFE_CHAR 32
#define MAX_SAFE_CHAR 127

#define CHECK_INTEGER(val) (((val) > NOVALUE) && ((val) < TOO_BIG_INT))
#define IS_DOUBLE_TO_INT(temp_dbl) (((double)((long)temp_dbl)) == temp_dbl && temp_dbl <= MAXINT_DBL && temp_dbl >= MININT_DBL)
#define GET_DOUBLE(ob) (double)(IS_ATOM_INT(ob) ? (double)ob : DBL_PTR(ob)->dbl)
#define NEW_DOUBLE(ob) (object)(IS_ATOM_INT(ob) ? NewDouble((double)ob) : RefRet(ob))
#define GET_RO_ATOM_VAL(ob) (object)(IS_ATOM_INT(ob) ? ob : DoubleToInt(ob))

// Cast an Object to another very similar type.
// Types need to be the same size.
#define CASTING_PTR(T, ptr_ob) (*((T*)ptr_ob))
#define CASTING_OBJECT(T, ob) (*((T*)&ob))

namespace eu
{
        extern "C" {
                #include "be_eu.h" // See: "obj.hpp"
		//#include "be_math.h" // See: "eumath.hpp"
        }
        extern "C" object *rhs_slice_target;
        extern "C" s1_ptr *assign_slice_seq;

// typedef int integer;
// typedef struct d atom;
// typedef struct s1 sequence;

        int IS_SEQ_STRING(object ob, int minChar = MIN_SAFE_CHAR, int maxChar = MAX_SAFE_CHAR)
        {
                object_ptr elem;
                long len;
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
        int Version(void) { return 0; } // Version still in Alpha.
        void Abort(int error_level) { UserCleanup(error_level); }
        
        //here.
        
        class base_class // used to be inherited by other classes.
        {
        friend class Object;
        friend class Integer;
        friend class Atom;
        friend class Sequence;
        friend class Dbl;
        private: // used by friend functions/classes, not inherited.
                long const type() { return NULL; }
        protected: // protected in order to be inherited by other classes.
                object obj;
                object GetObject() { return obj; }
                void RefObj()
                {
                        Ref(obj)
                        if (IS_SEQUENCE(obj))
                        {
                                object_ptr ptr = SEQ_PTR(obj)->base;
                                while (*(++ptr) != NOVALUE)
                                {
                                        (*(base_class*)ptr).RefObj();
                                }
                        }
                }
                void DeRefObj()
                {
                        if (IS_SEQUENCE(obj))
                        {
                                object_ptr ptr = SEQ_PTR(obj)->base;
                                while (*(++ptr) != NOVALUE)
                                {
                                        (*(base_class*)ptr).DeRefObj();
                                }
                        }
                        DeRef(obj)
                }
#ifndef DONE_DEBUGGING
                //long etype;
                //union {
                //      s1_ptr sptr;
                //      d_ptr dptr;
                //};
                //s1_ptr GetSeqPtr() { return SEQ_PTR(obj); }
                //d_ptr GetDblPtr() { return DBL_PTR(obj); }
        public:
                void ShowDebug() {
                        int etype = IS_DBL_OR_SEQUENCE(obj) + IS_SEQUENCE(obj);
                        int count = 0;
                        int len = 0;
                        s1_ptr ptr = NULL;
                        if (etype > 0)
                        {
                                ptr = SEQ_PTR(obj);
                                count = ptr->ref;
                                if (etype > 1)
                                {
                                        etype += IS_SEQ_STRING(obj, MININT_VAL, MAXINT_VAL) + IS_SEQ_STRING(obj, 1, 127);
                                        len = ptr->length;
                                }
                        }
                        printf("DEBUG: 0x%x:0x%x etype=%d, count=%d, length=%d, value=", (unsigned long)this, (unsigned long)ptr, etype, count, len);
                        println(1, 2);
                }
#endif
        public:
                base_class() { obj = NOVALUE; } // default constructor
                ~base_class() { DeRefObj(); obj = NOVALUE; } // default destructor
                base_class(const base_class& x) { obj = x.obj; RefObj(); } // copy constructor
                //base_class(const object& x) { obj = x; RefObj(); }
                //base_class(object& x) { obj = x; RefObj(); }
                //base_class(const object x) { obj = x; RefObj(); }
                base_class(object x) { obj = x; RefObj(); }
                base_class& operator= (const base_class& x)
                {
                        DeRefObj();
                        obj = x.obj;
                        RefObj();
                        return *this;
                }
                base_class& operator= (const object& x)
                {
                        DeRefObj();
                        obj = x;
                        RefObj();
                        return *this;
                }
                void print(int stringflag = 0, int debugflag = 0, const char atomformat[] = "%.17g", int forceint = 0)
                {
                        if (IS_ATOM_INT(obj) or IS_ATOM_DBL(obj))
                        {
                                //object d = DoubleToInt(obj);
                                double a = GET_DOUBLE(obj);
                                if (forceint) {
                                        printf(atomformat, (int)a);
                                }
                                else {
                                        printf(atomformat, a);
                                }
                                if (((double)((long)a)) == a)
                                {
                                        if (debugflag >= 2 && a <= MAX_BITWISE_DBL && a >= MIN_BITWISE_DBL)
                                        {
                                                printf("(0x%x)", (unsigned long)a);
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
                                long len = SEQ_PTR(obj)->length;
                                if ((stringflag >= 2) && IS_SEQ_STRING(obj, MIN_SAFE_CHAR, MAX_SAFE_CHAR))
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
                                                ((base_class*)(++ob))->print(stringflag, debugflag, atomformat, forceint);
                                                for (long i = 2; i <= len; i++) {
                                                        printf(", ");
                                                        if (debugflag >= 1) {
                                                                printf("[%i]", (int)i);
                                                        }
                                                        ((base_class*)(++ob))->print(stringflag, debugflag, atomformat, forceint);
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
                void println(int stringflag = 0, int debugflag = 0, const char atomformat[] = "%.17g", int forceint = 0) {
                        print(stringflag, debugflag, atomformat, forceint);
                        printf("\n");
                }
                friend object seq(unsigned int n, ... );
        };
        object seq(unsigned int n, ... ) {
                // Make a sequence with 'n' elements, each element must be an object.
                object ob;
                object_ptr obj_ptr;
                s1_ptr ptr = NewS1(n);
                obj_ptr = ptr->base;
                va_list v1;
                va_start(v1, n);
                while (n-- > 0) {
                        ob = va_arg(v1, object);
                        (*(base_class*)&ob).RefObj();
                        *(++obj_ptr) = ob;
                }
                va_end(v1);
                ob = MAKE_SEQ(ptr);
                return ob;
        }
        
        class Integer : public base_class
        {
        friend class Atom;
        friend class Object;
        friend class Sequence;
        private:
                long const type() { return E_INTEGER; }
                Integer(object ob) { obj = ob; }
        public:
                Integer() { obj = NOVALUE; } // default constructor
                ~Integer() { DeRefObj(); obj = NOVALUE; } // default destructor
                Integer(const Integer& x) { obj = x.obj; RefObj(); } // copy constructor
                //Integer& operator= (const Integer& x) { DeRefObj(); obj = x.obj; RefObj(); return *this; } // copy assignment
                //Integer (Integer&& x) { obj = x.obj; x.obj = NOVALUE; } // move constructor
                //Integer& operator= (Integer&& x) { DeRefObj(); obj = x.obj; x.obj = NOVALUE; return *this; } // move assignment
                
                Integer(int val) { obj = CHECK_INTEGER(val) ? val : NOVALUE; }
                void NewInteger(int val) { DeRefObj(); obj = CHECK_INTEGER(val) ? val : NOVALUE; }
                int GetInteger(void) { return CHECK_INTEGER(obj) ? obj : NOVALUE; }
                
                friend Integer E_compare(Object a, Object b);
                friend Integer E_find(Object a, Sequence b);
                friend Integer E_find_from(Object a, Sequence b, object c);
                friend Integer E_match(Sequence a, Sequence b);
                friend Integer E_match_from(Sequence a, Sequence b, object c);
        };
        
        class Atom : public base_class
        {
        friend class Object;
        private:
                long const type() { return E_ATOM; }
                Atom(object ob) { obj = ob; }
        public:
                Atom() { obj = NOVALUE; } // default constructor
                ~Atom() { DeRefObj(); obj = NOVALUE; } // default destructor
                Atom(const Atom& x) { obj = x.obj; RefObj(); } // copy constructor
                //Atom& operator= (const Atom& x) { DeRefObj(); obj = x.obj; RefObj(); return *this; } // copy assignment
                //Atom (Atom&& x) { obj = x.obj; x.obj = NOVALUE; } // move constructor
                //Atom& operator= (Atom&& x) { DeRefObj(); obj = x.obj; x.obj = NOVALUE; return *this; } // move assignment
                
                Atom(d_ptr ptr) { ++(ptr->ref); obj = MAKE_DBL(ptr); }
                Atom(double d) { obj = IS_DOUBLE_TO_INT(d) ? (object)d : NewDouble(d); }
                Atom(int val) { obj = CHECK_INTEGER(val) ? val : NewDouble((double)val); }
                
                void NewAtom(double d) { DeRefObj(); obj = IS_DOUBLE_TO_INT(d) ? (object)d : NewDouble(d); }
                void NewAtom(int i32) { DeRefObj(); obj = CHECK_INTEGER(i32) ? i32 : NewDouble((double)i32); }
                void NewAtom(unsigned int u32) { DeRefObj(); obj = CHECK_INTEGER(u32) ? u32 : NewDouble((double)u32); }
                double GetAtomDbl(void) { if(IS_ATOM_INT(obj)) { return (double)obj; } else if(IS_ATOM_DBL(obj)) { return DBL_PTR(obj)->dbl; } else { RTFatal("Expected an Atom, but found a Sequence, in 'GetAtomDbl()'"); return 0.0; } }
                int GetAtomInt(void) { if (IS_ATOM_INT(obj)) { return obj; } else if (IS_ATOM_DBL(obj)) { return (int)(DBL_PTR(obj)->dbl); } RTFatal("Expected an Atom, but found a Sequence, in 'GetAtomInt()'"); return 0; }
                unsigned int GetAtomUnsignedInt(void) { if (IS_ATOM_INT(obj)) { return (unsigned int)obj; } else if (IS_ATOM_DBL(obj)) { return (unsigned int)(DBL_PTR(obj)->dbl); } RTFatal("Expected an Atom, but found a Sequence, in 'GetAtomUnsignedInt()'"); return 0; }
                
                // TODO: Try to convert Atoms to Integers first, then call the appropriate functions.
                int TryDoubleToInt(void) { if (IS_DBL_OR_SEQUENCE(obj) && IS_ATOM_DBL(obj)) { object ob = DoubleToInt(obj); if (ob != obj) { DeRefDS(obj) obj = ob; return E_INTEGER; } return E_ATOM; } return E_OBJECT; }
                
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
                long const type() { return E_SEQUENCE; }
                Sequence(object ob) { obj = ob; }
        public:
                Sequence() { obj = NOVALUE; } // default constructor
                ~Sequence() { DeRefObj(); obj = NOVALUE; } // default destructor
                Sequence(const Sequence& x) { obj = x.obj; RefObj(); } // copy constructor
                //Sequence& operator= (const Sequence& x) { DeRefObj(); obj = x.obj; RefObj(); return *this; } // copy assignment
                //Sequence (Sequence&& x) { obj = x.obj; x.obj = NOVALUE; } // move constructor
                //Sequence& operator= (Sequence&& x) { DeRefObj(); obj = x.obj; x.obj = NOVALUE; return *this; } // move assignment
                //Sequence(s1_ptr ptr) { ++(ptr->ref); obj = MAKE_SEQ(ptr); }
                Sequence(const char * str) { obj = NewString(str); }
                void NewStr(const char * str) { DeRefObj(); obj = NewString(str); }
                char * GetCharStr() {
                        if (IS_DBL_OR_SEQUENCE(obj) && IS_SEQUENCE(obj) && IS_SEQ_STRING(obj, 1, 255)) {
                                long len = SEQ_PTR(obj)->length;
                                char * str = (char *)malloc(len + 1);
                                MakeCString(str, obj);
                                return str;
                        }
                        RTFatal("Expected a string Sequence, but found an Object, in 'GetCharStr()'");
                        return NULL;
                }
                void ScreenOutput(FILE *f) { char * out_string = GetCharStr(); screen_output(f, out_string); }
                long length() { if (IS_DBL_OR_SEQUENCE(obj) && IS_SEQUENCE(obj)) { return SEQ_PTR(obj)->length; } else { RTFatal("Expected a Sequence, but found an Atom, in 'length()'"); return -1; } }
                
                void E_construct_slice(Sequence src, object start, object end) { // make "slice = src[start..end]"
                        if (IS_DBL_OR_SEQUENCE(obj) && IS_SEQUENCE(obj) &&
                        IS_DBL_OR_SEQUENCE(src.obj) && IS_SEQUENCE(src.obj)) {
                                rhs_slice_target = &obj; // DeRef()'s "obj" in next statement below.
                                RHS_Slice((s1_ptr)src.obj, start, end); // takes all object's.
                                rhs_slice_target = NULL;
                        }
                        RTFatal("Expected target and argument Sequences in 'E_construct_slice()'");
                }
                void E_assign_to_slice(object start, object end, Sequence val) { // make "slice[start..end] = val"
                        if (IS_DBL_OR_SEQUENCE(obj) && IS_SEQUENCE(obj) &&
                        IS_DBL_OR_SEQUENCE(val.obj) && IS_SEQUENCE(val.obj)) {
                                assign_slice_seq = (s1_ptr *)&obj;
                                AssignSlice(start, end, (s1_ptr)val.obj);
                                assign_slice_seq = NULL;
                        }
                        RTFatal("Expected target and argument Sequences in 'E_assign_to_slice()'");
                }
                
                Object Sequence::E_at(int i); // use (1 to length) or (-1 to -length) // make "obj = seq[index]"

                friend Sequence S_repeat(Object item, object repcount);
                
                void Sequence::prepend(Object a);
                void Sequence::append(Object a);
                
                friend void S_prepend(Sequence target, Sequence src, Object a);
                friend void S_append(Sequence target, Sequence src, Object a);
                friend void S_concat(Sequence target, Object a, Object b);
                friend void S_concatN(Sequence target, Sequence source);
                
                friend Integer E_find(Object a, Sequence b);
                friend Integer E_find_from(Object a, Sequence b, object c);
                friend Integer E_match(Sequence a, Sequence b);
                friend Integer E_match_from(Sequence a, Sequence b, object c);
        };
        
        void S_concatN(Sequence target, Sequence sources)
        {
                s1_ptr p = SEQ_PTR(sources.obj);
                Concat_N(&(target.obj), p->base, p->length);
        }
        
        class Object : public base_class
        {
        friend class Sequence;
        friend class Integer;
        friend class Atom;
        private:
                long const type() { return E_OBJECT; }
                Object(object ob) { obj = ob; }
        public:
                Object() { obj = NOVALUE; } // default constructor
                ~Object() { DeRefObj(); obj = NOVALUE; } // default destructor
                Object(const Object& x) { obj = x.obj; RefObj(); } // copy constructor
                //Object& operator= (const Object& x) { DeRefObj(); obj = x.obj; RefObj(); return *this; } // copy assignment
                //Object(Object&& x) { obj = x.obj; x.obj = NOVALUE; } // move constructor
                //Object& operator= (Object&& x) { DeRefObj(); obj = x.obj; x.obj = NOVALUE; return *this; } // move assignment
                
                char GetChar() { return doChar(obj); } // aborts if type is sequence.
                
                friend Integer E_compare(Object a, Object b);
                friend Integer E_find(Object a, Sequence b);
                friend Integer E_find_from(Object a, Sequence b, object c);
                
                friend Sequence S_repeat(Object item, object repcount);
                friend void S_prepend(Sequence target, Sequence src, Object a);
                friend void S_append(Sequence target, Sequence src, Object a);
                friend void S_concat(Sequence target, Object a, Object b);
        };
        
        Integer E_compare(Object a, Object b) { Integer ret; ret.obj = compare(a.obj, b.obj); return ret; }
        Integer E_find(Object a, Sequence b) { Integer ret; ret.obj = find(a.obj, (s1_ptr)b.obj); return ret; }
        Integer E_match(Sequence a, Sequence b) { Integer ret; if (SEQ_PTR(a.obj)->length > 0) { ret.obj = e_match((s1_ptr)a.obj, (s1_ptr)b.obj); } else { ret.obj = -1; } return ret; }
        
        Integer E_find_from(Object a, Sequence b, object c) { Integer ret; ret.obj = find_from(a.obj, (s1_ptr)b.obj, c); return ret; }
        Integer E_match_from(Sequence a, Sequence b, object c) { Integer ret; if (SEQ_PTR(a.obj)->length > 0) { ret.obj = e_match_from((s1_ptr)a.obj, (s1_ptr)b.obj, c); } else { ret.obj = -1; } return ret; }
        
        Object Sequence::E_at(int i) { // use (1 to length) or (-1 to -length)
                Object ret;
                if (IS_DBL_OR_SEQUENCE(obj) && IS_SEQUENCE(obj) && CHECK_INTEGER(i)) {
                        s1_ptr ptr = SEQ_PTR(obj);
                        long len = ptr->length;
                        if (i < 0) {
                                i += len;
                                i++;
                        }
                        if ((i >= 1) && (i <= len)) {
                                ret.obj = ptr->base[i];
                                ret.RefObj();
                        }
                        else
                        {
                                RTFatal("Expected a valid index number in 'E_at()'");
                        }
                }
                else
                {
                        RTFatal("Expected a Sequence and a valid index number in 'E_at()'");
                }
                return ret;
        }
        Sequence S_repeat(Object item, object repcount)
        {
                Sequence ret;
                ret.obj = Repeat(item.obj, repcount);
                return ret;
        }
        void Sequence::prepend(Object a)
        {
                Ref(a.obj)
                Prepend(&obj, obj, a.obj);
        }
        void Sequence::append(Object a)
        {
                Ref(a.obj)
                Append(&obj, obj, a.obj);
        }
        void S_prepend(Sequence target, Sequence src, Object a)
        {
                Ref(a.obj)
                Prepend(&(target.obj), src.obj, a.obj);
        }
        void S_append(Sequence target, Sequence src, Object a)
        {
                Ref(a.obj)
                Append(&(target.obj), src.obj, a.obj);
        }
        void S_concat(Sequence target, Object a, Object b)
        {
                bool is_seq_a = IS_DBL_OR_SEQUENCE(a.obj) && IS_SEQUENCE(a.obj);
                bool is_seq_b = IS_DBL_OR_SEQUENCE(b.obj) && IS_SEQUENCE(b.obj);
                if (is_seq_a && (!is_seq_b))
                {
                        Ref(b.obj)
                        Append(&(target.obj), a.obj, b.obj);
                }
                else if ((!is_seq_a) && is_seq_b)
                {
                        Ref(a.obj)
                        Prepend(&(target.obj), b.obj, a.obj);
                }
                else
                {
                        Concat(&(target.obj), a.obj, (s1_ptr)b.obj);
                }
        }
}
