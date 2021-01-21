// #include "klib.h"
// #include <stdarg.h>

// #if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

// #define MAX_CHAR_CAPABILITY 256

// static inline void output(char * str){
//     for (uint32_t i = 0; str[i] != '\0'; ++i)
//     {
//         _putc(str[i]);
//     }
// } 


// int printf(const char *fmt, ...) {
//     va_list args;
//     char str[MAX_CHAR_CAPABILITY];
//     int length;
//     va_start(args, fmt);
//     length = vsprintf(str, fmt, args);
//     va_end(args);
//     output(str);
//     return length;
// }

// int vsprintf(char *out, const char *fmt, va_list ap) {
//     int length;
//     const char *t;
//     char *str = out;
//     for (; *fmt; fmt++) {
//         if (*fmt != '%') {
//             *str++ = *fmt;
//             continue;
//         }
//         fmt++;
//         switch (*fmt) {
//             case 's': {
//                 t = va_arg(ap, char*);
//                 length = strlen(t);
//                 for (uint32_t i = 0; i < length; i++) {
//                     *str = *t;
//                     str++;
//                     t++;
//                 }
//                 break;
//             }
//             case 'd': {
//                 uint32_t num = va_arg(ap, int);
//                 uint32_t i = 0;
//                 char c;
//                 if (num == 0){
//                     *str = '0';
//                     str++;
//                 }
//                 else {
//                     if (num < 0) {
//                         *str = '-';
//                         str++;
//                         num = (-num);
//                     }
//                     while (num != 0) {
//                         *str = num % 10 + '0';
//                         num /= 10;
//                         str++;
//                         i++;
//                     }
//                 }
//                 for (uint32_t j = 1; j <= (i / 2); j++) {
//                     c = *(str - j);
//                     *(str - j) = *(str - (i - j + 1));
//                     *(str - (i - j + 1)) = c;
//                 }
//                 break;
//             }
//             case 'c': {
//                 uint32_t num = va_arg(ap, int);
//                 *str = (char) num;
//                 str++;
//                 break;
//               }
//             case 'X': {
//                 uint32_t num = va_arg(ap, uint32_t);
//                 uint32_t i = 0;
//                 char nums[20];
//                 if (num == 0){
//                     nums[0] = '0';
//                 }
//                 else {
//                     while (num != 0) {
//                         uint32_t x = num % 16;
//                         if (x < 10){
//                             nums[i] = x + '0';
//                             i++;
//                         }
//                         else {
//                             nums[i] = x - 10 + 'A';
//                             i++;
//                         }
//                         num /= 16;
//                     }
//                 }

//                 *str = '0';
//                 str++;
//                 *str = 'x';
//                 str++;

//                 for (int j = i - 1; j >= 0; j--) {
//                     *str = nums[j];
//                     str++;
//                 }
//                 break;
//               }
//             case 'x': {
//                 uint32_t num = va_arg(ap, uint32_t);
//                 uint32_t i = 0;
//                 char nums[20];
//                 if (num == 0){
//                     nums[0] = '0';
//                 }
//                 else {
//                     while (num != 0) {
//                         uint32_t x = num % 16;
//                         if (x < 10){
//                             nums[i] = x + '0';
//                             i++;
//                         }
//                         else {
//                             nums[i] = x - 10 + 'a';
//                             i++;
//                         }
//                         num /= 16;
//                     }
//                 }

//                 *str = '0';
//                 str++;
//                 *str = 'x';
//                 str++;

//                 for (int j = i - 1; j >= 0; j--) {
//                     *str = nums[j];
//                     str++;
//                 }
//                 break;
//               }
//             default:
//                 break;
//         }
//     }
//     *str = '\0';
//     return 0;
// }

// int sprintf(char *out, const char *fmt, ...) {
//     va_list args;
//     int length;
//     va_start(args, fmt);
//     length = vsprintf(out, fmt, args);
//     va_end(args);
//     return length;
// }

// int snprintf(char *out, size_t n, const char *fmt, ...) {
//     return 0;
// }

// #endif

#include "klib.h"
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

const char lower_dict[] = "0123456789abcdef";
const char upper_dict[] = "0123456789ABCDEF";

// 借鉴了 https://github.com/atgreen/FreeRTOS/blob/master/Demo/CORTEX_STM32F103_Primer_GCC/printf-stdarg.c
static inline int _printc(char **out, char c) {
  if (out) {
    **out = c;
    (*out)++;
  } else {
    _putc(c);
  }
  return 1;
}

static inline int _prints(char **out, const char *s) {
  int cnt = 0;
  while (*s != '\0') {
    cnt += _printc(out, *(s++));
  }
  return cnt;
}

static inline int _printi(char **out, int num, int base, int use_upper, int sign) {
  assert(base <= 16 && base >= 2);

  const int buf_sz = 51;
  char buf[buf_sz];
  buf[buf_sz - 1] = '\0';
  const char *dict = use_upper ? upper_dict : lower_dict;

  if (num == 0) {
    return _printc(out, dict[0]);
  }

  char *p = &buf[buf_sz - 1];
  if (!sign) {
    unsigned int n = *(unsigned int *)&num;
    while (n != 0) {
      p--;
      *p = dict[n % base];
      n /= base;
    }
  } else {
    int is_neg = 0;
    if (num < 0) {
      num = -num;
      is_neg = 1;
    }

    while (num != 0) {
      p--;
      *p = dict[num % base];
      num /= base;
    }

    if (is_neg) {
      p--;
      *p = '-';
    }
  }

  return _prints(out, p);
}

static inline int _print(char **out, const char *fmt, va_list ap) {
  const char *p = fmt;
  int cnt = 0;
  while (*p) {
    if (*p == '%') {
      p++;
      switch (*p) {
        case 'd':
          cnt += _printi(out, va_arg(ap, int), 10, 0, 1);
          break;
        case 'u':
          cnt += _printi(out, va_arg(ap, int), 10, 0, 0);
          break;
        case 'x':
          cnt += _printi(out, va_arg(ap, int), 16, 0, 0);
          break;
        case 'X':
          cnt += _printi(out, va_arg(ap, int), 16, 1, 0);
          break;
        case 's':
          cnt += _prints(out, va_arg(ap, char *));
          break;
        default:
          break;
      }
    } else {
      cnt += _printc(out, *p);
    }
    p++;
  }
  if (out)
    **out = '\0';
  return cnt;
}

int printf(const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  int cnt = _print(NULL, fmt, ap);
  va_end(ap);
  return cnt;
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  return _print(&out, fmt, ap);
}

int sprintf(char *out, const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  int cnt = _print(&out, fmt, ap);
  va_end(ap);
  return cnt;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  return 0;
}

#endif
