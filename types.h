#ifndef TYPES_H
#define TYPES_H

#include <stdio.h>
#include <stdlib.h>

#define TCPBUFF 8192
#define UDPBUFF 8192

typedef enum { false, true } bool;


typedef struct {

  char Type;
  char Value[256];
  
}CmdArg;


#define NONE -1

typedef struct {
  int x;
  int y;
}Point;

#endif
