#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include <sys/time.h>
#include <sys/wait.h>
#include <sys/types.h>

#include "utils.h"

char *ltrim(char *str, const char *seps)
{
    size_t totrim;
    if (seps == NULL) {
        seps = "\t\n\v\f\r ";
    }
    totrim = strspn(str, seps);
    if (totrim > 0) {
        size_t len = strlen(str);
        if (totrim == len) {
            str[0] = '\0';
        }
        else {
            memmove(str, str + totrim, len + 1 - totrim);
        }
    }
    return str;
}


char *rtrim(char *str, const char *seps)
{
    int i;
    if (seps == NULL) {
        seps = "\t\n\v\f\r ";
    }
    i = strlen(str) - 1;
    while (i >= 0 && strchr(seps, str[i]) != NULL) {
        str[i] = '\0';
        i--;
    }
    return str;
}

char *trim(char *str, const char *seps)
{
    return ltrim(rtrim(str, seps), seps);
}

void panic(const char*format,...){

  va_list args;
  va_start(args, format);
  vprintf(format, args);
  va_end(args);
  
  exit(-1);

}


int max(int a, int b ) {
    if (a > b) {
        return a;
    }
    return b;
}
	
int min(int a, int b) {
    if (a < b) {
        return a;
    }
    return b;
}

int strpos(char *hay, char *needle, int offset)
{
  char haystack[strlen(hay)];
  strncpy(haystack, hay+offset, strlen(hay)-offset);
  char *p = strstr(haystack, needle);
  if (p){
    return p - haystack+offset;
  }
  return -1;
}


void itimeofday(long *sec, long *usec)
{
	struct timeval time;
	gettimeofday(&time, NULL);
	if (sec) *sec = time.tv_sec;
	if (usec) *usec = time.tv_usec;	
}

/* get clock in millisecond 64 */
IINT64 iclock64(void)
{
	long s, u;
	IINT64 value;
	itimeofday(&s, &u);
	value = ((IINT64)s) * 1000 + (u / 1000);
	return value;
}

IUINT32 iclock()
{
	return (IUINT32)(iclock64() & 0xfffffffful);
}

/* sleep in millisecond */
void isleep(unsigned long millisecond)
{
	/* usleep( time * 1000 ); */
	struct timespec ts;
	ts.tv_sec = (time_t)(millisecond / 1000);
	ts.tv_nsec = (long)((millisecond % 1000) * 1000000);
	/*nanosleep(&ts, NULL);*/
	usleep((millisecond << 10) - (millisecond << 4) - (millisecond << 3));

}
