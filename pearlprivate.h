
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
