#include "klib.h"
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

#define MAX_CHAR_CAPABILITY 256

static inline void output(char * str){
    for (uint32_t i = 0; str[i] != '\0'; ++i)
    {
        _putc(str[i]);
    }
} 


int printf(const char *fmt, ...) {
    va_list args;
    char str[MAX_CHAR_CAPABILITY];
    int length;
    va_start(args, fmt);
    length = vsprintf(str, fmt, args);
    va_end(args);
    output(str);
    return length;
}

int vsprintf(char *out, const char *fmt, va_list ap) {
    int length;
    const char *t;
    char *str = out;
    for (; *fmt; fmt++) {
        if (*fmt != '%') {
            *str++ = *fmt;
            continue;
        }
        fmt++;
        switch (*fmt) {
            case 's': {
                t = va_arg(ap, char*);
                length = strlen(t);
                for (uint32_t i = 0; i < length; i++) {
                    *str = *t;
                    str++;
                    t++;
                }
                break;
            }
            case 'd': {
                uint32_t num = va_arg(ap, int);
                uint32_t i = 0;
                char c;
                if (num == 0){
                    *str = '0';
                    str++;
                }
                else {
                    if (num < 0) {
                        *str = '-';
                        str++;
                        num = (-num);
                    }
                    while (num != 0) {
                        *str = num % 10 + '0';
                        num /= 10;
                        str++;
                        i++;
                    }
                }
                for (uint32_t j = 1; j <= (i / 2); j++) {
                    c = *(str - j);
                    *(str - j) = *(str - (i - j + 1));
                    *(str - (i - j + 1)) = c;
                }
                break;
            }
            case 'c': {
                uint32_t num = va_arg(ap, int);
                *str = (char) num;
                str++;
                break;
            }
            case 'x': {
                uint32_t num = va_arg(ap, uint32_t);
                uint32_t i = 0;
                char nums[20];
                if (num == 0){
                    nums[0] = '0';
                }
                else {
                    while (num != 0) {
                        uint32_t x = num % 16;
                        if (x < 10){
                            nums[i] = x + '0';
                            i++;
                        }
                        else {
                            nums[i] = x - 10 + 'a';
                            i++;
                        }
                        num /= 16;
                    }
                }
                
                *str = '0';
                str++;
                *str = 'x';
                str++;

                for (int j = i - 1; j >= 0; j--) {
                    *str = nums[j];
                    str++;
                }
                break;
            }
            default:
                break;
        }
    }
    *str = '\0';
    return 0;
}

int sprintf(char *out, const char *fmt, ...) {
    va_list args;
    int length;
    va_start(args, fmt);
    length = vsprintf(out, fmt, args);
    va_end(args);
    return length;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
    return 0;
}

#endif