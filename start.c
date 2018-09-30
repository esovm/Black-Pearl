
#include "pearl.h"

void repl(FILE * input_file, bool verbose) {
    const int MAX_LINE = 1000;
    char line[MAX_LINE];
    list_t * tokens = NULL;
    object_t * parsed;
    char * prompt = " > ";
    while (1) {
        if (verbose)
            printf(prompt);
        if (fgets(line, MAX_LINE, input_file) == NULL)
            break;
        if (!strcmp(line, "exit"))
            break;
        tokens = appendl(tokens, tokenize(line));
        while (tokens != NULL) {
            parsed = parsef(&tokens);
            if (parsed == NULL) {
                prompt = " . ";
                break;
            } else
                prompt = " > ";
            writeobj(eval(parsed, genv));
        }
    }
}

int main(int argc, char ** argv) {
    FILE * input_file = stdin;
    bool verbose = true;
    init();
    printf("%s, Copyright (C) Kamila \"Palaiologos\" Szewczyk.\n", NAME);
    if (argc == 2) {
        input_file = fopen(argv[1], "r");
        if (input_file == NULL) {
            printf("Could not open file %s\n", argv[1]);
            exit(1);
        }
        verbose = false;
    } else if (argc > 2) {
        printf("Usage: %s (input_file)\n", argv[0]);
        exit(1);
    }
    repl(input_file, verbose);
    return 0;
}
