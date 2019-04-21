#ifndef TYPES_H
#define TYPES_H

#include <stdio.h>
#include <stdlib.h>

typedef enum { false, true } bool;


typedef struct {

  char Type;
  char Value[256];
  
}CmdArg;


#define NONE -1


#endif
