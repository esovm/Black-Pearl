
/* Black Pearl
 * Copyright (C) Kamila Palaiologos Szewczyk, 2019.
 * License: MIT
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the
 * Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef BLACKPEARL_H
#define BLACKPEARL_H

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>

#define NAME "Black Pearl v 1.0, 29-9-18"

#define MAX_MACRO_DEPTH 100
#define MAX_TOKEN_LENGTH 1000
#define MAX_STACK_SIZE 1000
#define MAX_HEAP 4096
#define HTABLE_SIZE 10

typedef struct bplist {
    struct bplist * next;
    void * item;
} list_t;

struct phtab_t {
    struct phtab_t * next;
    char * key;
    struct bpobject * refer;
};

typedef struct bpenv {
    struct bpenv * parent;
    struct phtab_t * bindings[HTABLE_SIZE];
} env_t;

enum token_t {
    TT_INT, TT_FLT, TT_FRAC, TT_CHR, TT_VECTOR, TT_STR, TT_PORT,
    TT_PAIR, TT_BOOL, TT_PROC, TT_PRIM, TT_NIL, TT_SYM
};

typedef struct bpobject {
    enum token_t type;
    union {
        struct {
            int value;
        } bpint;
        struct {
            double value;
        } bpflt;
        struct {
            unsigned denominator;
            int numerator;
        } bpfrac;
        struct {
            int value;
        } bpchr;
        struct {
            int length;
            struct bpobject ** value;
        } bpvec;
        struct {
            int length;
            char * value;
        } bpstr;
        struct {
            int flags;
            void * accessed;
        } bpprt;
        struct {
            char value;
        } bpbool;
        struct {
            struct bpobject * car;
            struct bpobject * cdr;
        } bptuple;
        struct {
            struct bpobject * arguments;
            struct bpobject * body;
            env_t * env;
        } bpfn;
        struct {
            struct bpobject * (*f)(struct bpobject * args, env_t * env);
        } bprim;
    };
} object_t;

void init();

list_t * tokenize(char * string);
object_t * parsef(list_t ** tokens_pointer);

object_t * eval(object_t * to_eval, env_t * env);
object_t * parse(list_t ** tokens_pointer);

list_t * appendl(list_t * l1, list_t * l2);

struct phtab_t * entryget(char * identifier, env_t * env);
object_t * objectget(char * identifier, env_t * env);
void objectdef(char * identifier, object_t * obj, env_t * env);
void objectset(char * identifier, object_t * obj, env_t * env);

extern object_t * nil;
extern object_t * t;
extern object_t * f;
extern env_t * genv;

env_t * envcreat(env_t * parent);
void envfree(env_t * env);

#endif //BLACKPEARL
