#include "string.h"
#include <stdint.h>

/**
 * K&R implementation
 */
void int_to_ascii(int n, char str[]) {
    int i, sign;
    if ((sign = n) < 0) n = -n;
    i = 0;
    do {
        str[i++] = n % 10 + '0';
    } while ((n /= 10) > 0);

    if (sign < 0) str[i++] = '-';
    str[i] = '\0';

    reverse(str);
}

void hex_to_ascii(int n, char str[]) {
    append(str, '0');
    append(str, 'x');
    char zeros = 0;

    int32_t tmp;
    int i;
    for (i = 28; i > 0; i -= 4) {
        tmp = (n >> i) & 0xF;
        if (tmp == 0 && zeros == 0) continue;
        zeros = 1;
        if (tmp > 0xA) append(str, tmp - 0xA + 'a');
        else append(str, tmp + '0');
    }

    tmp = n & 0xF;
    if (tmp >= 0xA) append(str, tmp - 0xA + 'a');
    else append(str, tmp + '0');
}

/* K&R */
void reverse(char s[]) {
    int c, i, j;
    for (i = 0, j = strlen(s)-1; i < j; i++, j--) {
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
}

/* K&R */
int strlen(char s[]) {
    int i = 0;
    while (s[i] != '\0') ++i;
    return i;
}

void append(char s[], char n) {
    int len = strlen(s);
    s[len] = n;
    s[len+1] = '\0';
}

int backspace(char s[]) {
    int len = strlen(s);

    if (len > 0) {
        s[len-1] = '\0';
    }

    return len;
}

// Concatenates two strings, followed by a null char
char *strcat(char dest[], const char source[]) {
    int i, j = 0;

    // get to the end of the first string
    while (dest[i])
        i++;

    // copy over the contents from source
    while (source[j]) {
        dest[i] = source[j];
        i++;
        j++;
    }

    // append null char
    dest[i + 1] = '\0';
    return dest;
}

// Safe concatenation of two strings, followed by a null char
char *strncat(char dest[], const char source[], int n) {
    int i, j = 0;

    // Get to the end of the first string
    while (dest[i])
        i++;

    // copy over the contents from source as far as n
    while (source[j] && j < n) {
        dest[i] = source[j];
        i++;
        j++;
    }

    // finally, append null char
    dest[i + 1] = '\0';
    return dest;
}



/* K&R 
 * Returns <0 if s1<s2, 0 if s1==s2, >0 if s1>s2 */
int strcmp(const char s1[], const char s2[]) {
    int i;
    for (i = 0; s1[i] == s2[i]; i++) {
        if (s1[i] == '\0') return 0;
    }
    return s1[i] - s2[i];
}

// strcmp, but with a limit
int strncmp(const char s1[], const char s2[], int n) {
    int i;
    for (i = 0; s1[i] == s2[i] && i < n; i++) {
        if (s1[i] == '\0')
            return 0;
    }

    return s1[i] - s2[i];
}

// copies a string from one location to another
char *strcpy(char dest[], const char source[]) {
    int i = 0;
    while (source[i]) {
        dest[i] = source[i];
        i++;
    }

    // we should copy the nullchar as well
    dest[i + 1] = '\0';

    return dest;
}

// copies a string from one location to another, with a limit
char *strncpy(char dest[], const char source[], int n) {
    int i = 0;
    while (source[i] && i < n) {
        dest[i] = source[i];
        i++;
    }

    // we should only copy the nullchar if i != n (if we haven't reached the end of the string)
    if (i < n) {
        dest[i + 1] = '\0';
    }

    return dest;
}