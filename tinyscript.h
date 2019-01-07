/* Tinyscript interpreter

   Copyright 2016 Total Spectrum Software Inc.

   +--------------------------------------------------------------------
   ¦  TERMS OF USE: MIT License
   +--------------------------------------------------------------------
   Permission is hereby granted, free of charge, to any person obtaining
   a copy of this software and associated documentation files
   (the "Software"), to deal in the Software without restriction,
   including without limitation the rights to use, copy, modify, merge,
   publish, distribute, sublicense, and/or sell copies of the Software,
   and to permit persons to whom the Software is furnished to do so,
   subject to the following conditions:

   The above copyright notice and this permission notice shall be
   included in all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
   IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
   CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
   TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
   SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
   +-----------------------------
  */
#ifndef TINYSCRIPT_H
#define TINYSCRIPT_H

#include <stdint.h>

#ifdef __propeller__
// define SMALL_PTRS to use 16 bits for pointers
// useful for machines with <= 64KB of RAM
#define SMALL_PTRS
#endif


#ifdef __cplusplus
extern "C"{
#endif
// errors
// all the ParseXXX functions return 0 on success, a negative
// error code otherwise
enum {
    TS_ERR_OK = 0,
    TS_ERR_NOMEM = -1,
    TS_ERR_SYNTAX = -2,
    TS_ERR_UNKNOWN_SYM = -3,
    TS_ERR_BADARGS = -4,
    TS_ERR_TOOMANYARGS = -5,
    TS_ERR_OK_ELSE = 1, // special internal condition
};

// we use this a lot
typedef char Byte;

//our target is machines with < 64KB of memory, so 16 bit pointers
//will do
typedef Byte *Ptr;

// strings are represented as (length,ptr) pairs
// this is done so that we can re-use variable names and similar
// substrings directly from the script text, without having
// to insert 0 into them
typedef struct {
#ifdef SMALL_PTRS
    uint16_t len_;
    uint16_t ptr_;
#else
    unsigned len_;
    const char *ptr_;
#endif
} TS_String;

// val has to be able to hold a pointer
typedef intptr_t Val;

static inline unsigned StringGetLen(TS_String s) { return (unsigned)s.len_; }
static inline const char *StringGetPtr(TS_String s) { return (const char *)(intptr_t)s.ptr_; }
#ifdef SMALL_PTRS
static inline void StringSetLen(TS_String *s, unsigned len) { s->len_ = (uint16_t)len; }
static inline void StringSetPtr(TS_String *s, const char *ptr) { s->ptr_ = (uint16_t)(intptr_t)ptr; }
#else
static inline void StringSetLen(TS_String *s, unsigned len) { s->len_ = len; }
static inline void StringSetPtr(TS_String *s, const char *ptr) { s->ptr_ = ptr; }
#endif

// symbols can take the following forms:
#define INT      0x0  // integer
#define STRING   0x1  // string
#define OPERATOR 0x2  // operator; precedence in high 8 bits
#define ARG      0x3  // argument; value is offset on stack
#define BUILTIN  'B'  // builtin: number of operands in high 8 bits
#define USRFUNC  'f'  // user defined a procedure; number of operands in high 8 bits
#define TOK_BINOP 'o'

#define BINOP(x) (((x)<<8)+TOK_BINOP)
#define CFUNC(x) (((x)<<8)+BUILTIN)

typedef struct symbol {
    TS_String name;
    int    type;   // symbol type
    Val    value;  // symbol value, or string ptr
} Sym;

#define MAX_BUILTIN_PARAMS 4

typedef Val (*Cfunc)(Val, Val, Val, Val);
typedef Val (*Opfunc)(Val, Val);

// structure to describe a user function
typedef struct ufunc {
    TS_String body; // pointer to the body of the function
    int nargs;   // number of args
    // names of arguments
    TS_String argName[MAX_BUILTIN_PARAMS];
} UserFunc;

//
// global interface
//
int TinyScript_Init(void *mem, int mem_size);
int TinyScript_Define(const char *name, int toktype, Val value);
int TinyScript_Run(const char *s, int saveStrings, int topLevel);

// provided by our caller
extern int inchar(void);
extern void outchar(int c);


#ifdef __cplusplus
}
class _TinyScript
{
  public:
static void pollREPL();

//Make it easy by hardcoding the possible func sigs.
static void addFunc(const char *name, Val (*Cfunc)(Val,Val,Val,Val) );
static void addFunc(const char *name, Val (*Cfunc)(Val, Val, Val));
static void addFunc(const char *name, Val (*Cfunc)(Val, Val));
static void addFunc(const char *name, Val (*Cfunc)(Val));
static void addFunc(const char *name, Val (*Cfunc)());

static void begin(int size);
};
extern class _TinyScript TinyScript;

#endif


#endif
