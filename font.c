#include "font.h"

SDL_Surface* Font_Render(TTF_Font*fnt,char*text,bool antialias,SDL_Color*col, SDL_Color*background) {
  
  int height;
  int c;
  SDL_Surface*surf;
  
  bool just_return;
  just_return=false;
  
  if(strlen(text) == 0) {
    just_return=true;
  }
  
  if(just_return==true) {
    height = TTF_FontHeight(fnt);
    surf = SDL_CreateRGBSurface(0, 1,height,32,0xff<<16,0xff<<8,0xff,0);
    if(background!=NULL) {
      c = SDL_MapRGB(surf->format,background->r,background->g,background->b);
      SDL_FillRect(surf,NULL,c);
    }else {
      SDL_SetColorKey(surf,SDL_TRUE,0);
    }
    return surf;
  }
  
  if(antialias==true) {
    if(background!=NULL){
      surf = TTF_RenderUTF8_Shaded(fnt,text, *col, *background);
      if(surf==NULL) { 
        panic("font render failed %s\n",TTF_GetError());
      }
    }else {
      surf = TTF_RenderUTF8_Blended(fnt,text, *col);
      if(surf==NULL) {
        panic("font render failed %s\n",TTF_GetError());
      }
    }
    
  }else {
    surf = TTF_RenderUTF8_Solid(fnt,text,*col);
    if(surf==NULL) {
      panic("font render failed %s\n",TTF_GetError());
    }
  }
  
  if( antialias == false && background !=NULL && just_return==false) {
    //turn off transparancy
    SDL_SetColorKey(surf,SDL_FALSE,0);// false to disable colokey transparancy
    if(surf->format->palette!=NULL) {
        if(surf->format->palette->ncolors > 0) {
          surf->format->palette->colors[0].r = background->r;
          surf->format->palette->colors[0].g = background->g;
          surf->format->palette->colors[0].b = background->b;
        }
    }
  }
  
  return surf;

}
