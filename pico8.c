#include "pico8.h"
#include "lisp_parser.h"

Pico8* NewPico8() {
  SDL_Surface *surface;
  int i;

  Pico8*p = NULL;
  p = (Pico8*)malloc(sizeof(Pico8));

  p->Width = 128;
  p->Height = 128;
  p->Version = 8;
    
  p->pal_colors[0]  = (SDL_Color){0,0,0,255};
  p->pal_colors[1]  = (SDL_Color){29,43,83,255};
  p->pal_colors[2]  = (SDL_Color){126,37,83,255};
  p->pal_colors[3]  = (SDL_Color){0,135,81,255};
  p->pal_colors[4]  = (SDL_Color){171,82,54,255};
  p->pal_colors[5]  = (SDL_Color){95,87,79,255};
  p->pal_colors[6]  = (SDL_Color){194,195,199,255};
  p->pal_colors[7]  = (SDL_Color){255,241,232,255};
  p->pal_colors[8]  = (SDL_Color){255,0,77,255};
  p->pal_colors[9]  = (SDL_Color){255,163,0,255};
  p->pal_colors[10] = (SDL_Color){255,240,36,255};
  p->pal_colors[11] = (SDL_Color){0,231,86,255};
  p->pal_colors[12] = (SDL_Color){41,173,255,255};
  p->pal_colors[13] = (SDL_Color){131,118,156,255};
  p->pal_colors[14] = (SDL_Color){255,119,168,255};
  p->pal_colors[15] = (SDL_Color){255,204,170,255};
  
    
  surface = SDL_CreateRGBSurface(0, p->Width, p->Height, 8,0,0,0,0);
  if (surface == NULL) {
        SDL_Log("SDL_CreateRGBSurface() failed: %s", SDL_GetError());
        exit(1);
  }
  p->DisplayCanvas = surface;
  
  surface = SDL_CreateRGBSurface(0, p->Width, p->Height, 8,0,0,0,0);
  if (surface == NULL) {
        SDL_Log("SDL_CreateRGBSurface() failed: %s", SDL_GetError());
        exit(1);
  }

  p->DrawCanvas = surface;

  surface = SDL_CreateRGBSurface(0, p->Width, p->Height,32,0,0,0,0);
  if (surface == NULL) {
        SDL_Log("SDL_CreateRGBSurface() failed: %s", SDL_GetError());
        exit(1);
  }
  
  p->GfxSurface = surface;
  
  for(i=0;i<sizeof(p->pal_colors)/sizeof(SDL_Color);i++ ){
    p->draw_colors[i] = p->pal_colors[i];
    p->display_colors[i] = p->pal_colors[i];
  }
  
  p->DrawPalette = SDL_AllocPalette(16);
  p->DisplayPalette = SDL_AllocPalette(16);
  
  SDL_SetPaletteColors(p->DrawPalette, p->draw_colors,0,16);
  SDL_SetPaletteColors(p->DisplayPalette, p->display_colors,0,16);
  
  
  SDL_SetSurfacePalette(p->DisplayCanvas,p->DisplayPalette);
  SDL_SetSurfacePalette(p->DrawCanvas,p->DrawPalette);
  
  for(i=0;i<16;i++) {
    p->DrawPaletteIdx[i] =i;
    if(i==0) {
      p->PalTransparent[i] = 0;
    }else {
      p->PalTransparent[i] = 1;
    }
  }
 	
  p->Font=TTF_OpenFont("PICO-8.ttf", 4);
  if(!p->Font) {
    printf("TTF_OpenFont: %s\n", TTF_GetError());
    exit(-1);
  }
  
  p->PenColor = 1;
  
  p->Version = 8;
  
  p->Uptime = SDL_GetTicks();
  
  
  p->res_state = -1;
  p->res_offset = 0;
  
  p->Cursor[0] = 0;
  p->Cursor[1] = 0;

  // clear resource
  memset(p->Map,0,64*128);
  memset(p->Sprite,0,128*128);
  memset(p->SpriteFlags,0,256);
  
  memset(p->Sfx,0,64*84);
  memset(p->Music,0,64*5);

  return p;
}

void Pico8_SetGfx(Pico8*self,char*data) { //data is one line 
  self->res_offset+=1;
}

void Pico8_SetGff(Pico8*self,char*data) {
  self->res_offset+=1;

}

void Pico8_SetMap(Pico8*self,char*data) {
  self->res_offset+=1;

}

void Pico8_SetResource(Pico8*self,char*data) {
  switch(self->res_state) {
    case RES_GFX:
      Pico8_SetGfx(self,data);
    break;
    case RES_GFF:
      Pico8_SetGff(self,data);
    break;
    case RES_MAP:
      Pico8_SetMap(self,data);
    break;
  }
}

void Pico8_Res(Pico8*self,LispCmd*lisp_cmd) {
  
  int tmp;
  tmp = -1;
  self->res_offset = 0;
  
  tmp = CmdArg_GetInt(&lisp_cmd->Args[0]);
  
  self->res_state = tmp;
  
}

void Pico8_ResDone(Pico8*self,LispCmd*lisp_cmd) {
  
  self->res_offset = 0;
  self->res_state = -1;
  
  //clean up
  
}

void Pico8_Cls(Pico8*self,LispCmd*lisp_cmd) {
  int color_index = 0;
  if(lisp_cmd->Argc == 0) {
    SDL_FillRect(self->DrawCanvas, NULL, SDL_MapRGB(self->DrawCanvas->format, 0, 0, 0));
    return;
  }
  
  color_index = CmdArg_GetInt(&lisp_cmd->Args[0]);
  if(color_index >=0 && color_index < 16 ) {
    SDL_FillRect(self->DrawCanvas, NULL, self->DrawPaletteIdx[ color_index]);
  }
  
  self->Cursor[0] = 0;
  self->Cursor[1] = 0;
  
}

