#ifndef PICO8_H
#define PICO8_H

#include <stdio.h>
#include <stdlib.h>

#include <SDL.h>
#include <SDL_ttf.h>


#include "types.h"
#include "lisp_parser.h"

#define RES_GFX    0
#define RES_GFF    1
#define RES_MAP    2
#define RES_SFX    3
#define RES_MUSIC  4


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
  int Uptime;
  
  TTF_Font *Font;
  
  unsigned char Map[64*128];//8k,32 shared ,
  unsigned char Sprite[128*128];//16k
  unsigned char SpriteFlags[256];//

  unsigned char Sfx[64*84];
  unsigned char Music[64*5];
  
  int res_state;
  int res_offset;

  float FPS;
}Pico8;

Pico8* NewPico8();


void Pico8_Res(Pico8*self,LispCmd*lisp_cmd);
void Pico8_ResOver(Pico8*self,LispCmd*lisp_cmd);
void Pico8_SetResource(Pico8*self,char*data);


#endif
