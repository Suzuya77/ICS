#include "klib.h"

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

//use standard string.h for reference
size_t strlen(const char *s) {
	register const char *temp = s;

	while(*temp++);

	return --temp - s;
}

char *strcpy(char* dst,const char* src) {
	register char *temp = dst;

	while(*temp++ = *src++);

	return dst;
}

char* strncpy(char* dst, const char* src, size_t n) {
	register char *temp = dst;

	if (n > 0)
	{
		// n -= 1;
		while((*temp++ = *src++) && (--n > 0));

		if ((*--src == '\0') && (--n > 0))
		{
			while(n > 0){
				*temp++ = '\0'; 
				n--;
			}
		}
	}
	return dst;
}

char* strcat(char* dst, const char* src) {
	register char *temp = dst;

	while(*temp++ != '\0');

	temp--;

	while(*temp++ = *src++);

	return dst;
}

int strcmp(const char* s1, const char* s2) {
	while(*s1 == *s2){
		if (*s1 == '\0')
		{
			return 0;
		}
		s1++;
		s2++;
	}

	return (*(unsigned char*)s1 < *(unsigned char*)s2)?-1:1;
}

int strncmp(const char* s1, const char* s2, size_t n) {
  	while((*s1 == *s2) && n > 0){
		if (*s1 == '\0')
		{
			return 0;
		}
		s1++;
		s2++;
		n--;
	}

	return (*(unsigned char*)s1 < *(unsigned char*)s2)?-1:1;
}

void* memset(void* v,int c,size_t n) {
	register char *temp = v;

	while(n > 0){
		*temp = c;
		temp++;
		n--;
	}

	return v;
}

void* memcpy(void* out, const void* in, size_t n) {
	register char *temp1 = out;
	register const char *temp2 = in;

	while(n > 0){
		*temp1 = *temp2;
		temp1++;
		temp2++;
		n--;
	}

	return out;
}

int memcmp(const void* s1, const void* s2, size_t n){
	register const unsigned char *temp1 = s1, *temp2 = s2;

	while(n > 0){
		if (*temp1 == *temp2)
		{
			temp1++;
			temp2++;
			continue;
		}else{
			return *--temp1 - *--temp2;
		}
	}
	return 0;
}

#endif
