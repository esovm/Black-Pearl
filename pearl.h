
#ifndef BLACKPEARL_H
#define BLACKPEARL_H

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>

#define CAR(obj) (obj->bptuple.car)
#define CDR(obj) (obj->bptuple.cdr)
#define CAAR(obj) (obj->bptuple.car->bptuple.car)
#define CADR(obj) (obj->bptuple.car->bptuple.cdr)
#define CDAR(obj) (obj->bptuple.cdr->bptuple.car)

#define NAME "Black Pearl v 1.0, 29-9-18"
#define HTABLE_SIZE 10

#define MAX_MACRO_DEPTH 100
#define MAX_TOKEN_LENGTH 1000
#define MAX_STACK_SIZE 1000

#define MAX_HEAP 4096

#define GC_MARK 0x01
#define GC_ENV 0x02

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

object_t * mksym(char * val);
object_t * parsef(list_t ** tokens_pointer);

void writeobj(object_t * object);
void writeitm(object_t * object);

object_t * eval(object_t * to_eval, env_t * env);
void evals(object_t * to_eval, env_t * env);

object_t * parse(list_t ** tokens_pointer);

struct phtab_t * entryget(char * identifier, env_t * env);
object_t * objectget(char * identifier, env_t * env);
void objectdef(char * identifier, object_t * obj, env_t * env);
void objectset(char * identifier, object_t * obj, env_t * env);

env_t * envcreat(env_t * parent);
void envfree(env_t * env);

void bpanic(const char * fmt, ...);

list_t * appendl(list_t * l1, list_t * l2);

int nextint(char * tok, int * value);
int nextfloat(char * tok, double * value);
int nextfrac(char * tok, int * num, unsigned * denom);
unsigned hash(char * bpstr);
bool isatom(object_t * object);

extern object_t * nil;
extern object_t * t;
extern object_t * f;
extern env_t * genv;

object_t *prplus(object_t *params, env_t *env);
object_t *prgt(object_t *params, env_t *env);
object_t *prminus(object_t *params, env_t *env);
object_t *prequal(object_t *params, env_t *env);
object_t *prmul(object_t *params, env_t *env);

#endif //BLACKPEARL
