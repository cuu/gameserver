#ifndef GAMETHREAD_H
#define GAMETHREAD_H

#include <stdio.h>
#include <stdlib.h>

#include "types.h"
#include "pico8.h"

#include <SDL_ttf.h>


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
  
}GameThread;


GameThread* NewGameThread();


#endif
