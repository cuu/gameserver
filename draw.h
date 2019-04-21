#ifndef DRAW_H
#define DRAW_H

#include <stdio.h>
#include <stdlib.h>

#include <SDL.h>


int Pixel(SDL_Surface*surf,SDL_Color*c, int x,int y);

void Draw_Rect(SDL_Surface*surf,SDL_Color *color,SDL_Rect*_rect,int border_width );




#endif
