//
// Obj.hpp
// Classfile for EuObject

#pragma once

extern "C" {
        #include "be_eu.h"
}

extern "C" object *rhs_slice_target;
extern "C" s1_ptr *assign_slice_seq;

typedef int integer;
typedef struct d atom;
typedef struct s1 sequence;

#define MIN_SAFE_CHAR 32
#define MAX_SAFE_CHAR 126

void RefObj(object ob)
{
        Ref(ob)
        if (IS_SEQUENCE(ob))
        {
                object_ptr obj_ptr = SEQ_PTR(ob)->base;
                while (*(++obj_ptr) != NOVALUE)
                {
                        RefObj(*obj_ptr);
                }
        }
}

void DeRefObj(object ob)
{
        if (IS_SEQUENCE(ob))
        {
                object_ptr obj_ptr = SEQ_PTR(ob)->base;
                while (*(++obj_ptr) != NOVALUE)
                {
                        DeRefObj(*obj_ptr);
                }
        }
        DeRef(ob)
}

int IS_SEQ_STRING(object ob)
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
                if (!IS_ATOM_INT(ob) || (ob > MAX_SAFE_CHAR) || (ob < MIN_SAFE_CHAR))
                {
                        return FALSE;
                }
        }
        return TRUE;
}

#define CHECK_INTEGER(val) (IS_ATOM_INT(val) && (val < TOO_BIG_INT))

// Assumes ob is not encoded pointer to s1.
object RefRet(object ret) { RefDS(ret); return ret; }
#define GET_DOUBLE(ob) (double)(IS_ATOM_INT(ob) ? (double)ob : DBL_PTR(ob)->dbl)
#define NEW_DOUBLE(ob) (object)(IS_ATOM_INT(ob) ? NewDouble((double)ob) : RefRet(ob))

// #define MININT_VAL MININT
// #define MININT_DBL ((double)MININT_VAL)
// #define MAXINT_VAL MAXINT
// #define MAXINT_DBL ((double)MAXINT_VAL)

namespace eu
{
        int Version(void) { return 0; } // Version still in Alpha.
        void Abort(int error_level) { UserCleanup(error_level); }
        
        class base_class
        {
        public:
                object obj;
                
                void Print(int stringflag) {
                        if (IS_ATOM_INT(obj))
                        {
                                printf("%i", (int)obj);
                        }
                        else if (IS_ATOM_DBL(obj))
                        {
                                printf("%e", DBL_PTR(obj)->dbl);
                        }
                        else if (IS_SEQUENCE(obj))
                        {
                                long len = SEQ_PTR(obj)->length;
                                if ((stringflag == 1) && IS_SEQ_STRING(obj))
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
                                                printf("[1]");
                                                ((base_class*)(++ob))->Print(stringflag);
                                                for (long i = 2; i <= len; i++) {
                                                        printf(",[%i]", (int)i);
                                                        ((base_class*)(++ob))->Print(stringflag);
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
        };
        
        class Integer : public base_class
        {
        public:
                long type() { return E_INTEGER; }
                // union {
                //         object obj;
                //         //s1_ptr seq;
                //         //d_ptr dbl;
                // };
                Integer() { obj = NOVALUE; } // default constructor
                ~Integer() { DeRefObj(obj); obj = NOVALUE; } // default destructor
                Integer(const Integer& x) { obj = x.obj; RefObj(obj); } // copy constructor
                Integer& operator= (const Integer& x) { DeRefObj(obj); obj = x.obj; RefObj(obj); return *this; } // copy assignment
                //Integer (Integer&& x) { obj = x.obj; x.obj = NOVALUE; } // move constructor
                //Integer& operator= (Integer&& x) { DeRefObj(obj); obj = x.obj; x.obj = NOVALUE; return *this; } // move assignment
                
                Integer(object ob) { obj = ob; }
                Integer(int val) { obj = CHECK_INTEGER(val) ? val : NOVALUE; }
                void NewInteger(int val) { DeRefObj(obj); obj = CHECK_INTEGER(val) ? val : NOVALUE; }
                int GetInteger(void) { return CHECK_INTEGER(obj) ? obj : NOVALUE; }
                
                
        };
        
        
        class Dbl : public base_class
        {
        public:
                long type() { return E_DBL; }
                // union {
                //         object obj;
                //         //s1_ptr s;
                //         //d_ptr p;
                // };
                Dbl() { obj = NOVALUE; } // default constructor
                ~Dbl() { DeRefObj(obj); obj = NOVALUE; } // default destructor
                Dbl(const Dbl& x) { obj = x.obj; RefObj(obj); } // copy constructor
                Dbl& operator= (const Dbl& x) { DeRefObj(obj); obj = x.obj; RefObj(obj); return *this; } // copy assignment
                //Dbl (Dbl&& x) { obj = x.obj; x.obj = NOVALUE; } // move constructor
                //Dbl& operator= (Dbl&& x) { DeRefObj(obj); obj = x.obj; x.obj = NOVALUE; return *this; } // move assignment
                
                Dbl(object ob) { obj = ob; }
                Dbl(d_ptr ptr) { ++(ptr->ref); obj = MAKE_DBL(ptr); }
                Dbl(double d) { obj = NewDouble(d); }
                void NewDbl(double d) { DeRefObj(obj); obj = NewDouble(d); }
                double GetDbl(void) { return IS_ATOM_DBL(obj) ? DBL_PTR(obj)->dbl : 0.0; }
                
                Dbl operator + (const Dbl& param) { Dbl ret; ret.obj = Dadd(DBL_PTR(obj), DBL_PTR(param.obj)); return ret; }
                Dbl operator - (const Dbl& param) { Dbl ret; ret.obj = Dminus(DBL_PTR(obj), DBL_PTR(param.obj)); return ret; }
                Dbl operator * (const Dbl& param) { Dbl ret; ret.obj = Dmultiply(DBL_PTR(obj), DBL_PTR(param.obj)); return ret; }
                Dbl operator / (const Dbl& param) { Dbl ret; ret.obj = Ddivide(DBL_PTR(obj), DBL_PTR(param.obj)); return ret; }
                
                
        };
        
        class Atom : public base_class // public Integer, public Dbl
        {
        public:
                long type() { return E_ATOM; }
                // union {
                //         object obj;
                //         //s1_ptr s;
                //         //d_ptr p;
                // };
                Atom() { obj = NOVALUE; } // default constructor
                ~Atom() { DeRefObj(obj); obj = NOVALUE; } // default destructor
                Atom(const Atom& x) { obj = x.obj; RefObj(obj); } // copy constructor
                Atom& operator= (const Atom& x) { DeRefObj(obj); obj = x.obj; RefObj(obj); return *this; } // copy assignment
                //Atom (Atom&& x) { obj = x.obj; x.obj = NOVALUE; } // move constructor
                //Atom& operator= (Atom&& x) { DeRefObj(obj); obj = x.obj; x.obj = NOVALUE; return *this; } // move assignment
                
                Atom(object ob) { obj = ob; }
                Atom(d_ptr ptr) { ++(ptr->ref); obj = MAKE_DBL(ptr); }
                Atom(double d) { obj = NewDouble(d); }
                Atom(int val) { obj = CHECK_INTEGER(val) ? val : NOVALUE; }
                
                Atom operator + (const Atom& param) { Atom ret; if(IS_ATOM_INT(obj) && IS_ATOM_INT(param.obj)) { ret.obj = add(obj, param.obj); return ret; } ret.obj = (object)NewDouble(GET_DOUBLE(obj) + GET_DOUBLE(param.obj)); return ret; }
                Atom operator - (const Atom& param) { Atom ret; if(IS_ATOM_INT(obj) && IS_ATOM_INT(param.obj)) { ret.obj = minus(obj, param.obj); return ret; } ret.obj = (object)NewDouble(GET_DOUBLE(obj) - GET_DOUBLE(param.obj)); return ret; }
                Atom operator * (const Atom& param) { Atom ret; if(IS_ATOM_INT(obj) && IS_ATOM_INT(param.obj)) { ret.obj = multiply(obj, param.obj); return ret; } ret.obj = (object)NewDouble(GET_DOUBLE(obj) * GET_DOUBLE(param.obj)); return ret; }
                Atom operator / (const Atom& param) { Atom ret; if(IS_ATOM_INT(obj) && IS_ATOM_INT(param.obj)) { ret.obj = divide(obj, param.obj); return ret; } else { Atom a(NEW_DOUBLE(obj)), b(NEW_DOUBLE(param.obj)); ret.obj = Ddivide(DBL_PTR(a.obj), DBL_PTR(b.obj)); return ret; } }
                
        };
        // Boolean atom functions:
        Integer A_unot(Atom a) { Integer ret; if(IS_ATOM_INT(a.obj)) { ret.obj = unot(a.obj); return ret; } else { ret.obj = Dnot(DBL_PTR(a.obj)); return ret; } }
        Integer A_equals(Atom a, Atom b) { Integer ret; if(IS_ATOM_INT(a.obj) && IS_ATOM_INT(b.obj)) { ret.obj = equals(a.obj, b.obj); return ret; } else { Atom s(NEW_DOUBLE(a.obj)), t(NEW_DOUBLE(b.obj)); ret.obj = Dequals(DBL_PTR(s.obj), DBL_PTR(t.obj)); return ret; } }
        Integer A_lessThan(Atom a, Atom b) { Integer ret; if(IS_ATOM_INT(a.obj) && IS_ATOM_INT(b.obj)) { ret.obj = less(a.obj, b.obj); return ret; } else { Atom s(NEW_DOUBLE(a.obj)), t(NEW_DOUBLE(b.obj)); ret.obj = Dless(DBL_PTR(s.obj), DBL_PTR(t.obj)); return ret; } }
        Integer A_greaterThan(Atom a, Atom b) { Integer ret; if(IS_ATOM_INT(a.obj) && IS_ATOM_INT(b.obj)) { ret.obj = greater(a.obj, b.obj); return ret; } else { Atom s(NEW_DOUBLE(a.obj)), t(NEW_DOUBLE(b.obj)); ret.obj = Dgreater(DBL_PTR(s.obj), DBL_PTR(t.obj)); return ret; } }
        Integer A_notEqual(Atom a, Atom b) { Integer ret; if(IS_ATOM_INT(a.obj) && IS_ATOM_INT(b.obj)) { ret.obj = noteq(a.obj, b.obj); return ret; } else { Atom s(NEW_DOUBLE(a.obj)), t(NEW_DOUBLE(b.obj)); ret.obj = Dnoteq(DBL_PTR(s.obj), DBL_PTR(t.obj)); return ret; } }
        Integer A_lessThanOrEqual(Atom a, Atom b) { Integer ret; if(IS_ATOM_INT(a.obj) && IS_ATOM_INT(b.obj)) { ret.obj = lesseq(a.obj, b.obj); return ret; } else { Atom s(NEW_DOUBLE(a.obj)), t(NEW_DOUBLE(b.obj)); ret.obj = Dlesseq(DBL_PTR(s.obj), DBL_PTR(t.obj)); return ret; } }
        Integer A_greaterThanOrEqual(Atom a, Atom b) { Integer ret; if(IS_ATOM_INT(a.obj) && IS_ATOM_INT(b.obj)) { ret.obj = greatereq(a.obj, b.obj); return ret; } else { Atom s(NEW_DOUBLE(a.obj)), t(NEW_DOUBLE(b.obj)); ret.obj = Dgreatereq(DBL_PTR(s.obj), DBL_PTR(t.obj)); return ret; } }
        Integer A_logicaland(Atom a, Atom b) { Integer ret; if(IS_ATOM_INT(a.obj) && IS_ATOM_INT(b.obj)) { ret.obj = Band(a.obj, b.obj); return ret; } else { Atom s(NEW_DOUBLE(a.obj)), t(NEW_DOUBLE(b.obj)); ret.obj = Dand(DBL_PTR(s.obj), DBL_PTR(t.obj)); return ret; } }
        Integer A_logicalor(Atom a, Atom b) { Integer ret; if(IS_ATOM_INT(a.obj) && IS_ATOM_INT(b.obj)) { ret.obj = Bor(a.obj, b.obj); return ret; } else { Atom s(NEW_DOUBLE(a.obj)), t(NEW_DOUBLE(b.obj)); ret.obj = Dor(DBL_PTR(s.obj), DBL_PTR(t.obj)); return ret; } }
        Integer A_logicalxor(Atom a, Atom b) { Integer ret; if(IS_ATOM_INT(a.obj) && IS_ATOM_INT(b.obj)) { ret.obj = Bxor(a.obj, b.obj); return ret; } else { Atom s(NEW_DOUBLE(a.obj)), t(NEW_DOUBLE(b.obj)); ret.obj = Dxor(DBL_PTR(s.obj), DBL_PTR(t.obj)); return ret; } }
        // Regular atom functions:
        Atom A_remainder(Atom a, Atom b) { Atom ret; if(IS_ATOM_INT(a.obj) && IS_ATOM_INT(b.obj)) { ret.obj = eremainder(a.obj, b.obj); return ret; } else { Atom s(NEW_DOUBLE(a.obj)), t(NEW_DOUBLE(b.obj)); ret.obj = Dremainder(DBL_PTR(s.obj), DBL_PTR(t.obj)); return ret; } }
        Atom A_power(Atom a, Atom b) { Atom ret; if(IS_ATOM_INT(a.obj) && IS_ATOM_INT(b.obj)) { ret.obj = power(a.obj, b.obj); return ret; } else { Atom s(NEW_DOUBLE(a.obj)), t(NEW_DOUBLE(b.obj)); ret.obj = Dpower(DBL_PTR(s.obj), DBL_PTR(t.obj)); return ret; } }
        // 32-bit atom functions:
        Atom A_and_bits(Atom a, Atom b) { Atom ret; if(IS_ATOM_INT(a.obj) && IS_ATOM_INT(b.obj)) { ret.obj = and_bits(a.obj, b.obj); return ret; } else { Atom s(NEW_DOUBLE(a.obj)), t(NEW_DOUBLE(b.obj)); ret.obj = Dand_bits(DBL_PTR(s.obj), DBL_PTR(t.obj)); return ret; } }
        Atom A_or_bits(Atom a, Atom b) { Atom ret; if(IS_ATOM_INT(a.obj) && IS_ATOM_INT(b.obj)) { ret.obj = or_bits(a.obj, b.obj); return ret; } else { Atom s(NEW_DOUBLE(a.obj)), t(NEW_DOUBLE(b.obj)); ret.obj = Dor_bits(DBL_PTR(s.obj), DBL_PTR(t.obj)); return ret; } }
        Atom A_xor_bits(Atom a, Atom b) { Atom ret; if(IS_ATOM_INT(a.obj) && IS_ATOM_INT(b.obj)) { ret.obj = xor_bits(a.obj, b.obj); return ret; } else { Atom s(NEW_DOUBLE(a.obj)), t(NEW_DOUBLE(b.obj)); ret.obj = Dxor_bits(DBL_PTR(s.obj), DBL_PTR(t.obj)); return ret; } }
        // Unary Ops
        Atom A_not_bits(Atom a) { Atom ret; if(IS_ATOM_INT(a.obj)) { ret.obj = not_bits(a.obj); return ret; } else { ret.obj = Dnot_bits(DBL_PTR(a.obj)); return ret; } }
        Atom A_uminus(Atom a) { Atom ret; if(IS_ATOM_INT(a.obj)) { ret.obj = uminus(a.obj); return ret; } else { ret.obj = Duminus(DBL_PTR(a.obj)); return ret; } }
        Atom A_sqrt(Atom a) { Atom ret; if(IS_ATOM_INT(a.obj)) { ret.obj = e_sqrt(a.obj); return ret; } else { ret.obj = De_sqrt(DBL_PTR(a.obj)); return ret; } }
        Atom A_sin(Atom a) { Atom ret; if(IS_ATOM_INT(a.obj)) { ret.obj = e_sin(a.obj); return ret; } else { ret.obj = De_sin(DBL_PTR(a.obj)); return ret; } }
        Atom A_cos(Atom a) { Atom ret; if(IS_ATOM_INT(a.obj)) { ret.obj = e_cos(a.obj); return ret; } else { ret.obj = De_cos(DBL_PTR(a.obj)); return ret; } }
        Atom A_tan(Atom a) { Atom ret; if(IS_ATOM_INT(a.obj)) { ret.obj = e_tan(a.obj); return ret; } else { ret.obj = De_tan(DBL_PTR(a.obj)); return ret; } }
        Atom A_arctan(Atom a) { Atom ret; if(IS_ATOM_INT(a.obj)) { ret.obj = e_arctan(a.obj); return ret; } else { ret.obj = De_arctan(DBL_PTR(a.obj)); return ret; } }
        Atom A_log(Atom a) { Atom ret; if(IS_ATOM_INT(a.obj)) { ret.obj = e_log(a.obj); return ret; } else { ret.obj = De_log(DBL_PTR(a.obj)); return ret; } }
        Atom A_floor(Atom a) { Atom ret; if(IS_ATOM_INT(a.obj)) { ret.obj = e_floor(a.obj); return ret; } else { ret.obj = De_floor(DBL_PTR(a.obj)); return ret; } }
        
        class Sequence : public base_class
        {
        public:
                long type() { return E_SEQUENCE; }
                // union {
                //         object obj;
                //         //s1_ptr s;
                //         //d_ptr p;
                // };
                Sequence() { obj = NOVALUE; } // default constructor
                ~Sequence() { DeRefObj(obj); obj = NOVALUE; } // default destructor
                Sequence(const Sequence& x) { obj = x.obj; RefObj(obj); } // copy constructor
                Sequence& operator= (const Sequence& x) { DeRefObj(obj); obj = x.obj; RefObj(obj); return *this; } // copy assignment
                //Sequence (Sequence&& x) { obj = x.obj; x.obj = NOVALUE; } // move constructor
                //Sequence& operator= (Sequence&& x) { DeRefObj(obj); obj = x.obj; x.obj = NOVALUE; return *this; } // move assignment
                
                Sequence(object ob) { obj = ob; }
                Sequence(const char * str) { obj = NewString((unsigned char*) str); }
                long length() { return (IS_DBL_OR_SEQUENCE(obj) && IS_SEQUENCE(obj)) ? SEQ_PTR(obj)->length : -1; }
                void NewStr(const char * str) { DeRefObj(obj); obj = NewString((unsigned char*) str); }
                char * GetCharStr(void) {
                        if (IS_DBL_OR_SEQUENCE(obj) && IS_SEQUENCE(obj) && IS_SEQ_STRING(obj)) {
                                long len = SEQ_PTR(obj)->length;
                                char * str = (char *)malloc(len + 1);
                                MakeCString(str, obj);
                                return str;
                        }
                        return NULL;
                }
                void ScreenOutput(FILE *f) { char * out_string = GetCharStr(); screen_output(f, out_string); }
                
                object e_at(int i) { // use (1 to length) or (-1 to -length)
                        if (IS_DBL_OR_SEQUENCE(obj) && IS_SEQUENCE(obj) && CHECK_INTEGER(i)) {
                                s1_ptr ptr = SEQ_PTR(obj);
                                long len = ptr->length;
                                if (i < 0) {
                                        i += len;
                                        i++;
                                }
                                if ((i >= 1) && (i <= len)) {
                                        object ret = ptr->base[i];
                                        RefObj(ret);
                                        return ret;
                                }
                        }
                        return NOVALUE;
                }
                int e_construct_slice(Sequence src, object start, object end) {
                        if (IS_DBL_OR_SEQUENCE(obj) && IS_SEQUENCE(obj) &&
                        IS_DBL_OR_SEQUENCE(src.obj) && IS_SEQUENCE(src.obj)) {
                                rhs_slice_target = &obj; // DeRef()'s "obj" in next statement below.
                                RHS_Slice((s1_ptr)src.obj, start, end); // takes all object's.
                                rhs_slice_target = NULL;
                                return TRUE;
                        }
                        return FALSE;
                }
                int e_assign_to_slice(object start, object end, Sequence val) {
                        if (IS_DBL_OR_SEQUENCE(obj) && IS_SEQUENCE(obj) &&
                        IS_DBL_OR_SEQUENCE(val.obj) && IS_SEQUENCE(val.obj)) {
                                assign_slice_seq = (s1_ptr *)&obj;
                                AssignSlice(start, end, (s1_ptr)val.obj);
                                assign_slice_seq = NULL;
                                return TRUE;
                        }
                        return FALSE;
                }
                //AssignSlice(object start, object end, s1_ptr val);
                void prepend(object a) { Ref(a) Prepend(&obj, obj, a); }
                void append(object a) { Ref(a) Append(&obj, obj, a); }
                
                
        };
        void S_prepend(Sequence target, Sequence src, object a)
        {
                Ref(a)
                Prepend(&(target.obj), src.obj, a);
        }
        void S_append(Sequence target, Sequence src, object a)
        {
                Ref(a)
                Append(&(target.obj), src.obj, a);
        }
        void S_concat(Sequence target, object a, object b)
        {
                bool is_seq_a = IS_DBL_OR_SEQUENCE(a) && IS_SEQUENCE(a);
                bool is_seq_b = IS_DBL_OR_SEQUENCE(b) && IS_SEQUENCE(b);
                if (is_seq_a && (!is_seq_b))
                {
                        Ref(b)
                        Append(&(target.obj), a, b);
                }
                else if ((!is_seq_a) && is_seq_b)
                {
                        Ref(a)
                        Prepend(&(target.obj), b, a);
                }
                else
                {
                        Concat(&(target.obj), a, (s1_ptr)b);
                }
        }
        void S_concatN(Sequence target, Sequence source)
        {
                s1_ptr p = SEQ_PTR(source.obj);
                Concat_N(&(target.obj), p->base, p->length);
        }
        Sequence S_repeat(object item, object repcount)
        {
                Sequence ret;
                ret.obj = Repeat(item, repcount);
                return ret;
        }
        
        
        class Object : public base_class
        {
        public:
                long type() { return E_OBJECT; }
                // union {
                //         object obj;
                //         //s1_ptr seq_ptr;
                //         //d_ptr dbl_ptr;
                // };
                Object() { obj = NOVALUE; } // default constructor
                ~Object() { DeRefObj(obj); obj = NOVALUE; } // default destructor
                Object(const Object& x) { obj = x.obj; RefObj(obj); } // copy constructor
                Object& operator= (const Object& x) { DeRefObj(obj); obj = x.obj; RefObj(obj); return *this; } // copy assignment
                //Object(Object&& x) { obj = x.obj; x.obj = NOVALUE; } // move constructor
                //Object& operator= (Object&& x) { DeRefObj(obj); obj = x.obj; x.obj = NOVALUE; return *this; } // move assignment
                
                Object(object ob) { obj = ob; }
                void TryDoubleToInt(void) { if (IS_DBL_OR_SEQUENCE(obj) && IS_ATOM_DBL(obj)) { obj = DoubleToInt(obj); } }
                char GetChar() { return doChar(obj); } // aborts if type is sequence.
                
                
                
        };
        
        Integer E_compare(Object a, Object b) { Integer ret; ret.obj = compare(a.obj, b.obj); return ret; }
        Integer E_find(Object a, Sequence b) { Integer ret; ret.obj = find(a.obj, (s1_ptr)b.obj); return ret; }
        Integer E_match(Sequence a, Sequence b) { Integer ret; if (SEQ_PTR(a.obj)->length > 0) { ret.obj = e_match((s1_ptr)a.obj, (s1_ptr)b.obj); } else { ret.obj = -1; } return ret; }
        
        Integer E_find_from(Object a, Sequence b, object c) { Integer ret; ret.obj = find_from(a.obj, (s1_ptr)b.obj, c); return ret; }
        Integer E_match_from(Sequence a, Sequence b, object c) { Integer ret; if (SEQ_PTR(a.obj)->length > 0) { ret.obj = e_match_from((s1_ptr)a.obj, (s1_ptr)b.obj, c); } else { ret.obj = -1; } return ret; }
        
        
}
