#include "types.h"
#include "utils.h"
#include "transform.h"

SDL_Surface* newsurf_fromsurf(SDL_Surface*surf, int width,int height) {
  SDL_Surface *newsurf=NULL;
  int key = 0;
  unsigned char alpha=255;
  
  if( surf->format->BytesPerPixel <= 0 || surf->format->BytesPerPixel > 4 ) {
    printf("unsupport Surface bit depth for transform");
    return newsurf;
  }
  
  newsurf = SDL_CreateRGBSurface(0, width,height, surf->format->BitsPerPixel,surf->format->Rmask,surf->format->Gmask,surf->format->Bmask,surf->format->Amask);
  
  if(newsurf == NULL) {
    panic( "newsurf_fromsurf failed %s",  SDL_GetError());
  }

  if(surf->format->BytesPerPixel == 1 && surf->format->palette != NULL) {
    SDL_SetSurfacePalette(newsurf,surf->format->palette);
  }

  
  if( SDL_GetColorKey(surf,&key) == 0) {
    SDL_SetColorKey(newsurf,SDL_TRUE,key);
  }
  
  if( SDL_GetSurfaceAlphaMod(surf,&alpha)== 0 ){
    SDL_SetSurfaceAlphaMod(newsurf,alpha);
  }

  return newsurf;
  
}

SDL_Surface* Transform_Flip(SDL_Surface*src_surf,bool xflip,bool yflip) {
  SDL_Surface*newsurf=NULL;
  int pixsize=0;
  unsigned char*srcpix=NULL;
  unsigned char*dstpix=NULL;
  int _pitch = 0;
  
  int loopx,loopy;
  int srcaddr,dstaddr;
  int u;

  if(src_surf==NULL) {
    return src_surf;
  }
  
	newsurf = newsurf_fromsurf(src_surf, src_surf->w,src_surf->h);

	if(newsurf ->w != src_surf->w || newsurf->h != src_surf->h ){
		panic("Flip destination surface create failed");
	}
  
  pixsize = src_surf->format->BytesPerPixel;
  
  //SDL_LockSurface(newsurf); SDL_LockSurface(src_surf);
  srcpix = (unsigned char*)src_surf->pixels;
  dstpix = (unsigned char*)newsurf->pixels;
  
  if(xflip==false) {
    if(yflip==false) { // x and y not flip
      memcpy(dstpix,srcpix, src_surf->w*src_surf->h*pixsize);
    }else{ // only yflip
      for(loopy=0;loopy<src_surf->h;loopy++){
        _pitch = src_surf->w*pixsize;
        for(loopx=0;loopx<_pitch;loopx++){
          dstpix[loopx+loopy*_pitch] = srcpix[loopx + (src_surf->h -1 -loopy)*_pitch];
        }
      }
    }
    
  }else { //xflip
    if(yflip==true){ // x and y both flip
      switch(pixsize) {
        case 1:
        case 2:
        case 3:
        case 4:
          for(loopy=0;loopy<src_surf->h;loopy++) {
            dstaddr = loopy*newsurf->w;
            srcaddr = ((src_surf->h-1-loopy)*src_surf->w)+src_surf->w-1;
            for(loopx=0;loopx<src_surf->w;loopx++){
              for(u=0;u<pixsize;u++){
                dstpix[dstaddr*pixsize+u] = srcpix[srcaddr*pixsize+u];
              }
              dstaddr+=1;
              srcaddr-=1;
            }
          }
        
      }
    }else { //only xflip,y not flip
      switch(pixsize) {
        case 1:
        case 2:
        case 3:
        case 4:
          for(loopy=0;loopy<src_surf->h;loopy++) {
            dstaddr = loopy*newsurf->w;
            srcaddr = loopy*src_surf->w+src_surf->w-1;
            for(loopx=0;loopx < src_surf->w;loopx++) {
              for(u=0;u<pixsize;u++) {
                dstpix[dstaddr*pixsize+u] = srcpix[srcaddr*pixsize+u];
              }
              dstaddr+=1;
              srcaddr-=1;
            }
          }
      }
    }
  }
  
  // SDL_UnlockSurface(src_surf); SDL_UnlockSurface(newsurf);

  return newsurf;
}
