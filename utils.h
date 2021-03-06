#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include "ikcp.h"

#define MAX(a,b) ((a) > (b) ? a : b)
#define MIN(a,b) ((a) < (b) ? a : b)

int max(int a, int b);
int min(int a, int b);

char *ltrim(char *str, const char *seps);
char *rtrim(char *str, const char *seps);
char *trim(char *str, const char *seps);

void panic(const char*format,...);

int strpos(char *hay, char *needle, int offset);

///KCP
void itimeofday(long *sec, long *usec);
IINT64 iclock64(void);
IUINT32 iclock();
void isleep(unsigned long millisecond);


#endif
