//
// be_eu.c

#include "be_eu.h"

/* convert atom to char. *must avoid side effects in elem* */
#define Char(elem) ((IS_ATOM_INT(elem)) ? ((char)INT_VAL(elem)) : doChar(elem)) 

char TempBuff[TEMP_SIZE]; /* buffer for error messages */
object *rhs_slice_target;
s1_ptr *assign_slice_seq;

//From: be_w.c

void screen_output(FILE *f, char *out_string)
{
    fputs(out_string, f);
}

//From: be_runtime.c

void Cleanup(int status)
/* clean things up before leaving 0 - ok, non 0 - error */
{
#ifdef EWINDOWS
    // Note: ExitProcess() - frees all the dlls but won't flush the regular files
    for (i = 0; i < open_dll_count; i++) {
	FreeLibrary(open_dll_list[i]);
    }
#endif      
    exit(status);
}

void UserCleanup(int status)
/* Euphoria abort() */
{
//    user_abort = TRUE;
    Cleanup(status);
}

void SimpleRTFatal(char *msg)
/* Fatal errors for translated code */
{
//    if (crash_msg == NULL || crash_count > 0) {
	screen_output(stderr, "\nFatal run-time error:\n");
	screen_output(stderr, msg);
	screen_output(stderr, "\n\n");
//    }
//    else {
//	screen_output(stderr, crash_msg);
//    }
//    TempErrFile = fopen(TempErrName, "w");
//    if (TempErrFile != NULL) {
//	fprintf(TempErrFile, "Fatal run-time error:\n");
//	fprintf(TempErrFile, "%s\n", msg);
//	
//	if (last_traced_line != NULL) {
//	    if (crash_msg == NULL || crash_count > 0)
//		fprintf(stderr, "%s\n", last_traced_line);
//	    fprintf(TempErrFile, "%s\n", last_traced_line);
//	}
//	fclose(TempErrFile);
//    }

//    call_crash_routines();
//    gameover = TRUE;
    Cleanup(1); 
}

void RTFatal(char *msg)
/* handle run time fatal errors */
{
//#ifndef ERUNTIME
//    if (Executing) 
//	CleanUpError(msg, NULL);
//    else
//#endif  
	SimpleRTFatal(msg);
}

//From: be_alloc.c

void SpaceMessage()
{
    /* should we free up something first, to ensure fprintf's work? */
    
    RTFatal("Your program has run out of memory.\nOne moment please...");
}

char *EMalloc(unsigned long nbytes) 
/* storage allocator */
/* Always returns a pointer that has 8-byte alignment (essential for our
   internal representation of an object). */
{
    char * ret = (char*)malloc(nbytes);
    if (((long)ret) % 8)
    {
    	RTFatal("Error: EMalloc does not have 8-byte alignment.");
    }
    return ret;
}

char *ERealloc(char *orig, unsigned long newsize)
/* Enlarge or shrink a malloc'd block. 
   orig must not be NULL - not supported.
   Return a pointer to a storage area of the desired size
   containing all the original data.
   I don't think a shrink could ever become an expansion + copy
   by accident, but newsize might be less than the current size! */
{
    // we always have 8-alignment
    char * ret = (char*)realloc(orig, newsize);
    if (((long)ret) % 8)
    {
    	RTFatal("Error: ERealloc does not have 8-byte alignment.");
    }
    return ret;
}

s1_ptr NewS1(long size)
/* make a new s1 sequence block with a single reference count */
/* size is number of elements, NOVALUE is added as an end marker */
{
    register s1_ptr s1;

    if (size > 1073741800) {
	// multiply by 4 could overflow 32 bits
	SpaceMessage();
	return NULL;
    }
    s1 = (s1_ptr)EMalloc(sizeof(struct s1) + (size+1) * sizeof(object));
    s1->ref = 1;
    s1->base = (object_ptr)(s1 + 1);
    s1->length = size;
    s1->postfill = 0; /* there may be some available but don't waste time */
		      /* prepend assumes this is set to 0 */
    s1->base[size] = NOVALUE;
    s1->base--;  // point to "0th" element
    return(s1);
}

object NewString(const char *s)
/* create a new string sequence */
{
    int len;
    object_ptr obj_ptr;
    s1_ptr c1;

    len = strlen(s);
    c1 = NewS1((long)len);
    obj_ptr = (object_ptr)c1->base;
    if (len > 0) {
	do {
	    *(++obj_ptr) = (unsigned char)*s++;
	} while (--len > 0);
    }
    return MAKE_SEQ(c1);
}

s1_ptr SequenceCopy(register s1_ptr a)
/* take a single-ref copy of sequence 'a' */
{
    s1_ptr c;
    register object_ptr cp, ap;
    register long length;
    register object temp_ap;

    /* a is a SEQ_PTR */
    length = a->length;
    c = NewS1(length);
    cp = c->base;
    ap = a->base;
    while (TRUE) {  // NOVALUE will be copied
	temp_ap = *(++ap);
	*(++cp) = temp_ap;
	if (!IS_ATOM_INT(temp_ap)) {
	    if (temp_ap == NOVALUE)
		break;
	    RefDS(temp_ap);
	}
    } 
    DeRefSP(a);
    return c;
}

object NewDouble(double d)
/* allocate space for a new double value */
{
    register d_ptr n;

    n = (d_ptr)EMalloc((long)D_SIZE);

    n->ref = 1;
    n->dbl = d;
    return MAKE_DBL(n);
}

object Dadd(d_ptr a, d_ptr b)
/* double add */
{
    return (object)NewDouble(a->dbl + b->dbl);
}


object Dminus(d_ptr a, d_ptr b)
/* double subtract */
{
    return (object)NewDouble(a->dbl - b->dbl);
}


object Dmultiply(d_ptr a, d_ptr b)
/* double multiply */
{
    return (object)NewDouble(a->dbl * b->dbl); 
}

//From: be_runtime.c

char doChar(object elem)
/* convert to char (int done in-line) */
{
    if (IS_ATOM_INT(elem)) 
	return (char)elem;
    if (IS_ATOM(elem)) 
	return (char)(DBL_PTR(elem)->dbl);
    else {
	RTFatal("sequence found inside character string");
	return '\0';
    }
}


void Prepend(object_ptr target, object s1, object a)
/* prepend object 'a' onto front of s1 sequence. Caller should
   increment ref count if necessary. */
{
    object_ptr p, q;
    s1_ptr t;
    s1_ptr s1p, new_seq;
    long len, new_len;
    object temp;
    
    t = (s1_ptr)*target;
    s1p = SEQ_PTR(s1);
    len = s1p->length;
    if ((s1_ptr)s1 == t && s1p->ref == 1) {
	/* we're free to prepend in-place */
	/* Check for room at beginning */
	if (s1p->base >= (object_ptr)(s1p+1)) {
	    s1p->length++;
	    *(s1p->base) = a;
	    s1p->base--;
	    return;
	}
	/* OPTIMIZE: check for postfill & copy down */
	/* OPTIMIZE: check for extra room in malloc'd area? */
	/* OPTIMIZE: Do an _expand() if possible */
    }
    /* make a new sequence */
    new_len = EXTRA_EXPAND(len);
    new_seq = NewS1(new_len);
    new_seq->length = len + 1;
    new_seq->base += new_len - new_seq->length; /* make room at beginning */
    p = new_seq->base+1;
    *p = a;
    q = s1p->base;
    while (TRUE) {  // NOVALUE will be copied
	temp = *(++q);
	*(++p) = temp;
	if (!IS_ATOM_INT(temp)) {
	    if (temp == NOVALUE)
		break;
	    RefDS(temp);
	}
    }
    DeRef(*target);
    *target = MAKE_SEQ(new_seq);
}

void Append(object_ptr target, object s1, object a)
/* append object 'a' onto the end of s1 sequence. Caller should
   increment ref count if necessary. */
{
    object_ptr p, q;
    s1_ptr t;
    s1_ptr s1p, new_s1p, new_seq;
    long len, new_len;
    object_ptr base, last;
    object temp;
    
    t = (s1_ptr)*target;
    s1p = SEQ_PTR(s1);
    len = s1p->length;

    if ((s1_ptr)s1 == t && s1p->ref == 1) {
	/* we're free to append in-place */
	if (s1p->postfill == 0) {
	    /* make some more postfill space */
	    new_len = EXTRA_EXPAND(len);
	    base = s1p->base;
	    /* allow 1*4 for end marker */
	    /* base + new_len + 2 could overflow 32-bits??? */
	    new_s1p = (s1_ptr)ERealloc((char *)s1p, 
			       (char *)(base + new_len + 2) - (char *)s1p);
	    new_s1p->base = (object_ptr)new_s1p + 
			     ((object_ptr)base - (object_ptr)s1p);
	    s1p = new_s1p;
	    s1p->postfill = new_len - len;
	    *target = MAKE_SEQ(s1p);
	/* OPTIMIZE: we may have more space in the malloc'd block
	   than we think, due to power of 2 round up etc. Can
	   we find out what we have and increment postfill 
	   accordingly? Then we can usually avoid memcopying too much
	   in Realloc. */
	}
	s1p->postfill--;
	s1p->length++;
	last = s1p->base + len + 1;
	*last = a; 
	*(last+1) = NOVALUE;  // make a new end marker
	return;
    }
    /* make a new sequence */
    new_len = EXTRA_EXPAND(len);
    new_seq = NewS1(new_len);
    new_seq->length = len + 1;
    new_seq->postfill = new_len - new_seq->length;
    p = new_seq->base;
    q = s1p->base;
    while (TRUE) {  // NOVALUE will be copied
	temp = *(++q);
	*(++p) = temp;
	if (!IS_ATOM_INT(temp)) {
	    if (temp == NOVALUE)
		break;
	    RefDS(temp);
	}
    }
    *p++ = a;
    *p = NOVALUE; // end marker
    DeRef(*target);
    *target = MAKE_SEQ(new_seq);
}


void Concat(object_ptr target, object a_obj, s1_ptr b)
/* concatenate a & b, put result in new object c */
/* new object created - no copy needed to avoid circularity */
/* only handles seq & seq and atom & atom */
/* seq & atom done by append, atom & seq done by prepend */
{
    object_ptr p, q;
    s1_ptr c, a;
    long na, nb;
    object temp;

    if (IS_ATOM(a_obj)) {
	c = NewS1(2);
	/* both are atoms */
	*(c->base+1) = a_obj;
	Ref(a_obj); 
	*(c->base+2) = (object)b;
	Ref((object)b);
    }
    else {
	/* both are sequences */
	a = SEQ_PTR(a_obj);
	b = SEQ_PTR(b);
	na = a->length;
	nb = b->length;
	
	if (a_obj == *target && 
	    a->ref == 1 && 
	    na > ((nb - a->postfill) << 3)) {
	    /* try to update in-place */
	    int insert;
	    object temp;
	    
	    q = b->base+1;
	    while (nb > 0) {
		insert = (nb <= a->postfill) ? nb : a->postfill;            
		p = a->base + 1 + a->length;
		a->postfill -= insert;
		a->length += insert;
		nb -= insert;
		while (--insert >= 0) {
		    temp = *q++;
		    *p++ = temp;
		    Ref(temp);
		}
		*p = NOVALUE; // end marker
		if (nb > 0) {
		    Ref(*q);
		    Append(target, a_obj, *q);
		    a_obj = *target;
		    a = SEQ_PTR(a_obj);
		    nb--;
		    q++;
		}
	    }
	    return;
	}
	
	c = NewS1(na + nb);

	p = c->base;
	q = a->base;
	while (TRUE) {  // NOVALUE will be copied
	    temp = *(++q);
	    *(++p) = temp;
	    if (!IS_ATOM_INT(temp)) {
		if (temp == NOVALUE)
		    break;
		RefDS(temp);
	    }
	}
	q = b->base;
	while (TRUE) {  // NOVALUE will be copied
	    temp = *(++q);
	    *p++ = temp;
	    if (!IS_ATOM_INT(temp)) {
		if (temp == NOVALUE)
		    break;
		RefDS(temp);
	    }
	}
    }
    
    DeRef(*target);
    *target = MAKE_SEQ(c);
}

void Concat_N(object_ptr target, object_ptr  source, int n)
/* run-time library version for Translator
 * Concatenate n objects (n > 2). This is more efficient 
 * than doing multiple calls to Concat() above, since we
 * can allocate space for the final result, and copy all 
 * the data just one time. 
 */
{
    s1_ptr result;
    object s_obj, temp;
    int i, size;
    object_ptr p, q;
    
    /* Compute the total size of all the operands */
    size = 0;
    for (i = 1; i <= n; i++) {
	s_obj = *source++;
	if (IS_ATOM(s_obj))
	    size += 1;
	else
	    size += SEQ_PTR(s_obj)->length;
    }
    
    /* Allocate the result sequence */
    result = NewS1(size);
    
    /* Copy the operands into the result. */
    /* The operands are in reverse order. */
    p = result->base+1;
    for (i = 1; i <= n; i++) {
	s_obj = *(--source);
	if (IS_ATOM(s_obj)) {
	    *p++ = s_obj;
	    Ref(s_obj);
	}
	else {
	    /* sequence */
	    q = SEQ_PTR(s_obj)->base;
	    while (TRUE) {  // NOVALUE will be copied
		temp = *(++q);
		*p++ = temp;
		if (!IS_ATOM_INT(temp)) {
		    if (temp == NOVALUE)
			break;
		    RefDS(temp);
		}
	    }
	    p--;
	}
    }
    
    DeRef(*target);
    *target = MAKE_SEQ(result);
}

void Concat_Ni(object_ptr target, object_ptr *source, int n)
/* version used by interpreter
 * Concatenate n objects (n > 2). This is more efficient 
 * than doing multiple calls to Concat() above, since we
 * can allocate space for the final result, and copy all 
 * the data just one time. 
 */
{
    s1_ptr result;
    object s_obj, temp;
    int i, size;
    object_ptr p, q;
    
    /* Compute the total size of all the operands */
    size = 0;
    for (i = 1; i <= n; i++) {
	s_obj = **source++;
	if (IS_ATOM(s_obj))
	    size += 1;
	else
	    size += SEQ_PTR(s_obj)->length;
    }
    
    /* Allocate the result sequence */
    result = NewS1(size);
    
    /* Copy the operands into the result. */
    /* The operands are in reverse order. */
    p = result->base+1;
    for (i = 1; i <= n; i++) {
	s_obj = **(--source);
	if (IS_ATOM(s_obj)) {
	    *p++ = s_obj;
	    Ref(s_obj);
	}
	else {
	    /* sequence */
	    q = SEQ_PTR(s_obj)->base;
	    while (TRUE) {  // NOVALUE will be copied
		temp = *(++q);
		*p++ = temp;
		if (!IS_ATOM_INT(temp)) {
		    if (temp == NOVALUE)
			break;
		    RefDS(temp);
		}
	    }
	    p--;
	}
    }
    
    DeRef(*target);
    *target = MAKE_SEQ(result);
}

// used by translator 
void RepeatElem(int *addr, object item, int repcount)
/* replicate an object in memory - used by RIGHT_BRACE op */
/* repcount will be at least 10 */
{
    if (IS_DBL_OR_SEQUENCE(item)) {
	(DBL_PTR(item)->ref) += repcount;
    }
    while (repcount-- > 0) {
	*addr++ = item;
    }
}

object Repeat(object item, object repcount)
/* generate a sequence of <item> repeated <count> times */
{
    object_ptr obj_ptr;
    double d;
    long count;
    s1_ptr s1;
    
    if (IS_ATOM_INT(repcount)) {
	count = repcount;
	if (count < 0) 
	    RTFatal("repetition count must not be negative");
    }
    
    else if (IS_ATOM_DBL(repcount)) {
	d = DBL_PTR(repcount)->dbl;
	if (d > MAXINT_DBL)
	    RTFatal("repetition count is too large");
	if (d < 0.0)
	    RTFatal("repetition count must not be negative");
	count = (long)d;
    }
    
    else
	RTFatal("repetition count must be an atom");
    
    
    s1 = NewS1(count);
    obj_ptr = s1->base+1;

    if (IS_ATOM_INT(item)) {
	while (count >= 10) {
	    obj_ptr[0] = item;
	    obj_ptr[1] = item;
	    obj_ptr[2] = item;
	    obj_ptr[3] = item;
	    obj_ptr[4] = item;
	    obj_ptr[5] = item;
	    obj_ptr[6] = item;
	    obj_ptr[7] = item;
	    obj_ptr[8] = item;
	    obj_ptr[9] = item;
	    obj_ptr += 10;
	    count -= 10;
	};
	while (count > 0) {
	    *obj_ptr++ = item;  
	    count--;
	};
    }
    else {
	(DBL_PTR(item)->ref) += count;
	while (--count >= 0) {
	    *obj_ptr++ = item;  
	};
    }
    return MAKE_SEQ(s1);
}

/* non-recursive - no chance of stack overflow */
void de_reference(s1_ptr a)
/* frees an object whose reference count is 0 */
/* a must not be an ATOM_INT */
{
    object_ptr p;
    object t;
    
#ifdef EXTRA_CHECK
    s1_ptr a1;
    
    if ((long)a == NOVALUE || IS_ATOM_INT(a)) 
	RTInternal("bad object passed to de_reference");
    if (DBL_PTR(a)->ref > 1000)
	RTInternal("more than 1000 refs"); 
#endif    
    if (IS_ATOM_DBL(a)) {
#ifdef EXTRA_CHECK
	a1 = (s1_ptr)DBL_PTR(a);
	if (a1->ref < 0)
	    RTInternal("f.p. reference count less than 0");
#endif
	a = (s1_ptr)DBL_PTR(a);
	FreeD((unsigned char *)a);
    }

    else { /* SEQUENCE */
	/* sequence reference count has reached 0 */
	a = SEQ_PTR(a);
	p = a->base;
#ifdef EXTRA_CHECK
	if (a->ref < 0)
	    RTInternal("sequence reference count less than 0");
	if (*(p+(a->length+1)) != NOVALUE)
	    RTInternal("Sentinel missing!\n");
#endif
	while (TRUE) {
	    p++;
	    t = *p;
#ifdef EXTRA_CHECK
	    if (t > MAXINT)
		RTInternal("de_reference: invalid object found!");
#endif
	    if (!IS_ATOM_INT(t)) {
		if (t == NOVALUE) {
		    // end of sequence: back up a level
		    p = (object_ptr)a->length;
		    t = (object)a->ref;
		    EFree((char *)a); 
		    a = (s1_ptr)t;
		    if (a == NULL)
			break;  // it's the top-level sequence - quit
		}
		else if (--(DBL_PTR(t)->ref) == 0) {
		    if (IS_ATOM_DBL(t)) {
			FreeD((unsigned char *)DBL_PTR(t));
		    }
		    else {
			// switch to subsequence
			// was: de_reference((s1_ptr)t);
			t = (object)SEQ_PTR(t);
			((s1_ptr)t)->ref = (long)a;
			((s1_ptr)t)->length = (long)p;
			a = (s1_ptr)t;
			p = a->base;
		    }
		}
	    }
	}
    } 
}

void DeRef1(int a)
/* Saves space. Use in top-level code (outside of loops) */
{
    DeRef(a);
}

void DeRef5(int a, int b, int c, int d, int e)
/* Saves space. Use instead of 5 in-line DeRef's */
{
    DeRef(a);
    DeRef(b);
    DeRef(c);
    DeRef(d);
    DeRef(e);
}

/* NEW - non-recursive - only integer elements */
void de_reference_i(s1_ptr a)
/* frees an object whose reference count is 0 */
/* We know that if there are any sequence elements, 
   they will all be integers */
/* a must not be an ATOM_INT */
{
    //object_ptr p;
    //object t;
    
#ifdef EXTRA_CHECK
    s1_ptr a1;
    
    if ((long)a == NOVALUE || IS_ATOM_INT(a)) 
	RTInternal("bad object passed to de_reference");
    if (DBL_PTR(a)->ref > 1000)
	RTInternal("more than 1000 refs"); 
#endif    
    if (IS_ATOM_DBL(a)) {  
#ifdef EXTRA_CHECK
	a1 = (s1_ptr)DBL_PTR(a);
	if (a1->ref < 0)
	    RTInternal("f.p. reference count less than 0");
#endif
	a = (s1_ptr)DBL_PTR(a);
	FreeD((unsigned char *)a);
    }

    else { /* SEQUENCE */
	/* sequence reference count has reached 0 */
	a = SEQ_PTR(a);
#ifdef EXTRA_CHECK
	if (a->ref < 0)
	    RTInternal("sequence reference count less than 0");
#endif
	EFree((char *)a); 
    } 
}

object DoubleToInt(object d)
/* try to convert a double to an integer, if possible */
{
    double temp_dbl;

    temp_dbl = DBL_PTR(d)->dbl;
    if (floor(temp_dbl) == temp_dbl && 
	temp_dbl <= MAXINT_DBL &&
	temp_dbl >= MININT_DBL) {
	    /* return it in integer repn */
	    return MAKE_INT((long)temp_dbl);
    }
    else
	return d; /* couldn't convert */
}


/* --- Binary Ops --- */

object x()
/* error routine */
{
#ifdef EXTRA_CHECK
    RTInternal("bad fcode");
#endif  
    return NOVALUE;
}


object add(long a, long b)
/* integer add */
{
    long c;

    c = a + b;
    if (c + HIGH_BITS < 0)    
	return MAKE_INT(c);
    else
	return (object)NewDouble((double)c);
}

object minus(long a, long b)
/* integer subtract */
{
    long c;

    c = a - b;
    if (c + HIGH_BITS < 0)
	return MAKE_INT(c);
    else
	return (object)NewDouble((double)c);
}

object multiply(long a, long b)
/* integer multiply */
/* n.b. char type is signed */
{
    if (a == (short)a) {
	if ((b <= INT15 && b >= -INT15) || 
	   (a == (char)a && b <= INT23 && b >= -INT23) ||
	   (b == (short)b && a <= INT15 && a >= -INT15))
	    return MAKE_INT(a * b);
    }
    else if (b == (char)b && a <= INT23 && a >= -INT23)
	return MAKE_INT(a * b);

    return (object)NewDouble(a * (double)b);
}

object divide(long a, long b)
/* compute a / b */
{
    if (b == 0)
	RTFatal("attempt to divide by 0");
    if (a % b != 0) 
	return (object)NewDouble((double)a / b);
    else
	return MAKE_INT(a / b);
}

object Ddivide(d_ptr a, d_ptr b)
/* double divide */
{
    if (b->dbl == 0.0)
	RTFatal("attempt to divide by 0");
    return (object)NewDouble(a->dbl / b->dbl);
}

object eremainder(long a, long b)  // avoid conflict with "remainder" math fn
/* integer remainder of a divided by b */
{
    if (b == 0)
	RTFatal("can't get remainder of a number divided by 0");
    return MAKE_INT(a % b);
}

object Dremainder(d_ptr a, d_ptr b)
/* double remainder of a divided by b */
{
    if (b->dbl == 0.0)
	RTFatal("can't get remainder of a number divided by 0");
    return (object)NewDouble(fmod(a->dbl, b->dbl)); /* for now */
}

/* bitwise ops: as long as both are Euphoria integers then
   the result will always be a Euphoria integer. True for
   and/or/xor/not. This is because a Euphoria integer has the upper two
   bits the same - both 0 or both 1, and this fact can't change
   due to a bitwise op. */

void check32(d_ptr a, d_ptr b)
/* check for doubles that are greater than 32-bits */
{
    if (a->dbl < MIN_BITWISE_DBL ||
	a->dbl > MAX_BITWISE_DBL ||
	b->dbl < MIN_BITWISE_DBL ||
	b->dbl > MAX_BITWISE_DBL)
	RTFatal("bitwise operations are limited to 32-bit numbers");
}

object and_bits(long a, long b)
/* integer a AND b */
{
    return MAKE_INT(a & b);
}

object Dand_bits(d_ptr a, d_ptr b)
/* double a AND b */
{
    unsigned long longa, longb;
    long c;
    
    check32(a, b);
    longa = a->dbl;
    longb = b->dbl;
    c = longa & longb;
    if (c > NOVALUE && c < TOO_BIG_INT) 
	return c; // an integer
    else
	return (object)NewDouble((double)c); 
}

object or_bits(long a, long b)
/* integer a OR b */
{
    return MAKE_INT(a | b);
}

object Dor_bits(d_ptr a, d_ptr b)
/* double a OR b */
{
    unsigned long longa, longb;
    long c;
    
    check32(a, b);
    longa = a->dbl;
    longb = b->dbl;
    c = longa | longb;
    if (c > NOVALUE && c < TOO_BIG_INT) 
	return c; // an integer
    else
	return (object)NewDouble((double)c); 
}

object xor_bits(long a, long b)
/* integer a XOR b */
{
    return MAKE_INT(a ^ b);
}

object Dxor_bits(d_ptr a, d_ptr b)
/* double a XOR b */
{
    unsigned long longa, longb;
    long c;
    
    check32(a, b);
    longa = a->dbl;
    longb = b->dbl;
    c = longa ^ longb;
    if (c > NOVALUE && c < TOO_BIG_INT) 
	return c; // an integer
    else
	return (object)NewDouble((double)c); 
}

object not_bits(long a)
/* integer bitwise NOT of a */
{
    return MAKE_INT(~a); // Euphoria integer will produce Euphoria integer
}

object Dnot_bits(d_ptr a)
/* double bitwise NOT of a */
{
    unsigned long longa;
    long c;
    
    if (a->dbl < MIN_BITWISE_DBL ||
	a->dbl > MAX_BITWISE_DBL)
	 check32(a, a);  // error msg
    longa = a->dbl;
    c = ~longa;
    if (c > NOVALUE && c < TOO_BIG_INT) 
	return c; // an integer
    else
	return (object)NewDouble((double)c); 
}

object power(long a, long b)
/* integer a to the power b */
{
    long i, p;

    if (a == 2 && b >= 0 && b <= 29) {
	/* positive power of 2 */
	return MAKE_INT(1 << b);
    }
    else if (a == 0 && b <= 0) {
	RTFatal("can't raise 0 to power <= 0");
	return 0;
    }
    else if (b == 0) {
	return ATOM_1;
    }
    else if (b >= 1 && b <= 4 && a >= -178 && a <= 178) {
	p = a;  
	for (i = 2; i <= b; i++)
	    p = p * a;
	return MAKE_INT(p);
    }
    else
	return (object)NewDouble(pow((double)a, (double)b));
}

object Dpower(d_ptr a, d_ptr b)
/* double power */
{
    if (a->dbl == 0.0 && b->dbl <= 0.0)
	RTFatal("can't raise 0 to power <= 0");
    if (a->dbl < 0.0 && floor(b->dbl) != b->dbl)
	RTFatal("can't raise negative number to non-integer power");
    return (object)NewDouble(pow(a->dbl, b->dbl));
}

object equals(long a, long b)
/* integer a = b */
{
    if (a == b)
	return ATOM_1;
    else
	return ATOM_0;
}

object Dequals(d_ptr a, d_ptr b)
/* double a = b */
{
    if (a->dbl == b->dbl) 
	return ATOM_1;
    else 
	return ATOM_0;
}


object less(long a, long b)
/* integer a < b */
{
    if (a < b)
	return ATOM_1;
    else
	return ATOM_0;
}

object Dless(d_ptr a, d_ptr b)
/* double a < b */
{
    if (a->dbl < b->dbl)
	return ATOM_1;
    else
	return ATOM_0;
}


object greater(long a, long b)
/* integer a > b */
{
    if (a > b)
	return ATOM_1;
    else
	return ATOM_0;
}

object Dgreater(d_ptr a, d_ptr b)
/* double a > b */
{
    if (a->dbl > b->dbl) {
	return ATOM_1;
    }
    else {
	return ATOM_0;
    }
}


object noteq(long a, long b)
/* integer a != b */
{
    if (a != b)
	return ATOM_1;
    else
	return ATOM_0;
}

object Dnoteq(d_ptr a, d_ptr b)
/* double a != b */
{
    if (a->dbl != b->dbl)
	return ATOM_1;
    else
	return ATOM_0;
}


object lesseq(long a, long b)
/* integer a <= b */
{
    if (a <= b) 
	return ATOM_1;
    else
	return ATOM_0;
}

object Dlesseq(d_ptr a, d_ptr b)
/* double a <= b */
{
    if (a->dbl <= b->dbl)
	return ATOM_1;
    else
	return ATOM_0;
}


object greatereq(long a, long b)
/* integer a >= b */
{
    if (a >= b)
	return ATOM_1;
    else
	return ATOM_0;
}

object Dgreatereq(d_ptr a, d_ptr b)
/* double a >= b */
{
    if (a->dbl >= b->dbl)
	return ATOM_1;
    else
	return ATOM_0;
}


object Band(long a, long b)
/* integer a and b */
{
    if (a != 0 && b != 0)
	return ATOM_1;
    else
	return ATOM_0;
}

object Dand(d_ptr a, d_ptr b)
/* double a and b */
{
    if (a->dbl != 0.0 && b->dbl != 0.0)
	return ATOM_1;
    else
	return ATOM_0;
}


object Bor(long a, long b)
/* integer a or b */
{
    if (a != 0 || b != 0)
	return ATOM_1;
    else
	return ATOM_0;
}

object Dor(d_ptr a, d_ptr b)
/* double a or b */
{
    if (a->dbl != 0.0 || b->dbl != 0.0)
	 return ATOM_1;
     else
	 return ATOM_0;
}

object Bxor(long a, long b)
/* integer a xor b */
{
    if ((a != 0) != (b != 0))
	return ATOM_1;
    else
	return ATOM_0;
}

object Dxor(d_ptr a, d_ptr b)
/* double a xor b */
{
    if ((a->dbl != 0.0) != (b->dbl != 0.0))
	 return ATOM_1;
     else
	 return ATOM_0;
}

/* --- Unary Ops --- */

object uminus(long a)
/* integer -a */
{
    if (a == MININT_VAL)
	return (object)NewDouble((double)-MININT_VAL);
    else
	return MAKE_INT(-a);
}

object Duminus(d_ptr a)
/* double -a */
{
    return (object)NewDouble(-a->dbl);
}


object unot(long a)
/* compute c := not a */
{
    if (a == 0)
	return ATOM_1;
    else
	return ATOM_0;
}

object Dnot(d_ptr a)
/* double not a */
{
    if (a->dbl == 0.0)
	return ATOM_1;
    else
	return ATOM_0;
}


object e_sqrt(long a)
/* integer square_root(a) */
{
    if (a < 0)
	RTFatal("attempt to take square root of a negative number");
    return (object)NewDouble( sqrt((double)a) );
}

object De_sqrt(d_ptr a)
/* double square root(a) */
{
    if (a->dbl < 0)
	RTFatal("attempt to take square root of a negative number");
    return (object)NewDouble( sqrt(a->dbl) );
}


object e_sin(long a)
/* sin of an angle a (radians) */
{
    return (object)NewDouble( sin((double)a) );
}

object De_sin(d_ptr a)
/* double sin of a */
{
    return (object)NewDouble( sin(a->dbl) );
}

object e_cos(long a)
/* cos of an angle a (radians) */
{
    return (object)NewDouble( cos((double)a) );
}

object De_cos(d_ptr a)
/* double cos of a */
{
    return (object)NewDouble( cos(a->dbl) );
}

object e_tan(long a)
/* tan of an angle a (radians) */
{
    return (object)NewDouble( tan((double)a) );
}

object De_tan(d_ptr a)
/* double tan of a */
{
    return (object)NewDouble( tan(a->dbl) );
}

object e_arctan(long a)
/* arctan of an angle a (radians) */
{
    return (object)NewDouble( atan((double)a) );
}

object De_arctan(d_ptr a)
/* double arctan of a */
{
    return (object)NewDouble( atan(a->dbl) );
}

object e_log(long a)
/* natural log of a (integer) */
{
    if (a <= 0)
	RTFatal("may only take log of a positive number");
    return (object)NewDouble( log((double)a) );
}

object De_log(d_ptr a)
/* natural log of a (double) */
{
    if (a->dbl <= 0.0)
	RTFatal("may only take log of a positive number");
    return (object)NewDouble( log(a->dbl) );
}

object e_floor(long a)  // not used anymore
/* floor of a number - no op since a is already known to be an int */
{
    return a; 
}

object De_floor(d_ptr a)
/* floor of a number */
{
    double temp;

    temp = floor(a->dbl); 
#ifndef ERUNTIME    
    if (fabs(temp) < MAXINT_DBL)
	return MAKE_INT((long)temp);
    else 
#endif      
	return (object)NewDouble(temp);
}

#define V(a,b) ((((a) << 1) & 0xFFFF0000) | (((b) >> 14) & 0x0000FFFF))

#define prim1 ((long)2147483563L)
#define prim2 ((long)2147483399L)

#define root1 ((long)40014L)
#define root2 ((long)40692L)

#define quo1 ((long)53668L)  /* prim1 / root1 */
#define quo2 ((long)52774L)  /* prim2 / root2 */

#define rem1 ((long)12211L)  /* prim1 % root1 */
#define rem2 ((long)3791L)   /* prim2 % root2 */

// void setran()
// /* set random seed1 and seed2 - neither can be 0 */
// {
//     time_t time_of_day;
//     struct tm *local;
//     int garbage;
//     
// #ifdef EDOS
//     _bios_timeofday(_TIME_GETCLOCK, &seed1);
// #endif
//     time_of_day = time(NULL);
//     local = localtime(&time_of_day);
//     seed2 = local->tm_yday * 86400 + local->tm_hour * 3600 + 
// 	    local->tm_min * 60 +     local->tm_sec;   
// #ifdef EWINDOWS 
//     seed1 = GetTickCount();  // milliseconds since Windows started
// #endif  
//     if (seed1 == 0)
// 	seed1 = 1;    
//     if (seed2 == 0)
// 	seed2 = 1;    
//     good_rand();  // skip first one, second will be more random-looking
// }
// 
// ldiv_t my_ldiv (long int numer, long int denom)
// {
//     ldiv_t result;
// 
//     result.quot = numer / denom;
//     result.rem = numer % denom;
// 
//     if (numer >= 0 && result.rem < 0)   {
// 	++result.quot;
// 	result.rem -= denom;
//     }
// 
//     return result;
// }
// 
// unsigned long good_rand()
// /* Public Domain random number generator from USENET posting */
// {
//     ldiv_t temp;
//     long alpha, beta;
// 
//     if ((seed1 == 0L) || (seed2 == 0L)) {
// 	if (rand_was_set) {
// 	    /* need repeatable sequence of numbers */
// 	    seed1 = 123456;
// 	    seed2 = 9999;
// 	}
// 	else 
// 	    setran();
//     }
//     /* seed = seed * PROOT % PRIME */
//     temp = my_ldiv(seed1, quo1);
//     alpha = root1 * temp.rem;
//     beta = rem1 * temp.quot;
// 
//     /* normalize */
// 
//     if (alpha > beta) 
// 	seed1 = alpha - beta;
//     else
// 	seed1 = alpha - beta + prim1;
// 
//     temp = my_ldiv(seed2, quo2);
//     alpha = root2 * temp.rem;
//     beta = rem2 * temp.quot;
// 
//     if (alpha > beta) 
// 	seed2 = alpha - beta;
//     else
// 	seed2 = alpha - beta + prim2;    
// 
//     return V(seed1, seed2);
// }
// 
// object Random(long a)
// /* random number from 1 to a */
// /* a is a legal integer value */
// {
//     if (a <= 0)
// 	RTFatal("argument to rand must be >= 1");
//     return MAKE_INT((good_rand() % (unsigned)a) + 1);
// }
// 
// 
// object DRandom(d_ptr a)
// /* random number from 1 to a (a <= 1.07 billion) */
// {
//     if (a->dbl < 1.0)
// 	RTFatal("argument to rand must be >= 1");
//     if (a->dbl > MAXINT_DBL)
// 	RTFatal("argument to rand must be <= 1073741823");
// //  return (object)NewDouble( (double)(1 + good_rand() % (unsigned)(a->dbl)) );
//     return (object)(1 + good_rand() % (unsigned)(a->dbl));
// }


// object unary_op(int fn, object a)
// /* recursive evaluation of a unary op 
//    c may be the same as a. ATOM_INT case handled in-line by caller */
// {
//     long length;
//     object_ptr ap, cp;
//     object x;
//     s1_ptr c;
//     object (*int_fn)();
// 
//     if (IS_ATOM_DBL(a))
// 	return (*optable[fn].dblfn)(DBL_PTR(a));
// 
//     else {
// 	/* a must be a SEQUENCE */
// 	a = (object)SEQ_PTR(a);
// 	length = ((s1_ptr)a)->length;
// 	c = NewS1(length);
// 	cp = c->base;
// 	ap = ((s1_ptr)a)->base;
// 	int_fn = optable[fn].intfn;
// 	while (TRUE) {
// 	    x = *(++ap);
// 	    if (IS_ATOM_INT(x)) {
// 		*(++cp) = (*int_fn)(INT_VAL(x));
// 	    }
// 	    else {
// 		if (x == NOVALUE)
// 		    break;
// 		*(++cp) = unary_op(fn, x);
// 	    }
// 	}
// 	return MAKE_SEQ(c);
//     }
// }
// 
// 
// object binary_op_a(int fn, object a, object b)
// /* perform binary op on two atoms */
// {
//     struct d temp_d;
// 
//     if (IS_ATOM_INT(a)) { 
// 	if (IS_ATOM_INT(b)) 
// 	    return (*optable[fn].intfn)(INT_VAL(a), INT_VAL(b));
// 	else {
// 	    temp_d.dbl = (double)INT_VAL(a);
// 	    return (*optable[fn].dblfn)(&temp_d, DBL_PTR(b));
// 	}
//     }
//     else {
// 	if (IS_ATOM_INT(b)) {
// 	    temp_d.dbl = (double)INT_VAL(b);
// 	    return (*optable[fn].dblfn)(DBL_PTR(a), &temp_d);
// 	}
// 	else
// 	    return (*optable[fn].dblfn)(DBL_PTR(a), DBL_PTR(b));
//     }
// }
// 
// 
// object binary_op(int fn, object a, object b)
// /* Recursively calculates fn of a and b. */  
// /* Caller must handle INT:INT case */
// {
//     long length;
//     object_ptr ap, bp, cp;
//     struct d temp_d;
//     s1_ptr c;
//     object (*int_fn)();
//     object x;
//     
//     /* handle all ATOM:ATOM cases except INT:INT - not allowed 
//        n.b. IS_ATOM_DBL actually only distinguishes ATOMS from SEQUENCES */
//     if (IS_ATOM_INT(a) && IS_ATOM_DBL(b)) { 
// 	/* in test above b can't be an int if a is */
// 	temp_d.dbl = (double)INT_VAL(a);
// 	return (*optable[fn].dblfn)(&temp_d, DBL_PTR(b));
//     } 
//     else if (IS_ATOM_DBL(a)) { 
// 	/* a could be an int, but then b must be a sequence */
// 	if (IS_ATOM_INT(b)) {
// 	    temp_d.dbl = (double)INT_VAL(b);
// 	    return (*optable[fn].dblfn)(DBL_PTR(a), &temp_d);
// 	}
// 	else if (IS_ATOM_DBL(b))  {
// 	    return (*optable[fn].dblfn)(DBL_PTR(a), DBL_PTR(b));
// 	}
//     }
//     
//     /* result is a sequence */
//     int_fn = optable[fn].intfn;
//     if (IS_ATOM(a)) {
// 	/* b must be a sequence */
// 	b = (object)SEQ_PTR(b);
// 	length = ((s1_ptr)b)->length;
// 	c = NewS1(length);
// 	cp = c->base;
// 	bp = ((s1_ptr)b)->base;
// 	if (IS_ATOM_INT(a)) {
// 	    while (TRUE) {
// 		x = *(++bp);
// 		if (IS_ATOM_INT(x)) {
// 		    *(++cp) = (*int_fn)(INT_VAL(a), INT_VAL(x));
// 		}
// 		else {
// 		    if (x == NOVALUE)
// 			break;
// 		    *(++cp) = binary_op(fn, a, x);
// 		}
// 	    }
// 	}
// 	else {
// 	    // a is not an integer
// 	    while (--length >= 0) {
// 		*(++cp) = binary_op(fn, a, *(++bp));
// 	    }
// 	}
//     }
//     else if (IS_ATOM(b)) {
// 	/* a must be a sequence */
// 	a = (object)SEQ_PTR(a);
// 	length = ((s1_ptr)a)->length;
// 	c = NewS1(length);
// 	cp = c->base;
// 	ap = ((s1_ptr)a)->base;
// 	if (IS_ATOM_INT(b)) {
// 	    while (TRUE) { 
// 		x = *(++ap);
// 		if (IS_ATOM_INT(x)) {
// 		    *(++cp) = (*int_fn)(INT_VAL(x), INT_VAL(b));
// 		}
// 		else {
// 		    if (x == NOVALUE)
// 			break;
// 		    *(++cp) = binary_op(fn, x, b);
// 		}
// 	    }
// 	}
// 	else {
// 	    // b is not an integer
// 	    while (--length >= 0) { 
// 		*(++cp) = binary_op(fn, *(++ap), b);
// 	    }
// 	}
//     }
//     else {
// 	/* a and b must both be sequences */
// 	a = (object)SEQ_PTR(a);
// 	b = (object)SEQ_PTR(b);
// 	length = ((s1_ptr)a)->length;
// 	if (length != ((s1_ptr)b)->length) {
// 	    sprintf(TempBuff, 
// 		"sequence lengths are not the same (%ld != %ld)",
// 		length, ((s1_ptr)b)->length);
// 	    RTFatal(TempBuff);
// 	}
// 	c = NewS1(length);
// 	cp = c->base;
// 	ap = ((s1_ptr)a)->base;
// 	bp = ((s1_ptr)b)->base+1;
// 	while (TRUE) { 
// 	    x = *(++ap);
// 	    if (IS_ATOM_INT(x) && IS_ATOM_INT(*bp)) {
// 		*(++cp) = (*int_fn)(INT_VAL(x), INT_VAL(*bp++));
// 	    }
// 	    else {
// 		if (x == NOVALUE)
// 		    break;
// 		*(++cp) = binary_op(fn, x, *bp++);
// 	    }
// 	}
//     }
//     return MAKE_SEQ(c);
// }


int compare(object a, object b)
/* Compare general objects a and b. Return 0 if they are identical,
   1 if a > b, -1 if a < b. All atoms are less than all sequences.
   The INT-INT case *must* be taken care of by the caller */
{
    object_ptr ap, bp;
    object av, bv;
    long length, lengtha, lengthb;
    double da, db;
    int c;

    if (IS_ATOM(a)) {
	if (!IS_ATOM(b))
	    return -1;
	if (IS_ATOM_INT(a)) {
	    /* b *must* be a double */
	    da = (double)a; 
	    db = DBL_PTR(b)->dbl;
	}
	else {
	    da = DBL_PTR(a)->dbl;
	    if (IS_ATOM_INT(b)) 
		db = (double)b;
	    else
		db = DBL_PTR(b)->dbl;
	}
	return (da < db) ? -1: (da == db) ? 0: 1;
    }

    else {
	/* a must be a SEQUENCE */
	if (!IS_SEQUENCE(b))
	    return 1;
	a = (object)SEQ_PTR(a);
	b = (object)SEQ_PTR(b);
	ap = ((s1_ptr)a)->base;
	bp = ((s1_ptr)b)->base;
	lengtha = ((s1_ptr)a)->length;
	lengthb = ((s1_ptr)b)->length;
	if (lengtha < lengthb)
	    length = lengtha;
	else
	    length = lengthb;
	while (--length >= 0) {
	    ap++;
	    bp++;
	    av = *ap;
	    bv = *bp;
	    if (av != bv) {
		if (IS_ATOM_INT(av) && IS_ATOM_INT(bv)) {
		    if (av < bv)
			return -1;
		    else 
			return 1;
		}
		else { 
		    c = compare(av, bv);
		    if (c != 0)
			return c;
		}
	    }
	}
	return (lengtha < lengthb) ? -1: (lengtha == lengthb) ? 0: 1;
    }
}


long find(object a, s1_ptr b)
/* find object a as an element of sequence b */
{
    long length;
    object_ptr bp;
    object bv;

    if (!IS_SEQUENCE(b))
	RTFatal("second argument of find() must be a sequence");

    b = SEQ_PTR(b);
    bp = b->base;
    
    if (IS_ATOM_INT(a)) {
	while (TRUE) {
	    bv = *(++bp);
	    if (IS_ATOM_INT(bv)) {
		if (a == bv) 
		    return bp - (object_ptr)b->base;
	    }
	    else if (bv == NOVALUE) {
		break;  // we hit the end marker
	    }
	    else if (compare(a, bv) == 0) {  /* not INT-INT case */
		return bp - (object_ptr)b->base;
	    }
	}
    }
    
    else if (IS_SEQUENCE(a)) {
	long a_len;
	
	length = b->length;
	a_len = SEQ_PTR(a)->length;
	while (length > 0) {
	    bv = *(++bp);
	    if (IS_SEQUENCE(bv)) {
		if (a_len == SEQ_PTR(bv)->length) {
		    /* a is SEQUENCE => not INT-INT case */
		    if (compare(a, bv) == 0)
			return bp - (object_ptr)b->base;
		}
	    }
	    length--;
	}
    }
    
    else {
	length = b->length;
	while (length > 0) {
	    /* a is ATOM double => not INT-INT case */
	    if (compare(a, *(++bp)) == 0)
		return bp - (object_ptr)b->base;
	    length--;
	}
    }
    
    return 0; 
}


long e_match(s1_ptr a, s1_ptr b)
/* find sequence a as a slice within sequence b 
   sequence a may not be empty */
{
    long ntries, len_remaining;
    object_ptr a1, b1, bp;
    object_ptr ai, bi;
    object av, bv;
    long lengtha, lengthb;

    if (!IS_SEQUENCE(a))
	RTFatal("first argument of match() must be a sequence");
    
    if (!IS_SEQUENCE(b))
	RTFatal("second argument of match() must be a sequence");
    
    a = SEQ_PTR(a);
    b = SEQ_PTR(b);
    
    lengtha = a->length;
    if (lengtha == 0)
	RTFatal("first argument of match() must be a non-empty sequence");
    
    lengthb = b->length;
    b1 = b->base;
    bp = b1;
    a1 = a->base;
    ntries = lengthb - lengtha + 1;
    
    while (--ntries >= 0) {
	ai = a1;
	bi = bp;
	len_remaining = lengtha;
	
	do {
	    ai++;
	    bi++;
	    av = *ai;
	    bv = *bi;
	    if (av != bv) {
		if (IS_ATOM_INT(av) && IS_ATOM_INT(bv)) {
		    bp++;
		    break;
		}
		else if (compare(av, bv) != 0) {
		    bp++;
		    break;
		}
	    }
	    if (--len_remaining == 0)
		return bp - b1 + 1; /* perfect match */
	} while (TRUE);
    }
    
    return 0; /* couldn't match */
}

#ifndef ERUNTIME
void CheckSlice(s1_ptr a, long startval, long endval, long length)
/* check legality of a slice, return integer values of start, length */
/* startval and endval are deref'd */
{
    long n;

    if (IS_ATOM(a))
	RTFatal("attempt to slice an atom");

    if (startval < 1) {
	sprintf(TempBuff, "slice lower index is less than 1 (%ld)", startval);
	RTFatal(TempBuff);
    }
    if (endval < 0) {
	sprintf(TempBuff, "slice upper index is less than 0 (%ld)", endval);
	RTFatal(TempBuff);
    }

    if (length < 0 ) {
	sprintf(TempBuff, "slice length is less than 0 (%ld)", length);
	RTFatal(TempBuff);
    }

    a = SEQ_PTR(a);
    n = a->length;
    if (startval > n + 1 || length > 0 && startval > n) {
	sprintf(TempBuff, "slice starts past end of sequence (%ld > %ld)", 
		startval, n);
	RTFatal(TempBuff);
    }
    if (endval > n) {
	sprintf(TempBuff, "slice ends past end of sequence (%ld > %ld)",
		endval, n);
	RTFatal(TempBuff);
    } 
}
#endif

void RHS_Slice(s1_ptr a, object start, object end)
/* Construct slice a[start..end] */ 
{
    long startval;
    long length;
    long endval;
    s1_ptr newa, olda;
    object temp;
    object_ptr p, q, sentinel;
    object save;
    
    if (IS_ATOM_INT(start))
	startval = INT_VAL(start);
    else if (IS_ATOM_DBL(start)) {
	startval = (long)(DBL_PTR(start)->dbl);
    }
    else
	RTFatal("slice lower index is not an atom");

    if (IS_ATOM_INT(end))
	endval = INT_VAL(end);
    else if (IS_ATOM_DBL(end)) {
	endval = (long)(DBL_PTR(end)->dbl);
	 /* f.p.: if the double is too big for
	    a long WATCOM produces the most negative number. This
	    will be caught as a bad subscript, although the value in the
	    diagnostic will be wrong */
    }
    else
	RTFatal("slice upper index is not an atom");
    length = endval - startval + 1;

#ifndef ERUNTIME 
    CheckSlice(a, startval, endval, length);
#endif
    
    olda = SEQ_PTR(a);
    if (*rhs_slice_target == (object)a && 
	olda->ref == 1 &&
	(olda->base + olda->length - (object_ptr)olda) < 8 * (length+1)) {  
				   // we must limit the wasted space
	/* do it in-place */       // or we could even run out of memory
	object_ptr p;
	
	/* update the sequence descriptor */
	p = olda->base+1;
	olda->base = olda->base + startval - 1;
	
	/* deref the lower excluded elements */
	for (; p <= olda->base; p++)
	    DeRef(*p);
	
	/* deref the upper excluded elements */
	for (p = olda->base + 1 + length;
	     p <= olda->base + 1 + olda->length - startval;
	     p++) 
	    DeRef(*p);
	
	olda->postfill += olda->length - endval;
	olda->length = length;  
	*(olda->base + length + 1) = NOVALUE; // new end marker
    }
    else {
	/* allocate a new sequence */
	newa = NewS1(length);
	p = newa->base;
	q = olda->base + startval;
	
	// plant a sentinel
	sentinel = q + length;
	save = *(sentinel);
	*(sentinel) = NOVALUE;
	
	while (TRUE) {
	    temp = *q++;
	    *(++p) = temp;
	    if (!IS_ATOM_INT(temp)) {
		if (temp == NOVALUE)
		    break;
		RefDS(temp);
	    }
	}
	
	*(sentinel) = save;
	
	DeRef(*rhs_slice_target);
	*rhs_slice_target = MAKE_SEQ(newa);
    }
}


void AssignSlice(object start, object end, s1_ptr val)
/* assign to a sliced variable */
{
    s1_ptr *seq_ptr;
    s1_ptr sp;
    long startval, endval, length;
    object_ptr s_elem;
    object_ptr v_elem;

    seq_ptr = assign_slice_seq; /* "4th" arg */   

    if (IS_ATOM_INT(start))
	startval = INT_VAL(start);
    else if (IS_ATOM_DBL(start)) {
	startval = (long)(DBL_PTR(start)->dbl);
    }
    else
	RTFatal("slice lower index is not an atom");

    if (IS_ATOM_INT(end))
	endval = INT_VAL(end);
    else if (IS_ATOM_DBL(end)) {
	endval = (long)(DBL_PTR(end)->dbl); /* see above comments on f.p. */
    }
    else
	RTFatal("slice upper index is not an atom");

    length = endval - startval + 1;

#ifndef ERUNTIME
    CheckSlice(*seq_ptr, startval, endval, length);
#endif

    sp = SEQ_PTR(*seq_ptr);
    if (!UNIQUE(sp)) {
	sp = (s1_ptr)SequenceCopy(sp);
	*seq_ptr = (s1_ptr)MAKE_SEQ(sp);
    }
    s_elem = sp->base + startval; 

    if (IS_ATOM(val)) {
	if (!IS_ATOM_INT(val))   
	    (DBL_PTR(val)->ref) += length;
	
	while (--length >= 0) {
	    DeRef(*s_elem);
	    *s_elem++ = (object)val; 
	}
    }
    else {
	val = SEQ_PTR(val);
	v_elem = val->base+1;
	if (val->length != length) {
	    sprintf(TempBuff, 
	    "lengths do not match on assignment to slice (%ld != %ld)",
	    length, val->length);
	    RTFatal(TempBuff);
	}
	while (TRUE) {
	    if (!IS_ATOM_INT(*v_elem)) {
		if (*v_elem == NOVALUE)
		    break;
		RefDS(*v_elem);
	    }
	    DeRef(*s_elem);
	    *s_elem++ = *v_elem++; 
	}
    }
}

// object Date()
// /* returns year, month, day, hour, minute, second */
// {
//     object_ptr obj_ptr;
//     s1_ptr result;
//     time_t time_of_day;
//     struct tm *local;
// 
//     time_of_day = time(NULL);
//     local = localtime(&time_of_day);
//     result = NewS1(8);
//     obj_ptr = result->base;
//     obj_ptr[1] = MAKE_INT(local->tm_year);  
//     obj_ptr[2] = MAKE_INT(local->tm_mon+1);   
//     obj_ptr[3] = MAKE_INT(local->tm_mday);   
//     obj_ptr[4] = MAKE_INT(local->tm_hour);   
//     obj_ptr[5] = MAKE_INT(local->tm_min);   
//     obj_ptr[6] = MAKE_INT(local->tm_sec);   
//     obj_ptr[7] = MAKE_INT(local->tm_wday+1);   
//     obj_ptr[8] = MAKE_INT(local->tm_yday+1);   
//     return MAKE_SEQ(result);
// }

void MakeCString(char *s, object obj)
/* make an atom or sequence into a C string */
/* N.B. caller must allow one extra for the null terminator */
{
    object_ptr elem;
    object x;

    if (IS_ATOM(obj)) 
	*s++ = Char(obj);
    else {
	obj = (object)SEQ_PTR(obj);
	elem = ((s1_ptr)obj)->base;
	while (TRUE) { 
	    x = *(++elem);
	    if (IS_ATOM_INT(x)) {
		*s++ = (char)x;
	    }
	    else {
		if (x == NOVALUE)
		    break;
		*s++ = doChar(x);
	    }
	}
    }
    *s = '\0';
}

object make_atom32(unsigned c32)
/* make a Euphoria atom from an unsigned C value */
{
    if (c32 <= (unsigned)MAXINT)
	return c32;
    else
	return NewDouble((double)c32);
}

//here


long find_from(object a, s1_ptr b, object c)
/* find object a as an element of sequence b starting from c*/
{
    long length;
    object_ptr bp;
    object bv;

    if (!IS_SEQUENCE(b))
	RTFatal("second argument of find_from() must be a sequence");

    b = SEQ_PTR(b);
    length = b->length;

    // same rules as the lower limit on a slice
    if (IS_ATOM_INT(c)) {
	;
    }
    else if (IS_ATOM_DBL(c)) {
	c = (long)(DBL_PTR(c)->dbl);
    }
    else
	RTFatal("third argument of find_from() must be an atom");
		
    // we allow c to be $+1, just as we allow the lower limit
    // of a slice to be $+1, i.e. the empty sequence
    if (c < 1 || c > length+1) {
	sprintf(TempBuff, "third argument of find_from() is out of bounds (%ld)", c);
	RTFatal(TempBuff);
    }
		
    bp = b->base;
    bp += c - 1;
    if (IS_ATOM_INT(a)) {
	while (TRUE) {
	    bv = *(++bp);
	    if (IS_ATOM_INT(bv)) {
		if (a == bv) 
		    return bp - (object_ptr)b->base;
	    }
	    else if (bv == NOVALUE) {
		break; // we hit the end marker
	    }
	    else if (compare(a, bv) == 0) {  /* not INT-INT case */
		return bp - (object_ptr)b->base;
	    }
	}
    }
    
    else if (IS_SEQUENCE(a)) {
	long a_len;
		
	length -= c - 1;
	a_len = SEQ_PTR(a)->length;
	while (length > 0) {
	    bv = *(++bp);
	    if (IS_SEQUENCE(bv)) {
		if (a_len == SEQ_PTR(bv)->length) {
		    /* a is SEQUENCE => not INT-INT case */
		    if (compare(a, bv) == 0)
			return bp - (object_ptr)b->base;
		}
	    }
	    length--;
	}
    }
    
    else {
	length -= c - 1;
	while (length > 0) {
	    /* a is ATOM double => not INT-INT case */
	    if (compare(a, *(++bp)) == 0)
		return bp - (object_ptr)b->base;
	    length--;
	}
    }
    
    return 0; 
}

long e_match_from(s1_ptr a, s1_ptr b, object c)
/* find sequence a as a slice within sequence b 
   sequence a may not be empty */
{
    long ntries, len_remaining;
    object_ptr a1, b1, bp;
    object_ptr ai, bi;
    object av, bv;
    long lengtha, lengthb;

    if (!IS_SEQUENCE(a))
	RTFatal("first argument of match_from() must be a sequence");
    
    if (!IS_SEQUENCE(b))
	RTFatal("second argument of match_from() must be a sequence");
    
    a = SEQ_PTR(a);
    b = SEQ_PTR(b);
    
    lengtha = a->length;
    if (lengtha == 0)
	RTFatal("first argument of match_from() must be a non-empty sequence");
    
    // same rules as the lower limit on a slice
    if (IS_ATOM_INT(c)) {
	;
    }
    else if (IS_ATOM_DBL(c)) {
	c = (long)(DBL_PTR(c)->dbl);
    }
    else
	RTFatal("third argument of match_from() must be an atom");
		
    lengthb = b->length;

    // we allow c to be $+1, just as we allow the lower limit
    // of a slice to be $+1, i.e. the empty sequence
    if (c < 1 || c > lengthb+1) {
	sprintf(TempBuff, "third argument of match_from() is out of bounds (%ld)", c);
	RTFatal(TempBuff);
    }

    b1 = b->base;
    bp = b1 + c - 1;
    a1 = a->base;
    ntries = lengthb - lengtha - c + 2; // will be max 0, when c is lengthb+1
    while (--ntries >= 0) {
	ai = a1;
	bi = bp;

	len_remaining = lengtha;
	do {
	    ai++;
	    bi++;
	    av = *ai;
	    bv = *bi;
	    if (av != bv) {
		if (IS_ATOM_INT(av) && IS_ATOM_INT(bv)) {
		    bp++;
		    break;
		}
		else if (compare(av, bv) != 0) {
		    bp++;
		    break;
		}
	    }
	    if (--len_remaining == 0)
		return(bp - b1 + 1); /* perfect match */
	} while (TRUE);
    }
    return 0; /* couldn't match */
}

//here

