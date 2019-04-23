#include "pico8.h"
#include "draw.h"
#include "utils.h"
#include "lisp_parser.h"
#include "transform.h"
#include "surface.h"


Pico8* NewPico8() {
  SDL_Surface *surface;
  int i;

  Pico8*p = NULL;
  p = (Pico8*)malloc(sizeof(Pico8));

  p->Width = 128;
  p->Height = 128;
  p->Version = 8;
  
  /*
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
  */

  p->pal_colors[0]  = (SDL_Color){0,0,0,255};
  p->pal_colors[1]  = (SDL_Color){83,43,29,255};
  p->pal_colors[2]  = (SDL_Color){83,37,126,255};
  p->pal_colors[3]  = (SDL_Color){81,135,0,255};
  p->pal_colors[4]  = (SDL_Color){54,82,171,255};
  p->pal_colors[5]  = (SDL_Color){79,87,95,255};
  p->pal_colors[6]  = (SDL_Color){199,195,194,255};
  p->pal_colors[7]  = (SDL_Color){232,241,255,255};
  p->pal_colors[8]  = (SDL_Color){77,0,255,255};
  p->pal_colors[9]  = (SDL_Color){0,163,255,255};
  p->pal_colors[10] = (SDL_Color){36,240,255,255};
  p->pal_colors[11] = (SDL_Color){86,231,0,255};
  p->pal_colors[12] = (SDL_Color){255,173,41,255};
  p->pal_colors[13] = (SDL_Color){156,118,131,255};
  p->pal_colors[14] = (SDL_Color){168,119,255,255};
  p->pal_colors[15] = (SDL_Color){170,204,255,255};
    
  surface = SDL_CreateRGBSurface(0, p->Width, p->Height, 32,0,0,0,0);
  if (surface == NULL) {
        SDL_Log("SDL_CreateRGBSurface() failed: %s", SDL_GetError());
        exit(1);
  }
  p->DisplayCanvas = surface;
  
  surface = SDL_CreateRGBSurface(0, p->Width, p->Height, 32,0,0,0,0);
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
  
  for(i=0;i<16;i++ ){
    p->draw_colors[i] = p->pal_colors[i];
    p->display_colors[i] = p->pal_colors[i];
  }
  
  p->Draw_Palette = SDL_AllocPalette(16);
  p->DisplayPalette = SDL_AllocPalette(16);
 // p->Guu_Palette    = SDL_AllocPalette(16);

  
  SDL_SetPaletteColors(p->Draw_Palette, p->draw_colors,0,16);
  SDL_SetPaletteColors(p->DisplayPalette, p->display_colors,0,16);
  
  
  SDL_SetSurfacePalette(p->DisplayCanvas,p->DisplayPalette);
  SDL_SetSurfacePalette(p->DrawCanvas,p->Draw_Palette);
  
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
  p->Map = (char*)malloc(64*128*sizeof(char));
  p->Sprite = (char*)malloc(128*128*sizeof(char));
  p->SpriteFlags = (char*)malloc(256*sizeof(char));
  p->Sfx    = (char*)malloc(64*84*sizeof(char));
  p->Music  = (char*)malloc(64*5*sizeof(char));

  memset(p->Map,0,64*128); //64*128=8192
  memset(p->Sprite,0,128*128);
  memset(p->SpriteFlags,0,256);
  
  memset(p->Sfx,0,64*84);
  memset(p->Music,0,64*5);
  
  p->FPS = 60.0;
  p->PaletteModified = false;
  

  p->frames=0;
  p->curr_time=0;
  p->prev_time=0;

  return p;
}

void Pico8_sync_draw_pal(Pico8*self) {
  int i;
  for (i=0;i<16;i++) {
    self->draw_colors[i] = self->pal_colors[ self->DrawPaletteIdx[i]  ];
  }
  
  SDL_SetPaletteColors(self->Draw_Palette, self->draw_colors,0,16);

}

void Pico8_SetGfx(Pico8*self,char*data) { //data is one line 
  int i=0;
  int col=0;
  long ret;
  char tmp[2];
  
  data = trim(data,"\n");
  
  if(strlen(data) < 3 ) { return; }

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
  if(self->Version > 2) {
    for(i=0;i<strlen(data);i+=2) {
      memset(tmp,0,3);
      tmp[0] = data[i];
      tmp[1] = data[i+1];
      ret = strtol(tmp,NULL,16);
      self->SpriteFlags[self->res_offset] = (unsigned char)ret;
      self->res_offset+=1;
      if(self->res_offset > 255) {
        break;
      }
    }
  }
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
    //printf("%d ",(unsigned char)ret);
    self->Map[self->res_offset + col*64] = (unsigned char)ret;
    col++;
    if(col > 127) {
      break;
    }
  }

  //printf("\n");
  self->res_offset+=1;

}

void Pico8_set_shared_map(Pico8*self) {
  
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
        //printf("%d ",v);
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
  printf("Set version %d\n",tmp);
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
  
  Pico8_set_shared_map(self);

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

void Pico8_Spr(Pico8*self,LispCmd*lisp_cmd) {
  int n,x,y,w,h;
  int flip_x,flip_y;
  int idx,idy;
  int start_x,start_y;
  int _w,_h,_sw,_sh;
  int _x,_y;
  
  int i;

  bool xflip,yflip;
  
  SDL_Surface*gfx_piece = NULL;
  SDL_Surface*gfx_piece_new = NULL;
  SDL_Rect _r;

  int addr;
  unsigned char v;

  if(lisp_cmd->Argc==0) { printf("spr no arguments\n");return;}

  if(lisp_cmd->Argc > 2 ) {
    n = CmdArg_GetInt(&lisp_cmd->Args[0]);
    x = CmdArg_GetInt(&lisp_cmd->Args[1]);
    y = CmdArg_GetInt(&lisp_cmd->Args[2]);
  }
  
  if(lisp_cmd->Argc > 3 ) {
    w = CmdArg_GetInt(&lisp_cmd->Args[3]);
  }

  if(lisp_cmd->Argc > 4 ) {
    h = CmdArg_GetInt(&lisp_cmd->Args[4]);
  }
  
  if(lisp_cmd->Argc > 5 ) {
    flip_x = CmdArg_GetInt(&lisp_cmd->Args[5]);
  }

  if(lisp_cmd->Argc > 6 ) {
    flip_y = CmdArg_GetInt(&lisp_cmd->Args[6]);
  }

  idx = n%16;
  idy = n/16;
  
  start_x = idx*8;
  start_y = idy*8;
  
  _w = w*8;
  _h = h*8;
  _sw = _w;
  _sh = _h;
  
  if(start_x >= self->Width || start_y >= self->Height) {
    printf("spr start_x or start_y illegl\n");
    return;
  }
  
  if(start_x +_w > self->Width) {
    _sw = self->Width - start_x;
  }

  if(start_y + _h > self->Height) {
    _sh = self->Height - start_y;
  }
  
  if (_sw == 0 || _sh == 0 ) {
    printf("spr _sw or _sh is zero\n");
    return;
  }

  gfx_piece = SDL_CreateRGBSurface(0, _sw,_sh, 32,0,0,0,0);
  SDL_SetSurfacePalette(gfx_piece,self->Draw_Palette);
  
  SDL_SetColorKey(gfx_piece,SDL_TRUE,0);
  

  for(_x=0;_x<_sw;_x++)
    for(_y=0;_y<_sh;_y++)
    {
      addr = start_x+_x +(start_y+_y)*self->Width;
      v = self->Sprite[addr];
      //printf("spr %d %d ",addr,v);
      if(v>0)
        Pixel(gfx_piece,&self->draw_colors[v],_x,_y);
    }
  

  xflip = false;
  yflip = false;
  
  if(flip_x > 0 ) {
    xflip = true;
  }
  
  if(flip_y > 0 ) {
    yflip = true;
  }
    
  gfx_piece_new = Transform_Flip(gfx_piece,xflip,yflip);
  
  for(i=0;i<16;i++) {
    if(self->PalTransparent[i] == 0) {
      SDL_SetColorKey(gfx_piece_new,SDL_TRUE,i);
    }
  }

  _r = (SDL_Rect){x,y,0,0};
  
  Surface_Blit(self->DrawCanvas,gfx_piece_new,&_r,NULL);
  
  SDL_FreeSurface(gfx_piece);
  SDL_FreeSurface(gfx_piece_new);


}

void Pico8_draw_map(Pico8*self,int n,int x, int  y) {

  int idx=0,idy =0;
  int start_x = 0,start_y = 0;
  int w_=0, h_=0;
  int addr;
  int _x,_y;
  SDL_Surface*gfx_piece=NULL;
  SDL_Rect _r;
  unsigned char v;

  idx = n%16;
  idy = n/16;
  start_x = idx*8;
  start_y = idy*8;
  
  w_ = 8;
  h_ = 8;
  
  gfx_piece = SDL_CreateRGBSurface(0, w_,h_, 32,0,0,0,0);
  SDL_SetSurfacePalette(gfx_piece,self->Draw_Palette);
  SDL_SetColorKey(gfx_piece,SDL_TRUE,0);
  
  for(_x=0;_x<w_;_x++)
    for(_y=0;_y<h_;_y++)
    {
      addr = start_x+_x +(start_y+_y)*self->Width;
      v = self->Sprite[addr];
      Pixel(gfx_piece,&self->draw_colors[v],_x,_y);
    }
  
  
  _r = (SDL_Rect){x,y,0,0};
  Surface_Blit(self->DrawCanvas,gfx_piece,&_r,NULL);
  SDL_FreeSurface(gfx_piece);
  
}

void Pico8_Map(Pico8*self,LispCmd*lisp_cmd) {
  int cel_x,cel_y,sx,sy,cel_w,cel_h,bitmask;
  int addr;
  cel_x = cel_y = sx = sy = cel_w = cel_h = bitmask= 0;
  
  int x,y;
  unsigned char v;

  if(lisp_cmd->Argc==0) {
    printf("Map has no arguments\n");
    return;
  }

  if(lisp_cmd->Argc > 0 ){
    cel_x =  CmdArg_GetInt(&lisp_cmd->Args[0]);
  }
  if(lisp_cmd->Argc > 1 ){
    cel_y =  CmdArg_GetInt(&lisp_cmd->Args[1]);
  }

  if(lisp_cmd->Argc > 2 ){ 
    sx = CmdArg_GetInt(&lisp_cmd->Args[2]);
  }
  if(lisp_cmd->Argc > 3 ){ 
    sy = CmdArg_GetInt(&lisp_cmd->Args[3]);
  }

  if(lisp_cmd->Argc > 4 ){
    cel_w =  CmdArg_GetInt(&lisp_cmd->Args[4]);
  }  
  if(lisp_cmd->Argc > 5 ){
    cel_h =  CmdArg_GetInt(&lisp_cmd->Args[5]);
  }

  if(lisp_cmd->Argc > 6 ){
    bitmask =  CmdArg_GetInt(&lisp_cmd->Args[6]);
  }
  
  //printf("\n\n");
  for(y=0;y<cel_h;y++) {
    for(x=0;x<cel_w;x++) {
      addr = cel_y + y +(cel_x+x)*64;
      if(addr < 8192) {
        v = self->Map[addr];
        //printf(" %d==%d ",addr,v);
        if (v > 0 ) {
          if(bitmask == 0 ) {
            Pico8_draw_map(self,v,sx+x*8,sy+y*8);
          }else {
            if( (self->SpriteFlags[v] & bitmask) != 0) {
              Pico8_draw_map(self,v,sx+x*8,sy+y*8);
            }
          }
        }else {
          //printf("v is <=0 %d\n",v);
        }
      }else {
        printf("addr >= 8192,exceeds %d\n",addr);
      }
    }
  }
  
  //printf("\n");
}

int Pico8_Color(Pico8*self,LispCmd*lisp_cmd) {
  int p;
  if(lisp_cmd->Argc==0) {
    return self->PenColor;
  }
  
  p = CmdArg_GetInt(&lisp_cmd->Args[0]);
  
  if(p< 0) { return self->PenColor;}
  if (p < 16 && p >= 0 ) {
    self->PenColor = p;
  }
  
  return self->PenColor;

}

int Pico8_set_color(Pico8*self,int p) {
  
  if(p == -1) {
    return self->PenColor;
  }

  if(p< 0) { return self->PenColor;}
  if (p < 16 && p >= 0 ) {
    self->PenColor = p;
  }
  
  return self->PenColor;

}


void Pico8_Flip(Pico8*self,LispCmd*lisp_cmd) {
  SDL_Rect blit_rect;
  int window_w,window_h;
  int bigger_border;
  int _blit_x,_blit_y;

  SDL_Rect stretchRect;
  stretchRect.x = 0;
  stretchRect.y = 0;
  
  if(self->HWND != NULL) {
    window_w = self->HWND->w;
    window_h = self->HWND->h;
    
    blit_rect= (SDL_Rect){self->CameraDx,self->CameraDy,0,0};
    SDL_SetSurfacePalette(self->DisplayCanvas,self->DisplayPalette);
    
    Surface_Blit(self->DisplayCanvas,self->DrawCanvas,&blit_rect,NULL);
    
    if (window_w > self->Width && window_h > self->Height) {
      bigger_border = window_w;
      if(bigger_border > window_h) {
        bigger_border = window_h;
      }
      _blit_x = (window_w - bigger_border)/2;
      _blit_y = (window_h - bigger_border)/2;

      stretchRect.x = _blit_x;
      stretchRect.y = _blit_y;
      stretchRect.w = bigger_border;
      stretchRect.h = bigger_border;
      
      
      if( SDL_BlitScaled( self->DisplayCanvas, NULL, self->HWND, &stretchRect) != 0 ){
        printf("Scale blit error %s\n",SDL_GetError());
      }
      
    }else {
      Surface_Blit(self->HWND,self->DisplayCanvas,&stretchRect,NULL);
    }
  
    self->CameraDx=0;
    self->CameraDy=0;
    

  
  }
  
  self->frames+=1;
  self->curr_time = SDL_GetTicks();
  if ( (self->curr_time - self->prev_time) > 10000 ) {
    int fps = self->frames/10;
    printf("pico8 fps is %d\n",fps);
    self->frames=0;
    self->prev_time= self->curr_time;
  
  }


}


void Pico8_Circ(Pico8*self,LispCmd*lisp_cmd) {
  
  int ox,oy,r,col;
  col = -1;

  if( lisp_cmd->Argc < 3 ) {
    return;
  }
  
  if(lisp_cmd->Argc > 2) {
    ox = CmdArg_GetInt(&lisp_cmd->Args[0]);
    oy = CmdArg_GetInt(&lisp_cmd->Args[1]);
    r  = CmdArg_GetInt(&lisp_cmd->Args[2]);
  }
  
  if(lisp_cmd->Argc > 3) {
    col = CmdArg_GetInt(&lisp_cmd->Args[3]);
  }
  
  Pico8_set_color(self,col);

  Draw_Circle(self->DrawCanvas, &self->draw_colors[self->PenColor],ox,oy,r,1);

}


void Pico8_Circfill(Pico8*self,LispCmd *lisp_cmd) {
  int cx,cy,r,col;
  col = -1;
  if(lisp_cmd->Argc <  3 ) {
    return;
  }
  
  if( lisp_cmd->Argc > 2 ) {
    cx = CmdArg_GetInt(&lisp_cmd->Args[0]);
    cy = CmdArg_GetInt(&lisp_cmd->Args[1]);
    r  = CmdArg_GetInt(&lisp_cmd->Args[2]);
  }
  
  if(lisp_cmd->Argc > 3) {
    col = CmdArg_GetInt(&lisp_cmd->Args[3]);
  }
  
  Pico8_set_color(self,col);
  
  Draw_Circle(self->DrawCanvas,&self->draw_colors[self->PenColor],cx,cy,r,0);

}







void Pico8_Rectfill(Pico8*self,LispCmd*lisp_cmd) {
  int x0,y0,x1,y1,col;
  int w,h;
  SDL_Rect rect_;

  x0=y0=x1=y1=0;
  col = -1;
  
  if(lisp_cmd->Argc < 4) {
    return;
  }
  
  if(lisp_cmd->Argc > 3) {
    x0 = CmdArg_GetInt(&lisp_cmd->Args[0]);
    y0 = CmdArg_GetInt(&lisp_cmd->Args[1]);
    x1 = CmdArg_GetInt(&lisp_cmd->Args[2]);
    y1 = CmdArg_GetInt(&lisp_cmd->Args[3]);
  }

  if(lisp_cmd->Argc > 4) {
    col = CmdArg_GetInt(&lisp_cmd->Args[4]);
  }

  Pico8_set_color(self,col);
  
  w = (x1-x0)+1;
  h = (y1-y0)+1;

  if(w < 0 ) {
    w = -w;
    x0 = x0-w;
  }
  if(h<0) {
    h = -h;
    y0 = y0-h;
  }
  
  rect_ = (SDL_Rect){x0,y0,w,h};
  
  Draw_Rect(self->DrawCanvas,&self->draw_colors[self->PenColor], &rect_,0);

}

void Pico8_Rect(Pico8*self,LispCmd*lisp_cmd) {
  int x0,y0,x1,y1,col;
  SDL_Rect rect_;

  col = -1;
  
  if(lisp_cmd->Argc < 4 ) {
    return;
  }

  if(lisp_cmd->Argc > 3) {
    x0 = CmdArg_GetInt(&lisp_cmd->Args[0]);
    y0 = CmdArg_GetInt(&lisp_cmd->Args[1]);
    x1 = CmdArg_GetInt(&lisp_cmd->Args[2]);
    y1 = CmdArg_GetInt(&lisp_cmd->Args[3]);
  }
  if(lisp_cmd->Argc > 4) {
    col = CmdArg_GetInt(&lisp_cmd->Args[4]);
  }

  
  Pico8_set_color(self,col);
  
  rect_ = (SDL_Rect){x0+1,y0+1,x1-x0,y1-y0};
  
  Draw_Rect(self->DrawCanvas,&self->draw_colors[self->PenColor], &rect_,1);

}


void Pico8_Palt(Pico8*self,LispCmd*lisp_cmd) {
  int c,t;
  int i;

  if(lisp_cmd->Argc == 0 ) {
    for(i=0;i<16;i++) {
      if(i==0) {
        self->PalTransparent[i] = 0;
      }else{
        self->PalTransparent[i] = 1;
      }
    }
  }

  if(lisp_cmd->Argc == 2 ) {
    c = CmdArg_GetInt(&lisp_cmd->Args[0]);
    t = CmdArg_GetInt(&lisp_cmd->Args[1]);
    c = c % 16;
    if(t == 1) {
      self->PalTransparent[c] = 0;
    }else {
      self->PalTransparent[c] = 1;
    }
  }
}

void Pico8_set_palt(Pico8*self,int c,int t) {
  int i;
  
  if (c == NONE && t == NONE ) {
    for(i=0;i<16;i++) {
      if(i==0) {
        self->PalTransparent[i] = 0;
      }else{
        self->PalTransparent[i] = 1;
      }
    }
  }
    
  c = c % 16;
  if(t == 1) {
    self->PalTransparent[c] = 0;
  }else {
    self->PalTransparent[c] = 1;
  }
    
}

void Pico8_Pal(Pico8*self,LispCmd*lisp_cmd) {
  int c0,c1,p;
  int i;

  if(lisp_cmd->Argc == 0 ) {
    if(self->PaletteModified == false) { 
      return;
    }
    
    for(i=0;i<16;i++) {
      self->DrawPaletteIdx[i] = i;
      self->display_colors[i] = self->pal_colors[i];
      self->draw_colors[i]    = self->pal_colors[i];
    }

    SDL_SetPaletteColors(self->Draw_Palette, self->draw_colors,0,16);
    SDL_SetPaletteColors(self->DisplayPalette, self->display_colors,0,16);
    
    Pico8_set_palt(self,NONE,NONE);
    Pico8_sync_draw_pal(self);
    
    SDL_SetSurfacePalette(self->DisplayCanvas,self->DisplayPalette);
    SDL_SetSurfacePalette(self->DrawCanvas,self->Draw_Palette);

    self->PaletteModified = false;
    
  }else if(lisp_cmd->Argc == 3 ) {
    c0 = CmdArg_GetInt(&lisp_cmd->Args[0]);
    c1 = CmdArg_GetInt(&lisp_cmd->Args[1]);
    p =  CmdArg_GetInt(&lisp_cmd->Args[2]);
    c0 = c0 % 16;
    c1 = c1 % 16;  
    
    if(p==1) {
      self->display_colors[c0] = self->pal_colors[c1];
      self->PaletteModified = true;
      SDL_SetPaletteColors(self->DisplayPalette, self->display_colors,0,16);
      SDL_SetSurfacePalette(self->DisplayCanvas,self->DisplayPalette);
    }else if (p == 0 ) {
      
      self->DrawPaletteIdx[c0] = c1;
      self->PaletteModified  = true;
      Pico8_sync_draw_pal(self);
      SDL_SetSurfacePalette(self->DrawCanvas,self->Draw_Palette);
    }
  }

}

void Pico8_Print(Pico8*self,LispCmd*lisp_cmd) {
  char *text=NULL;
  int x,y,c;
  SDL_Rect rect_;
  
  x = self->Cursor[0];
  y = self->Cursor[1];
  c =1;
  
  
  if(lisp_cmd->Argc ==0 || lisp_cmd->Argc==2) {
    return;
  }
  
  if(lisp_cmd->Argc > 0) {
    text = CmdArg_GetStr(&lisp_cmd->Args[0]);
    self->Cursor[1]+=6;
  }
  if(lisp_cmd->Argc > 2) {
    x = CmdArg_GetInt(&lisp_cmd->Args[1]);
    y = CmdArg_GetInt(&lisp_cmd->Args[2]);
    self->Cursor[0] = x;
    self->Cursor[1] = y;
  }
  
  if(lisp_cmd->Argc > 3) {
    c = CmdArg_GetInt(&lisp_cmd->Args[3]);
  }
  
  Pico8_set_color(self,c);
  
  SDL_Surface*imgText = Font_Render(self->Font,text,false,&self->draw_colors[self->DrawPaletteIdx[self->PenColor]],NULL);
  SDL_SetColorKey(imgText,SDL_TRUE,0);
  
  rect_ = (SDL_Rect){x,y,0,0};
  Surface_Blit(self->DrawCanvas,imgText,&rect_,NULL);
  SDL_FreeSurface(imgText);
  
}
