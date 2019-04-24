#include "types.h"
#include "utils.h"
#include "line.h"
#include "draw.h"

bool PixelValue(SDL_Surface*surf,SDL_Color *c, int x,int y) {
  unsigned char*pixels= NULL;
  int bytes_per_pixel=0;
  int addr;

  pixels = (unsigned char*)surf->pixels;
  
  bytes_per_pixel = surf->format->BytesPerPixel;
  addr = y*surf->pitch+x*bytes_per_pixel;
  
	if(x < surf->clip_rect.x || x >= surf->clip_rect.x + surf->clip_rect.w || y < surf->clip_rect.y || y >= surf->clip_rect.y + surf->clip_rect.h) {
    printf("Pixels coord error\n");
		return false;
	}
  
  if (bytes_per_pixel == 1) {
		pixels[addr] = c->r;
	}

  else if (bytes_per_pixel == 2) {
		pixels[addr]   = c->r;
    pixels[addr+1] = c->r;
	} 
  
  else if (bytes_per_pixel == 3) {
		pixels[addr]   = c->r;
    pixels[addr+1] = c->r;
    pixels[addr+2] = c->r;
	}
  
  else if (bytes_per_pixel == 4) {
		pixels[addr]   = c->r;
    pixels[addr+1] = c->r;
    pixels[addr+2] = c->r;
    pixels[addr+3] = c->r;
	}

  return true;
}

bool Pixel(SDL_Surface*surf,SDL_Color *c, int x,int y) {
  unsigned char*pixels= NULL;
  int bytes_per_pixel=0;
  int addr;

  pixels = (unsigned char*)surf->pixels;
  
  bytes_per_pixel = surf->format->BytesPerPixel;
  addr = y*surf->pitch+x*bytes_per_pixel;
  
	if(x < surf->clip_rect.x || x >= surf->clip_rect.x + surf->clip_rect.w || y < surf->clip_rect.y || y >= surf->clip_rect.y + surf->clip_rect.h) {
    printf("Pixels coord error\n");
		return false;
	}
  
  if (bytes_per_pixel == 1) {
		pixels[addr] = c->r;
	}

  else if (bytes_per_pixel == 2) {
		pixels[addr]   = c->r;
    pixels[addr+1] = c->g;
	} 
  
  else if (bytes_per_pixel == 3) {
		pixels[addr]   = c->r;
    pixels[addr+1] = c->g;
    pixels[addr+2] = c->b;
	}
  
  else if (bytes_per_pixel == 4) {
		pixels[addr]   = c->r;
    pixels[addr+1] = c->g;
    pixels[addr+2] = c->b;
    pixels[addr+3] = c->a;
	}

  return true;
}

void Draw_Rect(SDL_Surface*surf,SDL_Color *color,SDL_Rect*_rect,int border_width ) {
  int l,r,t,b;
  Point points[4];
  
  l = _rect->x;
  r = _rect->x + _rect->w -1;
  t = _rect->y;
  b = _rect->y + _rect->h -1;

  //4x2
  
  points[0].x = l;
  points[0].y = t;

  points[1].x = r;
  points[1].y = t;

  points[2].x = r;
  points[2].y = b;

  points[3].x = l;
  points[3].y = b;

  Polygon(surf,color,&points,4,border_width);

}

static int compare_int(const void *a, const void *b)
{
    return (*(const int *)a) - (*(const int *)b);
}

void draw_fillpoly(SDL_Surface*surf, int*vx, int*vy, int numpoints, SDL_Color*col) {
  int i,j;
	int miny=0;
	int maxy=0;
  int minx,maxx;
	int y=0;
	int x1=0;
	int y1=0;
	int x2=0;
	int y2=0;
	int ints = 0;
	int ind1=0;
	int ind2=0;
	
	int *polyints;
  polyints = (int*)malloc(numpoints*sizeof(int));
	
	/* Determine Y maxima */
	miny = vy[0];
	maxy = vy[0];
	for (i=1; i < numpoints; i++) {
		miny = MIN(miny,vy[i]);
		maxy = MAX(maxy,vy[i]);
	}

	/* Draw, scanning y */
	for (y=miny;y<=maxy;y++) {
		ints = 0;
		for (i=0; i< numpoints;i++ ){
			if (i == 0) {
				ind1 = numpoints -1;
				ind2 = 0;
			}else {
				ind1 = i-1;
				ind2 = i;
			}

			y1 = vy[ind1];
			y2 = vy[ind2];
			if (y1 < y2) {
				x1 = vx[ind1];
				x2 = vx[ind2];
			}else if(y1 > y2) {
				y2 = vy[ind1];
				y1 = vy[ind2];
				x2 = vx[ind1];
				x1 = vx[ind2];
			}else if (miny == maxy) {
				/* Special case: polygon only 1 pixel high. */
				minx= vx[0];
				maxx= vx[0];
				
				for (j = 1; j < numpoints; j++) {
					minx = MIN(minx,vx[j]);
					maxx = MAX(maxx,vx[j]);
				}
				polyints[ints] = minx;
				ints+=1;
				polyints[ints] = maxx;
				ints+=1;
				break;
			}else {
				continue;
			}
			
			if ( y >= y1 && y < y2 ) {
//				fmt.Println("ints : ",ints)
				polyints[ints] = (y-y1) * (x2-x1) / (y2-y1) + x1;
				ints+=1;
				
			}else if ( (y == maxy) && (y > y1) && (y <= y2) ){
				polyints[ints] = (y-y1) * (x2-x1) / (y2-y1) + x1;
				ints+=1;
			}
		}
    
    qsort(polyints, ints, sizeof(int), compare_int); 

		for( i=0;i<ints;i+=2 ){
			drawhorzlineclip(surf, col, polyints[i], y, polyints[i+1]);
		}
	}
  
  free(polyints);
}


SDL_Rect Polygon(SDL_Surface*surf, SDL_Color*color,Point*points,int points_number, int border_width) {
  
  int bytes_per_pixel;
  Point *item;
  int *xlist,*ylist;
  int x,y;
  int left,right,top,bottom;
  int numpoints;
  int loop;

  SDL_Rect ret;

  if(border_width > 0) {
    ret = Lines(surf,color,true,points,points_number,border_width);
    return ret;
  }
  
  bytes_per_pixel = surf->format->BytesPerPixel;
  if (bytes_per_pixel <= 0 || bytes_per_pixel > 4) {
    panic("unsupport bit depth for line draw");
  }
  
  if(points_number < 3 ) {
    panic("points argument must contain more than 2 points");
  }
  
  item = &points[0];// assume length of item >=2
  x = item->x;
  y = item->y;

  left   = x;
	right  = x;
	top    = y;
	bottom = y;
  
  xlist = (int*)malloc( points_number *sizeof(int));
  ylist = (int*)malloc( points_number *sizeof(int));
  
  
  numpoints = 0;
  
  for(loop=0;loop<points_number;loop++) {
    item = &points[loop];
    x = item->x;
    y = item->y;
    
    xlist[numpoints] = x;
    ylist[numpoints] = y;
    numpoints+=1;
    left   = MIN(x,left);
    top    = MIN(y,top);
    right  = MAX(x,right);
    bottom = MAX(y,bottom);
  }
  
  draw_fillpoly(surf,xlist,ylist,numpoints,color);

	left = MAX(left,surf->clip_rect.x);
	top  = MAX(top, surf->clip_rect.y);
	right = MIN(right,surf->clip_rect.x + surf->clip_rect.w);
	bottom = MIN(bottom, surf->clip_rect.y + surf->clip_rect.h);
  
  free(xlist);
  free(ylist);
	return (SDL_Rect){left,top,right-left+1, bottom-top+1};

}




void draw_ellipse(SDL_Surface*dst,int x,int y,int rx,int ry ,SDL_Color*color) {

  if (rx == 0 && ry == 0 ){
    Pixel(dst,color,x,y);
    return;
  }
  
  if( rx == 0) { /* Special case for rx=0 - draw a vline */
    drawvertlineclip(dst,color,x,y-ry,y+ry);
    return;
  }
  
  if (ry == 0 ){ /* Special case for ry=0 - draw a hline */
    drawhorzlineclip(dst,color,x-rx,y,x+rx);
    return;
  }
  
  int oh,oi,oj,ok;
  oh =0xffff;
  oi = 0xffff;
  oj = 0xffff;
  ok = 0xffff;
  
  int ix, iy;
  int h, i, j, k;
  int xmh, xph, ypk, ymk;
  int xmi, xpi, ymj, ypj;
  int xmj, xpj, ymi, ypi;
  int xmk, xpk, ymh, yph;
  
  if(rx > ry) {
    ix = 0;
    iy = rx * 64;
    for(;;) {
      h = (ix + 16) >> 6;
      i = (iy + 16) >> 6;
      j = (h * ry) / rx;
      k = (i * ry) / rx;
      
            if ( ((ok!=k) && (oj!=k)) || ((oj!=j) && (ok!=j)) || (k!=j) ) {
                xph=x+h-1;
                xmh=x-h;
                if (k>0) {
                    ypk=y+k-1;
                    ymk=y-k;
                    if (h > 0) {
                        Pixel(dst, color,xmh, ypk);
                        Pixel(dst, color,xmh, ymk);
                    }
                    Pixel(dst,color,xph, ypk);
                    Pixel(dst,color,xph, ymk);
                }
                ok=k;
                xpi=x+i-1;
                xmi=x-i;
                if (j>0) {
                    ypj=y+j-1;
                    ymj=y-j;
                    Pixel(dst,color, xmi, ypj);
                    Pixel(dst,color, xpi, ypj);
                    Pixel(dst,color, xmi, ymj);
                    Pixel(dst,color, xpi, ymj);
                }
                oj=j;
            }
            ix = ix + iy / rx;
            iy = iy - ix / rx;     
      
      
      if (i<=h ){
        break;
      }
    }
  }else {
  
        ix = 0;
        iy = ry * 64;
        for(;;) {
            h = (ix + 32) >> 6;
            i = (iy + 32) >> 6;
            j = (h * rx) / ry;
            k = (i * rx) / ry;

            if ( ((oi!=i) && (oh!=i)) || ((oh!=h) && (oi!=h) && (i!=h)) ) {
                xmj=x-j;
                xpj=x+j-1;
                if (i>0 ){
                    ypi=y+i-1;
                    ymi=y-i;
                    if (j > 0) {
                        Pixel(dst,color, xmj, ypi);
                        Pixel(dst,color, xmj, ymi);
                    }
                    Pixel(dst, color,xpj, ypi);
                    Pixel(dst, color,xpj, ymi);
                }
                oi=i;
                xmk=x-k;
                xpk=x+k-1;
                if (h>0) {
                    yph=y+h-1;
                    ymh=y-h;
                    Pixel(dst, color,xmk, yph);
                    Pixel(dst,color,xpk, yph);
                    Pixel(dst,color,xmk, ymh);
                    Pixel(dst,color,xpk, ymh);
                }
                oh=h;
            }
            ix = ix + iy / ry;
            iy = iy - ix / ry;
         
          if (i <= h) {
            break;
          }
  
        }
  }
}


void draw_fillellipse(SDL_Surface*dst,int x,int y,int rx,int ry,SDL_Color*color) {
    int ix, iy;
    int h, i, j, k;
    int oh, oi, oj, ok;
    
    if (rx ==0 && ry == 0) {
      Pixel(dst,color,x,y);
      return;
    }
    
    if (rx == 0) {
      drawvertlineclip( dst, color, x, y-ry, y+ry);
      return;
    }
    
    if (ry == 0) {
      drawhorzlineclip( dst, color, x-rx, y, x+rx);
      return;
    }
    
    oh = 0xffff;
    oh = 0xffff;
    oj = 0xffff;
    ok = 0xffff;
    
    if (rx >= ry) {
        ix = 0;
        iy = rx * 64;

        for(;;) {
            h = (ix + 8) >> 6;
            i = (iy + 8) >> 6;
            j = (h * ry) / rx;
            k = (i * ry) / rx;
            if( (ok!=k) && (oj!=k) && (k<ry) ) {
                drawhorzlineclip(dst, color, x-h, y-k-1, x+h-1);
                drawhorzlineclip(dst, color, x-h, y+k, x+h-1);
                ok=k;
            }
            if ( (oj!=j) && (ok!=j) && (k!=j) ) {
                drawhorzlineclip(dst, color, x-i, y+j, x+i-1);
                drawhorzlineclip(dst, color, x-i, y-j-1, x+i-1);
                oj=j;
            }
            ix = ix + iy / rx;
            iy = iy - ix / rx;

            if (i <= h ) {
              break;
            }
        } 
    } else {
        ix = 0; 
        iy = ry * 64;

        for(;;) {
            h = (ix + 8) >> 6;
            i = (iy + 8) >> 6;
            j = (h * rx) / ry;
            k = (i * rx) / ry;

            if ( (oi!=i) && (oh!=i) && (i<ry) ) {
                drawhorzlineclip(dst, color, x-j, y+i, x+j-1);
                drawhorzlineclip(dst, color, x-j, y-i-1, x+j-1);
                oi=i;
            }
            if ( (oh!=h) && (oi!=h) && (i!=h) ){
                drawhorzlineclip(dst, color, x-k, y+h, x+k-1);
                drawhorzlineclip(dst, color, x-k, y-h-1, x+k-1);
                oh=h;
            }

            ix = ix + iy / ry;
            iy = iy - ix / ry;
            
            if (i <= h) {
              break;
            }
        }
    }
}

void Draw_Circle(SDL_Surface*surf,SDL_Color*color, int x,int y,int radius,int border_width) {
  
  if (radius< 0) {
    printf("Circle negative radius\n");
    return;
  }
  
  if(border_width < 0) {
    printf("Circle negative border width\n");
    return;
  }
  
  if(border_width > radius) {
    printf("Circle border width greater than radius\n");
    return;
  }
  
  if(border_width == 0) {
    draw_fillellipse(surf,x,y,radius,radius,color);
  }else {
    for(int loop = 0;loop < border_width;loop++) {
      draw_ellipse(surf,x,y,radius-loop,radius-loop,color);
            /* To avoid moiré pattern. Don't do an extra one on the outer ellipse.
               We draw another ellipse offset by a pixel, over drawing the missed
               spots in the filled circle caused by which pixels are filled.
            */
      if (border_width > 1 && loop > 0) {
        draw_ellipse(surf,x+1,y,radius-loop,radius-loop,color);
      }
    }
  }
}




