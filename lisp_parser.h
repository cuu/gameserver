#ifndef LISP_PARSER_H
#define LISP_PARSER_H

#include "types.h"

typedef struct {
  char Func[32];
  
  CmdArg*Args;
  int Argc;
  
}LispCmd;

LispCmd* lisp_parser(char* lisp_str);

int CmdArg_GetInt(CmdArg*arg);
bool CmdArg_GetBoolean(CmdArg*arg);
double CmdArg_GetFloat(CmdArg*arg);
char *CmdArg_GetStr(CmdArg*arg);

#endif
