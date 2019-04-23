#ifndef FONT_H
#define FONT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <SDL.h>
#include <SDL_ttf.h>

#include "types.h"
#include "utils.h"

SDL_Surface* Font_Render(TTF_Font*fnt,char*text,bool antialias,SDL_Color*col, SDL_Color*background);


#endif
