
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
