#ifndef SURFACE_H
#define SURFACE_H

#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>

SDL_Rect Surface_Blit(SDL_Surface*dst, SDL_Surface*source,SDL_Rect*dest,SDL_Rect*area);

SDL_Surface* NewSurface(int w,int h,int depth);

SDL_Surface* NewSurfaceNoMask(int w,int h,int depth);


#endif
