
#include "pearl.h"

env_t * syntaxes;
env_t * genv;
object_t * nil;
object_t * f;
object_t * t;

object_t * prplus(object_t * params, env_t * env) {
    object_t * ret = malloc(sizeof(object_t));
    ret->type = TT_INT;
    if (params->type == TT_NIL)
        ret->bpint.value = 0;
    else if (params->type == TT_PAIR && params->bptuple.car->type == TT_INT)
        ret->bpint.value = params->bptuple.car->bpint.value +
                             prplus(params->bptuple.cdr, env)->bpint.value;
    else
        bpanic("Quite the illegal addition there");
    return ret;
}

object_t * prminus(object_t * params, env_t * env) {
    object_t * ret = malloc(sizeof(object_t));
    ret->type = TT_INT;
    if (params->type == TT_PAIR && params->bptuple.car->type == TT_INT &&
        params->bptuple.cdr->type == TT_PAIR &&
        params->bptuple.cdr->bptuple.car->type == TT_INT &&
        params->bptuple.cdr->bptuple.cdr->type == TT_NIL)
        ret->bpint.value = params->bptuple.car->bpint.value -
                             params->bptuple.cdr->bptuple.car->bpint.value;
    else
        bpanic("Quite the illegal subtraction there");
    return ret;
}

object_t * prgt(object_t * params, env_t * env) {
    object_t * ret = malloc(sizeof(object_t));
    ret->type = TT_BOOL;
    if (params->type == TT_PAIR && params->bptuple.car->type == TT_INT &&
        params->bptuple.cdr->type == TT_PAIR &&
        params->bptuple.cdr->bptuple.car->type == TT_INT &&
        params->bptuple.cdr->bptuple.cdr->type == TT_NIL)
        ret->bpbool.value = params->bptuple.car->bpint.value >
                             params->bptuple.cdr->bptuple.car->bpint.value;
    else
        bpanic("Illegal comparison");
    return ret;
}

object_t * prequal(object_t * params, env_t * env) {
    object_t * ret = malloc(sizeof(object_t));
    ret->type = TT_BOOL;
    if (params->type == TT_PAIR && params->bptuple.car->type == TT_INT &&
        params->bptuple.cdr->type == TT_PAIR &&
        params->bptuple.cdr->bptuple.car->type == TT_INT &&
        params->bptuple.cdr->bptuple.cdr->type == TT_NIL)
        ret->bpbool.value = params->bptuple.car->bpint.value ==
                             params->bptuple.cdr->bptuple.car->bpint.value;
    else
        bpanic("Illegal comparison");
    return ret;
}


object_t * prmul(object_t * params, env_t * env) {
    object_t * ret = malloc(sizeof(object_t));
    ret->type = TT_INT;
    if (params->type == TT_NIL)
        ret->bpint.value = 1;
    else if (params->type == TT_PAIR && params->bptuple.car->type == TT_INT)
        ret->bpint.value = params->bptuple.car->bpint.value *
                             prplus(params->bptuple.cdr, env)->bpint.value;
    else
        bpanic("Quite the illegal addition there");
    return ret;
}

void init_syntax() {
    syntaxes = envcreat(NULL);
}

void define_syntax(char * syntax_name, object_t * syntax_rules) {
    objectdef(syntax_name, syntax_rules, syntaxes);
}

int is_syntax(char * symbol_name) {
    return entryget(symbol_name, syntaxes) != NULL;
}

object_t * expand_syntax_rule(object_t * expr) {}

object_t * expand_syntax_max(object_t * expr, int depth) {
    if (depth > MAX_MACRO_DEPTH)
        bpanic("Max macro depth reached");
    if (isatom(expr))
        return expr;
    if (expr->bptuple.car->type == TT_SYM &&
        is_syntax(expr->bptuple.car->bpstr.value)) {
        object_t * iter = expand_syntax_rule(expr);
        return expand_syntax_max(expr, depth+1);
    } else
        return expr;
}

object_t * expand_syntax(object_t * expr) {
    return expand_syntax_max(expr, 0);
}

int one_char_sym(int c) {
    return c == '(' || c == ')' || c == '\'';
}

int part_of_symbol(int c) {
    return c && !(isspace(c) || c == '(' || c == ')');
}

char * next_tok_from(char ** bpstring_pointer) {
    char * token = malloc(sizeof(char) * MAX_TOKEN_LENGTH);
    char * bpstring = *bpstring_pointer;
    if (isspace(*bpstring))
        while (isspace(*(++bpstring)));
    if (*bpstring == '"') {
        char * matching = strchr(bpstring + 1, '"');
        if (matching == NULL)
            bpanic("Unmatched \"\n");
        else if (matching - bpstring > MAX_TOKEN_LENGTH - 2)
            bpanic("String constant over %d length\n",
                        MAX_TOKEN_LENGTH - 2);
        strncpy(token, bpstring, matching - bpstring + 1);
        token[matching - bpstring + 1] = 0;
        bpstring = matching + 1;
    } else if (*bpstring == ';') {
        char * end = strchr(bpstring + 1, '\n');
        free(token);
        if (end == NULL)
            return NULL;
        bpstring = end;
        token = next_tok_from(&bpstring);
    } else if (one_char_sym(*bpstring)) {
        token[0] = *bpstring;
        token[1] = 0;
        bpstring++;
    } else if (*bpstring) {
        int i_token = 0;
        do {
            token[i_token] = *bpstring;
            i_token++;
            bpstring++;
        } while (*bpstring && part_of_symbol(*bpstring) &&
                 i_token < MAX_TOKEN_LENGTH - 1);
        token[i_token] = 0;
    } else {
        free(token);
        token = NULL;
    }
    *bpstring_pointer = bpstring;
    return token;
}

list_t * tokenize(char * bpstring) {
    list_t * current_lst_item;
    list_t * lst_head = NULL;
    char * new_token;
    while (*bpstring != 0) {
        new_token = next_tok_from(&bpstring);
        if (new_token == NULL)
            break;
        if (lst_head == NULL) {
            lst_head = malloc(sizeof(list_t));
            current_lst_item = lst_head;
            current_lst_item->item = new_token;
            current_lst_item->next = NULL;
        } else {
            current_lst_item->next = malloc(sizeof(list_t));
            current_lst_item = current_lst_item->next;
            current_lst_item->item = new_token;
            current_lst_item->next = NULL;
        }
    }
    return lst_head;
}

void write_inside_list(object_t * object) {
    if (object->bptuple.cdr->type == TT_NIL) {
        writeitm(object->bptuple.car);
        printf(")");
    } else if (object->bptuple.cdr->type == TT_PAIR) {
        writeitm(object->bptuple.car);
        printf(" ");
        write_inside_list(object->bptuple.cdr);
    } else {
        writeitm(object->bptuple.car);
        printf(" . ");
        writeitm(object->bptuple.cdr);
        printf(")");
    }
}

void writeitm(object_t * object) {
    int i;
    switch (object->type) {
        case TT_NIL:
            printf("()");
            break;
        case TT_INT:
            printf("%d", object->bpint.value);
            break;
        case TT_FLT:
            printf("%lf", object->bpflt.value);
            break;
        case TT_FRAC:
            printf("%d/%u", object->bpfrac.numerator, object->bpfrac.denominator);
            break;
        case TT_CHR:
            printf("#\\%c", object->bpchr.value);
            break;
        case TT_BOOL:
            printf("#%s", object->bpbool.value ? "t" : "f");
            break;
        case TT_STR:
            printf("\"%s\"", object->bpstr.value);
            break;
        case TT_VECTOR:
            printf("#(");
            for (i = 0; i < object->bpvec.length; i++) {
                writeitm(object->bpvec.value[i]);
                if (i < object->bpvec.length - 1)
                    printf(" ");
            }
            printf(")");
            break;
        case TT_PAIR:
            printf("(");
            write_inside_list(object);
            break;
        case TT_PROC: case TT_PRIM:
            printf("<procedure>");
            break;
        case TT_PORT:
            printf("<bpprt>");
            break;
        case TT_SYM:
            printf("%s", object->bpstr.value);
            break;
    }
}

void writeobj(object_t * object) {
    writeitm(object);
    printf("\n");
}


unsigned hash(char * bpstr) {
    unsigned valhash = 0;
    int i;
    for (i = 0; i < strlen(bpstr); i++)
        valhash = bpstr[i] + 31 * valhash;
    return valhash % HTABLE_SIZE;
}

struct phtab_t * env_entry_lookup(char * identifier, env_t * env) {
    unsigned uhash = hash(identifier);
    struct phtab_t * entry = env->bindings[uhash];
    while (entry != NULL) {
        if (!strcmp(entry->key, identifier))
            return entry;
        entry = entry->next;
    }
    return NULL;
}

struct phtab_t * entryget(char * identifier, env_t * env) {
    struct phtab_t * ret = NULL;
    while (env != NULL) {
        ret = env_entry_lookup(identifier, env);
        if (ret != NULL)
            return ret;
        env = env->parent;
    }
    return NULL;
}

object_t * objectget(char * identifier, env_t * env) {
    struct phtab_t * entry = entryget(identifier, env);
    if (entry == NULL)
        bpanic("Undefined identifier\n");
    object_t * ret = entry->refer;
    if (ret == NULL)
        bpanic("Accessing unspecific binding %s\n", identifier);
    return ret;
}

void objectdef(char * identifier, object_t * obj, env_t * env) {
    unsigned uhash = hash(identifier);
    struct phtab_t * entry = env->bindings[uhash];
    struct phtab_t * new_entry;
    new_entry = malloc(sizeof(struct phtab_t));
    new_entry->key = malloc(sizeof(char) * (strlen(identifier) + 1));
    strncpy(new_entry->key, identifier, strlen(identifier) + 1);
    new_entry->refer = obj;
    new_entry->next = NULL;
    if (entry == NULL) {
        env->bindings[uhash] = new_entry;
        return;
    }
    while (entry->next != NULL)
        entry = entry->next;
    entry->next = new_entry;
}

void objectset(char * identifier, object_t * obj, env_t * env) {
    struct phtab_t * entry = entryget(identifier, env);
    if (entry == NULL)
        bpanic("Trying to set undefined identifier %s\n", identifier);
    entry->refer = obj;
}

env_t * envcreat(env_t * parent) {
    env_t * envcreat = malloc(sizeof(env_t));
    int i;
    for (i = 0; i < HTABLE_SIZE; i++)
        envcreat->bindings[i] = NULL;
    envcreat->parent = parent;
    return envcreat;
}

void envfree(env_t * env) {
    int i;
    struct phtab_t * entry, *prev_entry;
    for (i = 0; i < HTABLE_SIZE; i++) {
        entry = env->bindings[i];
        while (entry != NULL) {
            prev_entry = entry;
            free(entry->key);
            entry = entry->next;
            free(prev_entry);
        }
    }
    free(env);
}

enum e_stack_inbpstr_type {
    INSTR_EVAL, INSTR_APPLY
};

typedef struct s_stack_inbpstr {
    enum e_stack_inbpstr_type type;
    env_t * env;
    object_t * obj;
    object_t * parameters;
} StackInbpstr;

StackInbpstr inbpstr_stack[MAX_STACK_SIZE];
int inbpstr_stack_pointer = 0;

object_t * ret_stack[MAX_STACK_SIZE];
int ret_stack_pointer = 0;

void print_ret_stack() {
    int i;
    for (i = 0; i < ret_stack_pointer; i++) {
        writeitm(ret_stack[i]);
        if (i != ret_stack_pointer - 1)
            printf(" | ");
    }
    printf("\n");
}

void inbpstr_stack_push(enum e_stack_inbpstr_type type, env_t * env,
                      object_t * obj, object_t * parameters) {
    if (inbpstr_stack_pointer > MAX_STACK_SIZE)
        bpanic("Stack overflow.\n");
    inbpstr_stack[inbpstr_stack_pointer].type = type;
    inbpstr_stack[inbpstr_stack_pointer].env = env;
    inbpstr_stack[inbpstr_stack_pointer].obj = obj;
    inbpstr_stack[inbpstr_stack_pointer].parameters = parameters;
    inbpstr_stack_pointer++;
}

StackInbpstr inbpstr_stack_pop() {
    if (inbpstr_stack_pointer <= 0)
        bpanic("Stack underflow.\n");
    return inbpstr_stack[--inbpstr_stack_pointer];
}

void ret_stack_push(object_t * obj) {
    if (ret_stack_pointer > MAX_STACK_SIZE)
        bpanic("Stack overflow.\n");
    ret_stack[ret_stack_pointer++] = obj;
}

object_t * ret_stack_pop() {
    if (ret_stack_pointer <= 0)
        bpanic("Stack underflow.\n");
    return ret_stack[--ret_stack_pointer];
}

void map_args(object_t * formals, object_t * parameters, env_t * env) {
    if (formals->type == TT_SYM)
        objectdef(formals->bpstr.value, parameters, env);
    else if (formals->type == TT_PAIR) {
        if (parameters->type != TT_PAIR)
            bpanic("Parameter debpstructuring error\n");
        map_args(formals->bptuple.car, parameters->bptuple.car, env);
        map_args(formals->bptuple.cdr, parameters->bptuple.cdr, env);
    } else if (formals->type == TT_NIL) {
        if (parameters->type != TT_NIL)
            bpanic("Parameter debpstructuring error\n");
    } else
        bpanic("Argument lists should only contain symbols and pairs\n");
}

object_t * map_eval(object_t * list, env_t * env) {
    object_t * ret = nil;
    if (list->type == TT_NIL)
        return nil;
    else if (list->type == TT_PAIR) {
        ret = malloc(sizeof(object_t));
        ret->type = TT_PAIR;
        ret->bptuple.car = eval(list->bptuple.car, env);
        ret->bptuple.cdr = map_eval(list->bptuple.cdr, env);
    } else
        bpanic("Malformed function call\n");
    return ret;
}

void st_apply(object_t * function, object_t * parameters, env_t * env) {
    env_t * running_env;
    if (function->type == TT_PRIM)
        ret_stack_push((function->bprim.f)(parameters, env));
    else if (function->type == TT_PROC) {
        running_env = envcreat(function->bpfn.env);
        map_args(function->bpfn.arguments, parameters, running_env);
        inbpstr_stack_push(INSTR_EVAL, running_env, function->bpfn.body, NULL);
    } else
        bpanic("Tried to apply a non-function\n");
}


void evals(object_t * to_eval, env_t * env) {
    int is_sym;
    char * sym;
    switch (to_eval->type) {
        case TT_STR:
        case TT_NIL:
        case TT_BOOL:
        case TT_FLT:
        case TT_INT:
        case TT_FRAC:
        case TT_CHR:
        case TT_PRIM:
        case TT_PROC:
            ret_stack_push(to_eval);
            break;
        case TT_SYM:
            ret_stack_push(objectget(to_eval->bpstr.value, env));
            break;
        case TT_PAIR:
            is_sym = (to_eval->bptuple.car->type == TT_SYM);
            sym = to_eval->bptuple.car->bpstr.value;
            if (is_sym && !strcmp(sym, "quote"))
                ret_stack_push(to_eval->bptuple.cdr->bptuple.car);
            else if (is_sym && !strcmp(sym, "begin")) {
                object_t * ret = nil;
                object_t * iter = to_eval->bptuple.cdr;
                if (iter == nil) {
                    ret_stack_push(ret);
                    return;
                }
                while (iter->bptuple.cdr != nil) {
                    eval(iter->bptuple.car, env);
                    iter = iter->bptuple.cdr;
                    ret_stack_pop();
                }
                inbpstr_stack_push(INSTR_EVAL, env, iter->bptuple.car, NULL);
            } else if (is_sym && !strcmp(sym, "define")) {
                object_t * identifier_obj = to_eval->bptuple.cdr->bptuple.car;
                object_t * new_value = eval(to_eval->bptuple.cdr->bptuple.cdr->bptuple.car,
                                          env);
                objectdef(identifier_obj->bpstr.value, new_value, env);
                ret_stack_push(nil);
            } else if (is_sym && !strcmp(sym, "set!")) {
                object_t * identifier_obj = to_eval->bptuple.cdr->bptuple.car;
                object_t * new_value = eval(to_eval->bptuple.cdr->bptuple.cdr->bptuple.car,
                                          env);
                objectset(identifier_obj->bpstr.value, new_value, env);
                ret_stack_push(nil);
            } else if (is_sym && !strcmp(sym, "if")) {
                object_t * cond;
                cond = eval(to_eval->bptuple.cdr->bptuple.car, env);
                if (cond->type == TT_BOOL && !cond->bpbool.value) {
                    if (to_eval->bptuple.cdr->bptuple.cdr->bptuple.cdr != nil)
                        inbpstr_stack_push(INSTR_EVAL, env, to_eval->bptuple.cdr->bptuple.cdr->bptuple.cdr->bptuple.car, NULL);
                    else
                        ret_stack_push(nil);
                } else
                    inbpstr_stack_push(INSTR_EVAL, env, to_eval->bptuple.cdr->bptuple.cdr->bptuple.car, NULL);
            } else if (is_sym && !strcmp(sym, "lambda")) {
                object_t * lambda = malloc(sizeof(object_t));
                object_t * body = malloc(sizeof(object_t));
                env_t * lambda_env = envcreat(env);
                body->type = TT_PAIR;
                body->bptuple.car = mksym("begin");
                body->bptuple.cdr = to_eval->bptuple.cdr->bptuple.cdr;
                lambda->type = TT_PROC;
                lambda->bpfn.arguments = to_eval->bptuple.cdr->bptuple.car;
                lambda->bpfn.body = body;
                lambda->bpfn.env = lambda_env;
                ret_stack_push(lambda);
            } else {
                object_t * function;
                object_t * parameters;
                function = eval(to_eval->bptuple.car, env);
                parameters = map_eval(to_eval->bptuple.cdr, env);
                inbpstr_stack_push(INSTR_APPLY, env, function, parameters);
            }
            break;
        default:
            bpanic("Unknown type");
            ret_stack_push(nil);
    }
}

object_t * eval(object_t * to_eval, env_t * env) {
    inbpstr_stack_push(INSTR_EVAL, env, to_eval, NULL);
    StackInbpstr to_exec;
    while (inbpstr_stack_pointer != 0) {
        to_exec = inbpstr_stack_pop();
        if (to_exec.type == INSTR_EVAL)
            evals(to_exec.obj, to_exec.env);
        else
            st_apply(to_exec.obj, to_exec.parameters, to_exec.env);
    }
    return ret_stack_pop();
}

void * objects_array[MAX_HEAP];
short objects_flags[MAX_HEAP];

static int find_free_index() {
    int i = 0;
    
    for (; i < MAX_HEAP; i++) {
        if (objects_array[i] == NULL)
            return i;
    }
    return -1;
}

void init_gc() {
    int i = 0;
    
    for (; i < MAX_HEAP; i++) {
        objects_array[i] = NULL;
        objects_flags[i] = 0;
    }
}

object_t * new_object(enum token_t type) {
    int free_index = find_free_index();
    if (free_index == -1)
        bpanic("No more space for new objects");
    object_t * ret = malloc(sizeof(object_t));
    ret->type = type;
    objects_array[free_index] = ret;
    return ret;
}

void collect_garbage(env_t * environment) {}

object_t * make_prim(char * identifier, object_t *(*f)(object_t *, env_t *)) {
    object_t * ret = malloc(sizeof(object_t));
    ret->type = TT_PRIM;
    ret->bprim.f = f;
    objectdef(identifier, ret, genv);
    return ret;
}

void init() {
    genv = envcreat(NULL);
    nil = malloc(sizeof(object_t));
    nil->type = TT_NIL;
    t = malloc(sizeof(object_t));
    t->type = TT_BOOL;
    t->bpbool.value = 1;
    f = malloc(sizeof(object_t));
    f->type = TT_BOOL;
    f->bpbool.value = 0;
    make_prim("+", prplus);
    make_prim(">", prgt);
    make_prim("-", prminus);
    make_prim("=", prequal);
    make_prim("*", prmul);
}

void bpanic(const char * fmt, ...) {
    va_list args;
    va_start(args, fmt);
    fprintf(stdout, "Error: ");
    vfprintf(stdout, fmt, args);
    fflush(stdout);
    fflush(stderr);
    exit(EXIT_FAILURE);
    va_end(args);
}

bool isatom(object_t * object) {
    return object->type != TT_PAIR;
}

list_t * appendl(list_t * l1, list_t * l2) {
    list_t * last_l1;
    if (l1 == NULL)
        return l2;
    for (last_l1 = l1; last_l1->next != NULL; last_l1 = last_l1->next);
    last_l1->next = l2;
    return l1;
}

object_t * mksym(char * val) {
    object_t * sym = malloc(sizeof(object_t));
    sym->type = TT_SYM;
    sym->bpstr.value = malloc(sizeof(char) * (strlen(val) + 1));
    strncpy(sym->bpstr.value, val, strlen(val) + 1);
    return sym;
}

int is_num_char(char c) {
    return c >= '0' && c <= '9';
}

int nextint(char * tok, int * value) {
    int val = 0;
    int sign = 1;
    if (tok[0] == '-') {
        sign = -1;
        tok++;
    }
    if (!*tok)
        return 0;
    while (*tok) {
        if (!is_num_char(*tok))
            return 0;
        val *= 10;
        val += (*tok) - '0';
        tok++;
    }
    *value = val * sign;
    return 1;
}

int nextfloat(char * tok, double * value) {
    int sign = 1;
    int bpint = 0;
    double frac = 0;
    int divider = 10;
    int not_null = 0;
    if (tok[0] == '-') {
        sign = -1;
        tok++;
    }
    if (!*tok)
        return 0;
    while (*tok) {
        if (!is_num_char(*tok)) {
            if (*tok == '.') {
                tok++;
                break;
            } else
                return 0;
        }
        not_null = 1;
        bpint *= 10;
        bpint += (*tok) - '0';
        tok++;
    }
    while (*tok) {
        if (!is_num_char(*tok))
            return 0;
        not_null = 1;
        frac += (*tok - '0') / (float) divider;
        divider *= 10;
        tok++;
    }
    *value = sign * ((double) bpint + frac);
    return not_null;
}

int nextfrac(char * tok, int * num_ptr, unsigned * denom_ptr) {
    int num = 0;
    unsigned denom = 0;
    int sign = 1;
    int num_not_null = 0, denom_not_null = 0;
    if (tok[0] == '-') {
        sign = -1;
        tok++;
    }
    if (!*tok)
        return 0;
    while (*tok) {
        if (!is_num_char(*tok)) {
            if (*tok == '/') {
                tok++;
                break;
            } else
                return 0;
        }
        num_not_null = 1;
        num *= 10;
        num += (*tok) - '0';
        tok++;
    }
    while (*tok) {
        if (!is_num_char(*tok))
            return 0;
        denom_not_null = 1;
        denom *= 10;
        denom += (*tok - '0');
        tok++;
    }
    *num_ptr = sign * num;
    *denom_ptr = denom;
    return num_not_null && denom_not_null;
}

object_t * parse_list(list_t ** tokens_pointer) {
    list_t * tokens = *tokens_pointer;
    char * first_token;
    object_t * ret;
    object_t * first, *rest;
    if (tokens == NULL)
        return NULL;
    first_token = tokens->item;
    if (!strcmp(first_token, ")")) {
        *tokens_pointer = tokens->next;
        ret = nil;
    } else if (!strcmp(first_token, ".")) {
        *tokens_pointer = tokens->next;
        first = parse(tokens_pointer);
        tokens = *tokens_pointer;
        if (tokens == NULL)
            return NULL;
        else if (strcmp(tokens->item, ")"))
            bpanic("Expected ) instead of %s after .\n", (char *) tokens->item);
        else
            *tokens_pointer = tokens->next;
        ret = first;
    } else {
        first = parse(tokens_pointer);
        rest = parse_list(tokens_pointer);
        if (rest == NULL)
            return NULL;
        ret = malloc(sizeof(object_t));
        ret->type = TT_PAIR;
        ret->bptuple.car = first;
        ret->bptuple.cdr = rest;
    }
    return ret;
}

object_t * parse(list_t ** tokens_pointer) {
    list_t * tokens = *tokens_pointer;
    if (tokens == NULL)
        return NULL;
    object_t * ret = malloc(sizeof(object_t));
    char * first_token = tokens->item;
    *tokens_pointer = tokens->next;
    if (!strcmp(first_token, "'")) {
        ret->type = TT_PAIR;
        ret->bptuple.car = malloc(sizeof(object_t));
        ret->bptuple.car->type = TT_SYM;
        ret->bptuple.car->bpstr.value = malloc(strlen("quote") + 1);
        strcpy(ret->bptuple.car->bpstr.value, "quote");
        ret->bptuple.cdr = malloc(sizeof(object_t));
        ret->bptuple.cdr->type = TT_PAIR;
        ret->bptuple.cdr->bptuple.cdr = nil;
        *tokens_pointer = tokens->next;
        ret->bptuple.cdr->bptuple.car = parse(tokens_pointer);
    } else if (!strcmp(first_token, "#t")) {
        ret->type = TT_BOOL;
        ret->bpbool.value = 1;
    } else if (!strcmp(first_token, "#f")) {
        ret->type = TT_BOOL;
        ret->bpbool.value = 0;
    } else if (!strncmp(first_token, "#\\", 2)) {
        if (strlen(first_token) < 3)
            bpanic("%s is an invalid symbol.\n", first_token);
        if (strlen(first_token) > 3)
            fprintf(stderr,
                    "Warning: extended chars not yet supported (parsing %s).\n",
                    first_token);
        ret->type = TT_CHR;
        ret->bpchr.value = first_token[2];
    } else if (strlen(first_token) >= 2 && first_token[0] == '"' &&
               first_token[strlen(first_token) - 1] == '"') {
        ret->type = TT_STR;
        ret->bpstr.length = (int) strlen(first_token) - 2;
        ret->bpstr.value = malloc((ret->bpstr.length + 2) * sizeof(char));
        strncpy(ret->bpstr.value, first_token + 1, ret->bpstr.length);
        ret->bpstr.value[ret->bpstr.length] = 0;
    } else if (nextint(first_token, &(ret->bpint.value)))
        ret->type = TT_INT;
    else if (nextfloat(first_token, &(ret->bpflt.value)))
        ret->type = TT_FLT;
    else if (nextfrac(first_token, &(ret->bpfrac.numerator),
                        &(ret->bpfrac.denominator)))
        ret->type = TT_FRAC;
    else if (!strcmp(first_token, "(")) {
        free(ret);
        ret = parse_list(tokens_pointer);
        if (ret == NULL)
            return NULL;
    } else if (!strcmp(first_token, ")") || !strcmp(first_token, "."))
        bpanic("Unmatched or unexpected %s\n", first_token);
    else {
        free(ret);
        ret = mksym(first_token);
    }
    return ret;
}

object_t * parsef(list_t ** tokens_pointer) {
    list_t * initial_tokens = *tokens_pointer;
    object_t * ret = parse(tokens_pointer);
    list_t * token;
    if (ret == NULL) {
        *tokens_pointer = initial_tokens;
        return NULL;
    }
    token =  initial_tokens;
    for (; token != NULL && token != *tokens_pointer;) {
        list_t * prev_token = token;
        free(token->item);
        token = token->next;
        free(prev_token);
    }
    return ret;
}
