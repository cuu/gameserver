#ifndef DRAW_H
#define DRAW_H

#include <stdio.h>
#include <stdlib.h>

#include <SDL.h>

#include "types.h"

bool Pixel(SDL_Surface*surf,SDL_Color*c, int x,int y);

void Draw_Rect(SDL_Surface*surf,SDL_Color *color,SDL_Rect*_rect,int border_width );

SDL_Rect Polygon(SDL_Surface*surf, SDL_Color*color,int**points,int points_number, int border_width);



#endif
