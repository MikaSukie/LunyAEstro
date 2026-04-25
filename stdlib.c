// GPL2.0 license
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <ctype.h>
#include <time.h>
#include <errno.h>

#ifndef WIFEXITED
#  define WIFEXITED(s)   (((s) & 0x7f) == 0)
#endif
#ifndef WEXITSTATUS
#  define WEXITSTATUS(s) (((s) >> 8) & 0xff)
#endif

static char* safe_strdup(const char* s) {
    if (!s) return NULL;
    size_t len = strlen(s) + 1;
    char* copy = malloc(len);
    if (!copy) return NULL;
    memcpy(copy, s, len);
    return copy;
}

void print(const char* s) {
    if (s) fputs(s, stdout);
}

void eprint(const char* s) {
    if (s) fputs(s, stderr);
}

char* tal(const char* s) {
    if (!s) return NULL;
    char* result = safe_strdup(s);
    if (!result) return NULL;

    for (char* p = result; *p; ++p) {
        *p = (char)tolower((unsigned char)*p);
    }

    return result;
}

int is_ascii(const char* s) {
    if (!s) return 0;

    for (const unsigned char* p = (const unsigned char*)s; *p; ++p) {
        if (*p > 0x7F) {
            return 0;
        }
    }

    return 1;
}

int64_t run_command(const char* cmd) {
    if (!cmd) return -1;

    int status = system(cmd);

    if (status == -1) {
        return -1;
    }

    if (WIFEXITED(status)) {
        return (int64_t)WEXITSTATUS(status);
    }

    return -2;
}

char* sinput(const char* prompt) {
    if (prompt) {
        fputs(prompt, stdout);
        fflush(stdout);
    }

    size_t cap = 128;
    size_t len = 0;
    char* buf = malloc(cap);
    if (!buf) return NULL;

    int ch;
    while ((ch = fgetc(stdin)) != EOF && ch != '\n') {
        if (len + 1 >= cap) {
            if (cap > SIZE_MAX / 2) {
                free(buf);
                return NULL;
            }
            cap *= 2;
            char* tmp = realloc(buf, cap);
            if (!tmp) {
                free(buf);
                return NULL;
            }
            buf = tmp;
        }
        buf[len++] = (char)ch;
    }

    buf[len] = '\0';
    return buf;
}

void wait(int64_t millis) {
    if (millis <= 0) return;

    struct timespec ts;
    ts.tv_sec  = millis / 1000;
    ts.tv_nsec = (millis % 1000) * 1000000L;

    nanosleep(&ts, NULL);
}