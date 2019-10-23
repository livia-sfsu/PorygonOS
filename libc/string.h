#ifndef STRINGS_H
#define STRINGS_H

void int_to_ascii(int n, char str[]);
void hex_to_ascii(int n, char str[]);
void reverse(char s[]);
int strlen(char s[]);
int backspace(char s[]);
void append(char s[], char n);
int strcmp(const char s1[], const char s2[]);
int strncmp(const char s1[], const char s2[], int n);
char *strcat(char dest[], const char source[]);
char *strncat(char dest[], const char source[], int n);
char *strcpy(char dest[], const char source[]);
char *strncpy(char dest[], const char source[], int n);

#endif
