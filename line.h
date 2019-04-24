#ifndef LINE_H
#define LINE_H

#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>

#include "types.h"



#define LEFT_EDGE   0x1
#define RIGHT_EDGE  0x2
#define BOTTOM_EDGE 0x4
#define TOP_EDGE    0x8


SDL_Rect Lines(SDL_Surface *surf,SDL_Color *col,bool closed, Point*pointlist,int length,int width);
SDL_Rect Line(SDL_Surface*surf,SDL_Color*col,int x1,int y1,int x2,int y2,int width );


int clip_and_draw_line_width(SDL_Surface*surf,SDL_Rect*rect,SDL_Color*col,int width, int *pts);
int clip_and_draw_line(SDL_Surface*surf, SDL_Rect*rect, SDL_Color*col, int*pts);

int clipline( int *pts, int left,int top,int right, int bottom);

void drawline(SDL_Surface*surf, SDL_Color*col, int x1,int y1,int x2, int y2);
void drawhorzline(SDL_Surface*surf, SDL_Color*col, int x1,int y1,int x2);
void drawhorzlineclip( SDL_Surface*surf, SDL_Color*col, int x1 ,int  y1, int x2 );

void drawvertline(SDL_Surface*surf, SDL_Color*col, int x1,int y1,int y2 );
void drawvertlineclip(SDL_Surface*surf, SDL_Color*col,int  x1, int y1, int y2);



#endif
