
#ifndef BLACKPEARL_PRIVATE_H
#define BLACKPEARL_PRIVATE_H

#include <stdio.h>
#include "pearl.h"

#define CAR(obj) (obj->bptuple.car)
#define CDR(obj) (obj->bptuple.cdr)
#define CAAR(obj) (obj->bptuple.car->bptuple.car)
#define CADR(obj) (obj->bptuple.car->bptuple.cdr)
#define CDAR(obj) (obj->bptuple.cdr->bptuple.car)

#define GC_MARK 0x01
#define GC_ENV 0x02

object_t * mksym(char * val);

void writeobj(object_t * object);
void writeitm(object_t * object);

void evals(object_t * to_eval, env_t * env);

void bpanic(const char * fmt, ...);

int nextint(char * tok, int * value);
int nextfloat(char * tok, double * value);
int nextfrac(char * tok, int * num, unsigned * denom);
unsigned hash(char * bpstr);
bool isatom(object_t * object);

object_t *prplus(object_t *params, env_t *env);
object_t *prgt(object_t *params, env_t *env);
object_t *prminus(object_t *params, env_t *env);
object_t *prequal(object_t *params, env_t *env);
object_t *prmul(object_t *params, env_t *env);

#endif //BLACKPEARL_PRIVATE_H
