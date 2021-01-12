#include "klib.h"
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

// struct param {
// 	char uc:1;
// 	char sign;
// 	unsigned int base;
// 	char *bf;
// };

// typedef void (*putcf) (void *, char);

// struct _vsprintf_putcf_data {
// 	char *dest;
// 	size_t num_chars;
// };

// static void _vsprintf_putcf(void *p, char c) {
// 	struct _vsprintf_putcf_data *data = (struct _vsprintf_putcf_data *)p;
// 	data->dest[data->num_chars ++] = c;
// }

// static void _printf_putc(void *p, char c) {  
// 	_putc(c);
// }

// static void putchw(void *putp, putcf putf, struct param *p) {
// 	char ch;
// 	char *bf = p->bf;
// 	while((ch = *bf++)) 
// 		putf(putp, ch);
// }

// static void ui2a(unsigned int num, struct param *p) {
// 	int n = 0;
// 	unsigned int d = 1;
// 	char *bf = p->bf;
// 	while (num / d >= p->base)
// 		d = d * p->base;

// 	while (d != 0) {
// 		int dgt = num /d;
// 		num = num % d;
// 		d = d / p->base;
// 		if (n || dgt > 0 || d == 0) {
// 			*bf++ = dgt + (dgt < 10 ? '0' : (p->uc ? 'A' : 'a') - 10);
// 			++n;
// 		}
// 	}
// 	*bf = 0;
// }
// static void i2a (int num, struct param *p) {
// 	if (num < 0) {
// 		num = -num;
// 		p->sign = '-';
// 	}
// 	ui2a(num, p);
// }


// void my_format(void *putp, putcf putf, const char *fmt, va_list va) {
// 	struct param p;
// 	char bf[12];
// 	char ch;
// 	p.bf = bf;

// 	while ((ch = *(fmt++))) {
// 		if (ch != '%') {
// 			putf(putp, ch);
// 		} else {
// 			ch = *(fmt++);
// 			switch (ch) {
// 				case 'd':
// 					p.base = 10;
// 					p.uc = 0;
// 					i2a(va_arg(va, int), &p);
// 					putchw(putp, putf, &p);
// 					break;
// 				case 'x' :
// 					p.base = 16;
// 					p.uc = 0;
// 					i2a(va_arg(va, int), &p);
// 					putchw(putp, putf, &p);
// 					break;
// 				case 's':
// 					p.bf = va_arg(va, char *);
// 					putchw(putp, putf, &p);
// 				    p.bf = bf; 
// 					break;
// 				default : break;
// 			}
// 		}

// 	}
// }


// int vsprintf(char *out, const char *fmt, va_list ap) {
// 	struct _vsprintf_putcf_data data;
// 	data.dest = out;
// 	data.num_chars = 0;
// 	my_format(&data, _vsprintf_putcf, fmt, ap);
// 	data.dest[data.num_chars] = '\0';

// 	return data.num_chars;
// }

// int sprintf(char *out, const char *fmt, ...) {
// 	va_list ap;
// 	int ret;
	
// 	va_start(ap, fmt);
// 	ret = vsprintf(out, fmt, ap);
// 	va_end(ap);

// 	return ret;
// }

// int printf(const char *fmt, ...) {
// 	va_list ap;
// 	va_start(ap, fmt);
// 	my_format(NULL, _printf_putc, fmt, ap);
// 	va_end(ap);

// 	return 0;
// }

// int snprintf(char *out, size_t n, const char *fmt, ...) {

//   return 0;
// }

// for reference
const char lower_dict[] = "0123456789abcdef";
const char upper_dict[] = "0123456789ABCDEF";

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
  	int flag = 0;
  	int cnt = 0;
  	while (*p) {
    	if (*p == '%' || flag == 1) {
    		flag = 0;
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
	        	case 'c':
	        		cnt += _printc(out, va_arg(ap, int));
	        		break;
	        	default:
	        		if (*p >= '0' && *p <= '9')
	        		{
	        			do{
	        				printf("hey\n");
	        				p++;
	        			}while(*p >= '0' && *p <= '9');
	        			p--;
	        			flag = 1;
	        		}
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