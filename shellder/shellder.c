#include "../kernel/kernel.h"
#include "../libc/string.h"
//#include <string.h>
#include "../libc/mem.h"
#include "../drivers/cscreen.h"
#include <stdlib.h>

#define DEFAULT_BUFF 20

char* shdr_read();
char** shdr_parse(char*);
void shdr_exec(char**);

void shellder() {
    char* read;
    char** parsed;

    read = shdr_read();
    parsed = shdr_parse(read);
    shdr_exec(parsed);
}

/* TODO: impliment the functions to make this (or something like this) possible */
/*
char* shdr_read() {
    char* line;
    size_t buf_size = 0

    getline(&line, &buf_size, stdin);

    return line;
}
*/

char* shdr_read() {
    return text_buffer;  // defined in kernel.h
}

/* TODO: impliment the fucntions to make this (or something like this) possible */
/*
char** shdr_parse(char* line) {

    int bufsize = DEFAULT_BUFF
    int pos = 0;
    char **tokens = malloc(bufsize * sizeof(char*));
    char *token;

    token = strtok(line, " ");
    while (token != NULL) {
        tokens[pos] = token;
        pos++;

        if (pos >= bufsize) {
            bufsize += DEFAULT_BUFF;
            tokens = realloc(tokens, bufsize * sizeof(char*));
        }

        token = strtok(NULL, " ");
    }

    tokens[pos] = NULL;
    return tokens;
}
*/

char** shdr_parse(char* line) {
    char** tokens;
    char* token;
    int pos = 0;
    
    kmalloc(DEFAULT_BUFF * sizeof(char*), 1, (uint32_t*)tokens); // TODO: Replace with an malloc ASAP
    token = strtok(line, " ");
    while (token != 0) {            // we don't have NULL yet
        tokens[pos] = token;
        pos++;

        token = strtok(0, " "); 
    }
    
    tokens[pos] = 0;
    return tokens;
}

void shdr_exec(char** tokens) {
    // for now, echo
    for (; *tokens != 0; tokens++) {
        _prints(*tokens);
        _prints("\n");
    }   
    _prints("\n{shdr}> ");
}
