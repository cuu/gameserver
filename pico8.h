#ifndef PICO8_H
#define PICO8_H

#include <stdio.h>
#include <stdlib.h>

#include <SDL.h>
#include <SDL_ttf.h>


#include "types.h"

typedef struct {

  int Width;
  int Height;
  int Version;
  int MapMatrix[64*128];
  
  SDL_Surface *CanvasHWND;
  SDL_Surface *HWND;
  
  SDL_Surface* DisplayCanvas;
  SDL_Surface* DrawCanvas;
  SDL_Surface* GfxSurface;
  
  SDL_Color pal_colors[16];
  SDL_Color draw_colors[16];
  SDL_Color display_colors[16];
  
  int DrawPaletteIdx[16];
  SDL_Palette *DisplayPalette;
  SDL_Palette *DrawPalette;
  int PalTransparent[16];
  
  SDL_Rect ClipRect;
  int PenColor;
  
  int Cursor[2];
  int CameraDx;
  int CameraDy;
  
  bool PaletteModified;
  int SpriteFlags[256];
  int Uptime;
  
  TTF_Font *Font;
  
}Pico8;

Pico8* NewPico8();

#endif
