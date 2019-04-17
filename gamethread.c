#include "gamethread.c"


GameThread*NewGameThread() {
  GameThread*p = NULL;
  
  p = (GameThread*)malloc(sizeof(GameThread));
  
  p->Width = 320;
  p->Height = 240;
  
  p->Inited = false;
  
  return p;
}

