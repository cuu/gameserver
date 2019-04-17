#include "pico8.h"


Pico8* NewPico8() {
  SDL_Surface *surface;
  int i;

  Pico8*p = NULL;
  p = (Pico8*)malloc(sizeof(Pico8));

  p->Width = 128;
  p->Height = 128;
  p->Version = 8;
  
  p->ClipRect = (SDL_Rect*)malloc(sizeof(SDL_Rect));
  
  p.pal_colors[0]  = {0,0,0,255};
  p.pal_colors[1]  = {29,43,83,255};
  p.pal_colors[2]  = {126,37,83,255};
  p.pal_colors[3]  = {0,135,81,255};
  p.pal_colors[4]  = {171,82,54,255};
  p.pal_colors[5]  = {95,87,79,255};
  p.pal_colors[6]  = {194,195,199,255};
  p.pal_colors[7]  = {255,241,232,255};
  p.pal_colors[8]  = {255,0,77,255};
  p.pal_colors[9]  = {255,163,0,255};
  p.pal_colors[10] = {255,240,36,255};
  p.pal_colors[11] = {0,231,86,255};
  p.pal_colors[12] = {41,173,255,255};
  p.pal_colors[13] = {131,118,156,255};
  p.pal_colors[14] = {255,119,168,255};
  p.pal_colors[15] = {255,204,170,255};

    
  surface = SDL_CreateRGBSurface(0, p.Width, p.Height, 8,0,0,0,0);
  if (surface == NULL) {
        SDL_Log("SDL_CreateRGBSurface() failed: %s", SDL_GetError());
        exit(1);
  }
  p.DisplayCanvas = surface;
  
  surface = SDL_CreateRGBSurface(0, p.Width, p.Height, 8,0,0,0,0);
  if (surface == NULL) {
        SDL_Log("SDL_CreateRGBSurface() failed: %s", SDL_GetError());
        exit(1);
  }

  p.DrawCanvas = surface

  surface = SDL_CreateRGBSurface(0, p.Width, p.Height,32,0,0,0,0);
  if (surface == NULL) {
        SDL_Log("SDL_CreateRGBSurface() failed: %s", SDL_GetError());
        exit(1);
  }
  
  p.GfxSurface = surface
  
  for(i=0;i<sizeof(p.pal_colors)/sizeof(SDL_Color);i++ ){
    p.draw_colors[i] = p.pal_colors[i]
    p.display_colors[i] = p.pal_colors[i]
  }
  
  p.DrawPalette = SDL_AllocPalette(16);
  p.DisplayPalette = SDL_AllocPalette(16);
  
  SDL_SetPaletteColors(p.DrawPalette, &p.draw_colors,0,16);
  SDL_SetPaletteColors(p.DisplayPalette, &p.display_colors,0,16);
  
  

}
