#include "lisp_parser.h"

#define SEGS_MAX 50

LispCmd* lisp_parser(char* lisp_str) {
  int depth = 0;
  int instring= 0;
  int lastpos =0;

  int segs_number=0;

  char**segs =(char**)malloc(SEGS_MAX*sizeof(char*));
  
  LispCmd *lisp_cmd=NULL;
  int i=0;

  for(i=0;i<strlen(lisp_str);i++ ) {
    if (lisp_str[i] != ' ' && lisp_str[i] != '(' ) {
      if(depth == 0) {
        printf("syntax error %d\n",i);
        return NULL;
      }
    }

    if (lisp_str[i]) == '('){
      depth+=1;
      lastpos=i;
    }
    
    if (lisp_str[i] == ')') {
      if(lastpos < i) {
        char*tmp = (char*)malloc((i-lastpos+1+1)*sizeof(char));
        strncpy(tmp, lisp_str +lastpos+1, i-lastpos+1 );
        tmp[ i-lastpos+1 ] = '\0';
        if(segs_number < SEGS_MAX) { 
          segs[segs_number] = tmp;
          segs_number+=1;
        }else{
          break;
        }
      }

      depth-=1;
    }
    
    if (depth > 0 ) {
      if (lisp_str[i] == ' ') {
        if(instring == 0) {
          char*tmp = (char*)malloc((i-lastpos+1+1)*sizeof(char));
          strncpy(tmp, lisp_str +lastpos+1, i-lastpos+1 );
          tmp[ i-lastpos+1 ] = '\0';
          if(segs_number < SEGS_MAX) { 
            segs[segs_number] = tmp;
            segs_number+=1;
          }else{
            break;
          }
        }
      }
    }
    
    if (lisp_str[i] == '"') {
      if(instring == 0) {
        instring+=1;
      }else if(instring > 0 ){
        instring-=1;
      }
    }


  }

  if(depth > 0) {
    for(i=0;i<segs_number;i++){
      free(segs[i]);
    }
    free(segs);
    printf("syntax error ,unexcepted closure\n");
    return NULL;
  }
  
  if( instring > 0) {
    for(i=0;i<segs_number;i++){
      free(segs[i]);
    }
    free(segs);
    printf("syntax error , string quato errors\n");
    return NULL;
  }
  

  if(segs_number < 1) {
    for(i=0;i<segs_number;i++){
      free(segs[i]);
    }
    free(segs);
    printf("unknown error\n");
    return NULL;
  }
  
  lisp_cmd = (LispCmd*)malloc(sizeof(LispCmd));
  lisp_cmd->Func = segs[0];
  lisp_cmd->Args = NULL;
  lisp_cmd->Argc = 0;

  if(segs_number -1 > 0) {
    lisp_cmd->Args = (CmdArgs*)malloc( (segs_number-1)*sizeof(CmdArg));
    lisp_cmd->Argc = segs_number-1;
  }
  
  for(i=1;i<segs_number;i++) {

    if(segs[i][0] == '"') {
      lisp_cmd->Args[i].Type = 'S';
      strncpy(lisp_cmd->Args[i].Value, segs[i]+1, strlen(segs[i])-1-1 );

    }else if(strcmp(segs[i],"true") == 0 || strcmp(segs[i],"false") == 0 ) {
      lisp_cmd->Args[i].Type = "B";
      strcpy(lisp_cmd->Args[i],segs[i]);

    }else if(strchr(segs[i],'.') != NULL){
      lisp_cmd->Args[i].Type= "F";
      strcpy(lisp_cmd->Args[i],segs[i]);
    }else {
      lisp_cmd->Args[i].Type= "I";
      strcpy(lisp_cmd->Args[i],segs[i]);      
    }
    
  }


  for(i=0;i<segs_number;i++){
      free(segs[i]);
  }  
  free(segs);
  
  return lisp_cmd;
}

//------------------------------------------------------------------------------
char *CmdArg_GetStr(CmdArg*arg) {
  if(arg->Type != 'S') {
    return NULL;
  }

  return arg->Value;
}

double CmdArg_GetFloat(CmdArg*arg) {
  double ret;
  ret = 0.0;
  if(arg->Type != 'F') {
    return ret;
  }
  
  ret = strtod(arg->Value,NULL);
  
  return ret;

}


int CmdArg_GetInt(CmdArg*arg) {
  int ret;
  ret = 0;
  if(arg->Type != 'I') {
    return ret;
  }

  ret = atoi(arg->Value);
  return ret;
}

bool CmdArg_GetBoolean(CmdArg*arg) {
  if(arg->Type != "B") {
    return false;
  }

  if( strcmp( arg->Value,"true") == 0)  {
    return true;
  }

  if( strcmp( arg->Value,"false") == 0)  {
    return false;
  }
  
  return false;

}
