#include "types.h"
#include "utils.h"
#include "surface.h"

SDL_Rect Surface_Blit(SDL_Surface*dst, SDL_Surface*source,SDL_Rect*dest,SDL_Rect*area) {
  int dx,dy;
  SDL_Rect dest_rect;

  if(dest==NULL) {
    dx = 0;
    dy = 0;
  }else {
    dx = dest->x;
    dy = dest->y;
  }

  dest_rect = (SDL_Rect){0,0,0,0};
  dest_rect.x = dx;
  dest_rect.y = dy;
  
  dest_rect.w = source->w;
  dest_rect.h = source->h;
  
  if(area==NULL) {
    dest_rect.w = area->w;
    dest_rect.h = area->h;
  }

  if ( SDL_BlitSurface(source,area,dst, &dest_rect) != 0 ){
    printf("Surface_Blit error %s\n",SDL_GetError());
    return (SDL_Rect){0,0,0,0};
  }
  
  return dest_rect;
}
