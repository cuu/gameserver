#include "types.h"
#include "utils.h"
#include "surface.h"


int
SDL_UpperBlit2(SDL_Surface * src, const SDL_Rect * srcrect,
              SDL_Surface * dst, SDL_Rect * dstrect)
{
  int srcx, srcy, w, h;
  SDL_Rect fulldst;
  if (!src || !dst) {
      return SDL_SetError("SDL_UpperBlit2: passed a NULL surface");
  }    
  if (src->locked || dst->locked) {
      return SDL_SetError("Surfaces must not be locked during blit");
  }
  
  /* If the destination rectangle is NULL, use the entire dest surface */
  if (dstrect == NULL) {
      fulldst.x = fulldst.y = 0; 
      fulldst.w = dst->w;
      fulldst.h = dst->h;
      dstrect = &fulldst;
  }
    /* clip the source rectangle to the source surface */
    if (srcrect) {
        int maxw, maxh;

        srcx = srcrect->x;
        w = srcrect->w;
        if (srcx < 0) {
            w += srcx;
            dstrect->x -= srcx;
            srcx = 0;
        }
        maxw = src->w - srcx;
        if (maxw < w)
            w = maxw;

        srcy = srcrect->y;
        h = srcrect->h;
        if (srcy < 0) {
            h += srcy;
            dstrect->y -= srcy;
            srcy = 0;
        }
        maxh = src->h - srcy;
        if (maxh < h)
            h = maxh;

    } else {
        srcx = srcy = 0;
        w = src->w;
        h = src->h;
    }
    /* clip the destination rectangle against the clip rectangle */
    {   
        SDL_Rect *clip = &dst->clip_rect;
        int dx, dy;

        dx = clip->x - dstrect->x;
        if (dx > 0) {
            w -= dx;
            dstrect->x += dx;
            srcx += dx;
        }
        dx = dstrect->x + w - clip->x - clip->w;
        if (dx > 0)
            w -= dx;

        dy = clip->y - dstrect->y;
        if (dy > 0) {
            h -= dy;
            dstrect->y += dy;
            srcy += dy;
        }
        dy = dstrect->y + h - clip->y - clip->h;
        if (dy > 0)
            h -= dy;
    }

    if (w > 0 && h > 0) {
        SDL_Rect sr;
        sr.x = srcx;
        sr.y = srcy;
        sr.w = dstrect->w = w;
        sr.h = dstrect->h = h;
        
        return SDL_LowerBlit(src, &sr, dst, dstrect);
    }
  
  dstrect->w = dstrect->h = 0;
  
  return 0;
}

SDL_Rect Surface_Vlaue_Blit(SDL_Surface*dst, SDL_Surface*source,SDL_Rect*dest,SDL_Rect*area) {
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
  
  if(area !=NULL) {
    dest_rect.w = area->w;
    dest_rect.h = area->h;
  }

  if ( SDL_BlitSurface(source,area,dst, &dest_rect) != 0 ){
      

    printf("Surface_Blit error %s\n",SDL_GetError());
    return (SDL_Rect){0,0,0,0};
  }
  
  return dest_rect;
}


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
  
  if(area !=NULL) {
    dest_rect.w = area->w;
    dest_rect.h = area->h;
  }

  if ( SDL_BlitSurface(source,area,dst, &dest_rect) != 0 ){
    printf("Surface_Blit error %s\n",SDL_GetError());
    return (SDL_Rect){0,0,0,0};
  }
  
  return dest_rect;
}

SDL_Surface* NewSurfaceNoMask(int w,int h,int depth) {
  SDL_Surface*surf=NULL;

  Uint32 rmask, gmask, bmask, amask;

  rmask=0;
  gmask=0;
  bmask=0;
  amask=0;

  surf = SDL_CreateRGBSurface(0,w,h,depth, rmask,gmask,bmask,amask);
  
  return surf;
}

SDL_Surface* NewSurface(int w,int h,int depth) {
  SDL_Surface*surf=NULL;
  Uint32 rmask, gmask, bmask, amask;

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
rmask = 0xff000000;
gmask = 0x00ff0000;
bmask = 0x0000ff00;
amask = 0x000000ff;
#else
rmask = 0x000000ff;
gmask = 0x0000ff00;
bmask = 0x00ff0000;
amask = 0xff000000;
#endif

  surf = SDL_CreateRGBSurface(0,w,h,depth, rmask,gmask,bmask,amask);
  
  return surf;

}
