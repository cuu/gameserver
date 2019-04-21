#include "types.h"
#include "utils.h"
#include "line.h"
#include "draw.h"

bool Pixel(SDL_Surface*surf,SDL_Color *c, int x,int y) {
  unsigned char*pixels= NULL;
  int bytes_per_pixels=0;
  int addr;

  pixels = (unsigned char*)surf->pixels;
  
  bytes_per_pixels = surf->format->BytesPerPixel;
  addr = y*surf->pitch+x*bytes_per_pixels;
  
	if(x < surf->clip_rect.x || x >= surf->clip_rect.x + surf->clip_rect.w || y < surf->clip_rect.y || y >= surf->clip_rect.y + surf->clip_rect.h) {
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
  int points[4][2];
  
  l = _rect->x;
  r = _rect->x + _rect->w -1;
  t = _rect->y;
  b = _rect->y + _rect_>h -1;

  //4x2
  
  points[0][0] = l;
  points[0][1] = t;

  points[1][0] = r;
  points[1][1] = t;

  points[2][0] = r;
  points[2][1] = b;

  points[3][0] = l;
  points[3][1] = b;

  Polygon(surf,color,points,4,border_width);

}

static int compare_int(const void *a, const void *b)
{
    return (*(const int *)a) - (*(const int *)b);
}

func draw_fillpoly(SDL_Surface*surf, int*vx, int*vy, int numpoints, SDL_Color*col) {
  int i,j;
	int miny=0;
	int maxy=0;
  int minx,miny;
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
	for (i:=1; i < numpoints; i++) {
		miny = MIN(miny,vy[i]);
		maxy = MAX(maxy,vy[i]);
	}

	/* Draw, scanning y */
	for (y=miny;y<=maxy;y++) {
		ints = 0;
		for (i:=0; i< numpoints;i++ ){
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

		for i:=0;i<ints;i+=2 {
			drawhorzlineclip(surf, col, polyints[i], y, polyints[i+1])
		}
	}
  
  free(polyints);
}


SDL_Rect Polygon(SDL_Surface*surf, SDL_Color*color,int**points,int points_number, int border_width) {
  
  int bytes_per_pixel;
  int *item;
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
  if (bytes_per_pixel <= 0 o|| bytes_per_pixel > 4) {
    panic("unsupport bit depth for line draw");
  }
  
  if(points_number < 3 ) {
    panic("points argument must contain more than 2 points");
  }
  
  item = &points[0];// assume length of item >=2
  x = item[0];
  y = item[1];

  left   = x;
	right  = x;
	top    = y;
	bottom = y;
  
  xlist = (int*)malloc( points_number *sizeof(int));
  ylist = (int*)malloc( points_number *sizeof(int));
  
  
  numpoints = 0;
  
  for(loop=0;loop<points_number;loop++) {
    item = &points[loop];
    x = item[0];
    y = item[1];
    
    xlist[numpoints] = x;
    ylist[numpoints] = y;
    numpoints+=1;
    left   = MAX(x,left)
    top    = MAX(y,top)
    right  = MAX(x,right)
    bottom = MAX(y,bottom)    
  }
  
  draw_fillpoly(surf,xlist,ylist,numpoints,color);

	left = MAX(left,surf->clip_rect.x);
	top  = MAX(top, surf->clip_rect.y);
	right = MIN(right,surf->clip_rect.x + surf->clip_rect.w);
	bottom = MIN(bottom, surf->clip_rect.Y + surf->clip_rect.h);
  
  free(xlist);
  free(ylist);
	return (SDL_Rect){left,top,right-left+1, bottom-top+1};

}



