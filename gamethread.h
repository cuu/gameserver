#ifndef GAMETHREAD_H
#define GAMETHREAD_H

#include <stdio.h>
#include <stdlib.h>

#include "pico8.h"

typedef enum { false, true } bool;

typedef struct GameThread {

  int Width;
  int Height;
  bool Inited;
  
  Pico8 *ThePico8;
  
  
  
}GameThread;


GameThread* NewGameThread();


#endif
