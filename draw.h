#ifndef DRAW_H
#define DRAW_H

#include <stdio.h>
#include <stdlib.h>

#include <SDL.h>

#include "types.h"

bool Pixel(SDL_Surface*surf,SDL_Color*c, int x,int y);

void Draw_Rect(SDL_Surface*surf,SDL_Color *color,SDL_Rect*_rect,int border_width );


void draw_fillpoly(SDL_Surface*surf, int*vx, int*vy, int numpoints, SDL_Color*col);
SDL_Rect Polygon(SDL_Surface*surf, SDL_Color*color,int**points,int points_number, int border_width);




void draw_ellipse(SDL_Surface*dst,int x,int y,int rx,int ry ,SDL_Color*color);
void draw_fillellipse(SDL_Surface*dst,int x,int y,int rx,int ry,SDL_Color*color);

void Draw_Circle(SDL_Surface*surf,SDL_Color*color, int x,int y,int radius,int border_width);


#endif
