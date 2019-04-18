#ifndef LISP_PARSER_H
#define LISP_PARSER_H

#include "types.h"

typedef struct {
  char Func[32];
  
  CmdArgs*Args;
  int Argc;
  
}LispCmd;


#endif
