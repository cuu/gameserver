#ifndef GAMETHREAD_H
#define GAMETHREAD_H

#include <stdio.h>
#include <stdlib.h>

#include <SDL_ttf.h>

#define MILL_USE_PREFIX
#include <libmill.h>


#include "types.h"
#include "utils.h"
#include "pico8.h"
#include "lisp_parser.h"

#define STATE_DRAW 0
#define STATE_RES 1

typedef struct {

  int Width;
  int Height;
  bool Inited;
  
  Pico8 *ThePico8;

  SDL_Texture*texture;
  SDL_Window *window;
  SDL_Surface *big_surface;
  SDL_Renderer *renderer;
  void *big_surface_pixels;
  

  int Frames;
  int CurrentTime;
  int PrevTime;
  
  int KeyLog[8];
  int state;
 
}GameThread;


GameThread* NewGameThread();


void GameThread_InitWindow(GameThread*self);
void GameThread_QuitWindow(GameThread*self);
void GameThread_EventLoop(GameThread*self);

mill_coroutine void GameThread_FlipLoop(GameThread*self);
char *GameThread_Btn(GameThread*self,LispCmd*lisp_cmd);

void GameThread_Run(GameThread*self);




#endif
