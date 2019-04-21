#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>

#include "types.h"

SDL_Surface* Transform_Flip(SDL_Surface*src_surf,bool xflip,bool yflip);


#endif
