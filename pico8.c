#include "pico8.h"
#include "utils.h"
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
  
  p->FPS = 60.0;
  return p;
}

void Pico8_SetGfx(Pico8*self,char*data) { //data is one line 
  int i=0;
  int col=0;
  long ret;
  char tmp[2];
  
  data = trim(data,"\n");
  
  if(strlen(data) < 3 ) { return ; }

  for(i=0;i<strlen(data);i++) {
    memset(tmp,0,2);
    tmp[0] = data[i];
    ret = strtol(tmp,NULL,16);
    self->Sprite[col+ self->res_offset*128] = (unsigned char)ret;
    //printf("%ld ",ret);
    col+=1;
    if(col >= 128) {
      break;
    }
  }

  //set shared map
  self->res_offset+=1;
}

void Pico8_SetGff(Pico8*self,char*data) {
  int i=0;
  int col=0;
  long ret;
  char tmp[3];
  
  data = trim(data,"\n");
  
  for(i=0;i<strlen(data);i+=2) {
    memset(tmp,0,3);
    tmp[0] = data[i];
    tmp[1] = data[i+1];
    ret = strtol(tmp,NULL,16);
    self->SpriteFlags[col] = (unsigned char)ret;
    col++;
    if(col > 127) {
      break;
    }
  }

  self->res_offset+=1;

}

void Pico8_SetMap(Pico8*self,char*data) { // 32*128, 32*128,two parts
  int i=0;
  int col=0;
  long ret;
  char tmp[3];
  
  for(i=0;i<strlen(data);i+=2) {
    memset(tmp,0,3);
    tmp[0] = data[i];
    tmp[1] = data[i+1];
    ret = strtol(tmp,NULL,16);
    self->Map[self->res_offset + col*64] = (unsigned char)ret;
    col++;
    if(col > 127) {
      break;
    }
  }  

  self->res_offset+=1;

}

void Pico8_SetSharedMap(Pico8*self,LispCmd*lisp_cmd) {
  
  int shared=0;

  int tx = 0;
  int ty = 32;
  int sx,sy;
  unsigned char lo,hi;
  unsigned char v;
  if(self->Version > 3) {
    for(sy=64;sy<128;sy++)
      for(sx=0;sx<128;sx+=2)
      {
        lo = self->Sprite[sx+sy*128];
        hi = self->Sprite[sx+1+sy*128];
        v = (hi << 4) | lo;
        self->Map[ty+tx*64] = v;
        
        shared+=1;
        tx+=1;
        if (tx == 128) {
          tx = 0;
          ty+=1;
        }
      }
      printf("Map Shared: %d\n",shared);
  }

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

void Pico8_Version(Pico8*self,LispCmd*lisp_cmd) {

  int tmp;

  if(lisp_cmd->Argc == 0 ) {
    return;
  }

  tmp = CmdArg_GetInt(&lisp_cmd->Args[0]);
  printf("Set version %d %s\n",tmp,lisp_cmd->Args[0].Value);
  self->Version = tmp;
  
}

void Pico8_Res(Pico8*self,LispCmd*lisp_cmd) {
  
  int tmp;
  tmp = -1;
  self->res_offset = 0;
  
  tmp = CmdArg_GetInt(&lisp_cmd->Args[0]);
  
  self->res_state = tmp;
  
}

void Pico8_ResOver(Pico8*self,LispCmd*lisp_cmd) {
  
  self->res_offset = 0;
  self->res_state = -1;
  
  //clean up
  
}

void Pico8_ResDone(Pico8*self,LispCmd*lisp_cmd) {
  
  Pico8_SetSharedMap(self,lisp_cmd);
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

