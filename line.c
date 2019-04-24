#include "types.h"
#include "utils.h"
#include "draw.h"
#include "line.h"


static int encode(int x,int y,int left, int top,int right,int bottom ) {
	int code = 0;

	if (x < left ) {
		code |= LEFT_EDGE;
	}
	if (x > right) {
		code |= RIGHT_EDGE;
	}
	if (y < top)   {
		code |= TOP_EDGE;
	}
	if (y > bottom) {
		code |= BOTTOM_EDGE;
	}
	return code;
}

static bool inside(int a) {
	if (a > 0) {
		return false;
	}//<=0;
	return true;
}

static bool accept(int a,int b ) {
  int ret;
  ret = a | b;
  if (ret > 0 ) {
  return false;
  }

  return true;

}

static bool reject(int a,int b )  {
	int ret ;
  ret = a & b;
	if (ret > 0) {
		return true;
	}
	return false;
}

//length == pointlist length;
SDL_Rect Lines(SDL_Surface *surf,SDL_Color *col,bool closed, Point*pointlist,int length,int width) {
  int x,y;
  int startx,starty,left,right;
  int top,bottom;
  int drawn;
  int loop;
  
  Point *item;

  if(length < 2) {
    panic("draw lines at least contains more than 1 points pair" );
  
  }
  
  int pts[4];
  item = &pointlist[0];
  x = item->x;
  y = item->y;
  
  startx  = x;
  pts[0]  = x;
  left    = x;
  right   = x;

  starty  = y;
  pts[1]  = y;
  top     = y;
  bottom  = y;

  if (width < 1) {
    return (SDL_Rect){x,y,0,0};
  }
  
  drawn = 1;
  
  for(loop=1;loop < length;loop++) {
    item = &pointlist[loop];
    
    x = item->x;
    y = item->y;
    drawn +=1;
    pts[0] = startx;
    pts[1] = starty;
    startx = x;
    starty = y;
    pts[2] = x;
    pts[3] = y;   
    if( clip_and_draw_line_width(surf, &surf->clip_rect, col, width, pts) > 0 ) {
      left = MIN(MIN(pts[0],pts[2]),left);
      top  =  MIN(MIN(pts[1],pts[3]),top);
      right =  MAX(MAX(pts[0],pts[2]),right);
      bottom = MAX(MAX(pts[1],pts[3]),bottom);
    }
  }  
  
  if(closed==true && drawn > 2 ) {
    item = &pointlist[0];
    x = item->x;
    y = item->y;
    
		pts[0] = startx;
		pts[1] = starty;
		pts[2] = x;
		pts[3] = y;
		clip_and_draw_line_width(surf, &surf->clip_rect, col, width, pts);
  
  }


	return (SDL_Rect){left,top,right-left+1, bottom-top+1};

}

SDL_Rect Line(SDL_Surface*surf,SDL_Color*col,int x1,int y1,int x2,int y2,int width ) {
  
  int pts[4];
  int anydraw;
  int rleft,rtop,rwidth,rheight;
  int dx,dy;

	pts[0] = x1;
	pts[1] = y1;
	pts[2] = x2;
	pts[3] = y2; 

  if(width< 1) {
    return (SDL_Rect){x1,y1,0,0};
  }

  anydraw = clip_and_draw_line_width(surf,&surf->clip_rect, col, width,pts);

  if(anydraw==0) {
    return (SDL_Rect){x1,y1,0,0};
  }

  rleft = 0;
  rtop = 0;
  
  if (x1 < x2 ) {
    rleft = x1;
  }else {
    rleft = x2;
  }
  
  if(y1 < y2) {
    rtop = y1;
  }else {
    rtop = y2;
  }

  dx = abs(x1-x2);
  dy = abs(y1-y2); 

  rwidth = 0;
  rheight = 0;
  if (dx > dy) {
    rwidth = dx +1;
    rheight = dy + width;
  }else {
    rwidth = dx + width;
    rheight = dy + 1;
  }  

  return (SDL_Rect){rleft,rtop,rwidth,rheight};

}



int clip_and_draw_line_width(SDL_Surface*surf,SDL_Rect*rect,SDL_Color*col,int width, int *pts) { //pts is int[4]
  int loop,xinc,yinc;
  int newpts[4];
  int range[4];
  int anydraw=0;
  
  if( abs(pts[0]-pts[2]) > abs(pts[1]-pts[3]) ) {
    yinc = 1;
  }else{
    xinc = 1;
  }
  
  memcpy(newpts,pts,4*sizeof(int));
  
  if (clip_and_draw_line(surf,rect,col,newpts) > 0) {
    anydraw=1;
    memcpy(range,newpts,4*sizeof(int));
  }else {
    range[0] = 10000;
    range[1] = 10000;
    range[2] = -10000;
    range[3] = -10000;
  }
  
	for(loop = 1; loop < width; loop +=2) {
		newpts[0] = pts[0] + xinc*(loop/2+1);
		newpts[1] = pts[1] + yinc*(loop/2+1);
		newpts[2] = pts[2] + xinc*(loop/2+1);
		newpts[3] = pts[3] + yinc*(loop/2+1);
		if(clip_and_draw_line(surf,rect,col,newpts) > 0) {
			anydraw = 1;
			range[0] = MIN(newpts[0],range[0]);
			range[1] = MIN(newpts[1],range[1]);
			range[2] = MAX(newpts[2],range[2]);
			range[3] = MAX(newpts[3],range[3]);
		}
		if((loop + 1) < width) {
			newpts[0] = pts[0] - xinc*(loop/2+1);
			newpts[1] = pts[1] - yinc*(loop/2+1);
			newpts[2] = pts[2] - xinc*(loop/2+1);
			newpts[3] = pts[3] - yinc*(loop/2+1);
			if(clip_and_draw_line(surf,rect,col, newpts) > 0) {
				anydraw = 1;
				range[0] = MIN(newpts[0],range[0]);
				range[1] = MIN(newpts[1],range[1]);
				range[2] = MAX(newpts[2],range[2]);
				range[3] = MAX(newpts[3],range[3]);
			}
		}
	}  
  if(anydraw > 0) {
    memcpy(pts,range,4*sizeof(int));
  }

  return anydraw;
  
}




int clip_and_draw_line(SDL_Surface*surf, SDL_Rect*rect, SDL_Color*col, int*pts)  {

  if ( clipline(pts, rect->x,rect->y,rect->x+ rect->w-1, rect->y+rect->h-1 ) == 0 ){
    return 0;
  }

  if (pts[1] == pts[3]) {
    drawhorzline(surf, col, pts[0],pts[1],pts[2]);
  }else if( pts[0] == pts[2] ) {
    drawvertline(surf,col, pts[0],pts[1],pts[3]);
  }else {
    drawline(surf, col, pts[0],pts[1],pts[2],pts[3]);
  }

  return 1;
}

int clipline( int *pts, int left,int top,int right, int bottom) {

  int x1,y1,x2,y2;

	x1 = pts[0];
	y1 = pts[1];
	x2 = pts[2];
	y2 = pts[3];
	
	int code1;
	int code2;

	int draw = 0;

  int swaptmp;
	float m;  /*slope*/

	for(;;) {
		code1 = encode(x1,y1,left,top,right,bottom);
		code2 = encode(x2,y2,left,top,right,bottom);
		if ( accept(code1,code2) ) {
			draw = 1;
			break;
		} else if ( reject(code1,code2 ) ) {
			break;
		}else {
			if (inside(code1) ) {
				swaptmp = x2;
				x2 = x1;
				x1 = swaptmp;
				swaptmp = y2;
				y2 = y1;
				y1 = swaptmp;
				swaptmp = code2;
				code2 = code1;
				code1 = swaptmp;
			}
			if (x2 != x1) {
				m = ( (float)y2 - (float)y1 ) / (  (float)x2-(float)x1  );
			}else {
				m = 1.0;
			}
			if ((code1 & LEFT_EDGE) > 0 ) {
				y1 += (int)((  (float)left-(float)x1  )*m);
				x1 = left;
			}else if ((code1 & RIGHT_EDGE) > 0) {
				y1 += (int)(  (float)(right-x1) * m);
				x1 = right;
			}else if ( (code1 & BOTTOM_EDGE) > 0 ){
				if (x2 != x1) {
					x1 += (int)( (float)(bottom-y1) / m);
				}
				y1 = bottom;
			}else if ( (code1 & TOP_EDGE) > 0 ) {
				if (x2 != x1) {
					x1 += (int)( (float)(top-y1) / m);
				}
				y1 = top;
			}
		}
	}

	if (draw > 0) {
		pts[0] = x1;
		pts[1] = y1;
		pts[2] = x2;
		pts[3] = y2;
	}
	
	return draw;
}


void  drawline(SDL_Surface*surf, SDL_Color*col, int x1,int y1,int x2, int y2) {
  int deltax,deltay;
  int signx,signy;
  int bytes_per_pixel;
  int pixx,pixy;
  int addr;
  int swaptmp;
  int x,y;

  unsigned char*pixels;

	deltax = x2 - x1;
	deltay = y2 - y1;

	signx = 0;
	signy = 0;
	
	if (deltax < 0) {
		signx = -1;
	}else {
		signx = 1;
	}

	if(deltay < 0) {
		signy = -1;
	}else {
		signy = 1;
	}

	deltax = signx * deltax + 1;
	deltay = signy * deltay + 1;

	bytes_per_pixel = surf->format->BytesPerPixel;

	pixx = bytes_per_pixel;
	pixy = surf->pitch;

	addr = pixy* y1 + x1 * bytes_per_pixel;

	pixx *= signx;
	pixy *= signy;

  swaptmp = 0;
	if (deltax < deltay) {
		swaptmp = deltax;
		deltax = deltay;
		deltay = swaptmp;
		swaptmp = pixx;
		pixx = pixy;
		pixy = swaptmp;
	}

	x = 0;  y = 0;

	
	pixels = (unsigned char*)surf->pixels;
	
	switch(bytes_per_pixel) {
    case 1:
      for(; x < deltax; x++) {
        pixels[addr] = col->r;
        y += deltay;
        if( y >= deltax ) {
          y -= deltax;
          addr += pixy;
        }
        addr +=pixx;
      }
      break;
    case 2:
      for(; x < deltax;x++ ) {
        pixels[addr] = col->r;
        pixels[addr+1] =col->g;
        y+= deltay;
        if (y >= deltax) {
          y -= deltax;
          addr += pixy;
        }
        addr+=pixx;
      }
      break;
    case 3:
      for (; x < deltax; x++ ){
        pixels[addr] = col->r;
        pixels[addr+1] = col->g;
        pixels[addr+2] = col->b;
        y+=deltay;
        if(y >= deltax) {
          y-=deltax;
          addr += pixy;
        }
        addr+=pixx;
      }
      break;
    case 4:
      for (; x < deltax; x++ ){
        pixels[addr] = col->r;
        pixels[addr+1] = col->g;
        pixels[addr+2] = col->b;
        pixels[addr+3] = col->a;
        y+=deltay;
        if(y >= deltax) {
          y-=deltax;
          addr += pixy;
        }
        addr+=pixx;
      }
      break;
  }
}

void drawhorzline(SDL_Surface*surf, SDL_Color*col, int x1,int y1,int x2) {
  int bytes_per_pixel;
  unsigned char*pixels;
  int addr;
  int start,end;
  
	if (x1 == x2) {
		Pixel(surf,col,x1,y1);
		return;
	}
	
	bytes_per_pixel = surf->format->BytesPerPixel;



	pixels = (unsigned char*)surf->pixels;

	addr = surf->pitch * y1;
	end  = 0;
	start = 0;

	if (x1 < x2) {
		end   = addr + x2*bytes_per_pixel;
		start = addr+x1 *bytes_per_pixel;
	}else {
		end  = addr + x1 *bytes_per_pixel;
		start = addr + x2 * bytes_per_pixel;
	}

	switch(bytes_per_pixel) {
	case 1:
		for (; start <=end; start++) {
			pixels[start] = col->r;
		}
  break;
	case 2:
		for( ; start <= end; start+=2 ){
			pixels[start] = col->r;
			pixels[start+1] = col->g;
		}
  break;
	case 3:
		for (; start <= end; start+=3) {
			pixels[start] = col->r;
			pixels[start+1] = col->g;
			pixels[start+2] = col->b;
		}
  break;
	case 4:
		for (; start <= end; start +=4) {
			pixels[start] = col->r;
			pixels[start+1] = col->g;
			pixels[start+2] = col->b;
			pixels[start+3] = col->a;
		}
  break;
	}

}

void  drawhorzlineclip( SDL_Surface*surf, SDL_Color*col, int x1 ,int  y1, int x2 ) {
  
  int temp;

	if( y1 < surf->clip_rect.y || y1 >= surf->clip_rect.y + surf->clip_rect.h) {
		return;
	}

	if(x2 < x1) {
		temp = x1;
		x1 = x2;
		x2 = temp;
	}

	x1 = MAX(x1,surf->clip_rect.x);
	x2 = MIN(x2,surf->clip_rect.x+surf->clip_rect.w-1);
	if (x2 < surf->clip_rect.x || x1 >= surf->clip_rect.x + surf->clip_rect.w) {
		return;
	}

	if (x1 == x2) {
		Pixel(surf,col, x1,y1);
	}else {
		drawhorzline(surf,col,x1,y1,x2);
	}
}

void drawvertline(SDL_Surface*surf, SDL_Color*col, int x1,int y1,int y2 ) {

  int bytes_per_pixel;
  unsigned char*pixels;
  int pitch;
  int addr;
  int start,end;

	if( y1 == y2 ){
		Pixel(surf,col, x1,y1);
	}

  bytes_per_pixel = surf->format->BytesPerPixel;

	pixels = surf->pixels;
	pitch  = surf->pitch;
	
	addr  = x1 * bytes_per_pixel;
	end   = 0;
	start = 0;
	if (y1 < y2) {
		end = addr + y2* pitch;
		start = addr + y1*pitch;
	}else {
		end = addr + y1*pitch;
		start = addr + y2*pitch;
	}

	switch(bytes_per_pixel) {
	case 1:
		for( ; start <=end; start+=pitch ){
			pixels[start] = col->r;
		}
  break;
	case 2:
		for (; start <= end; start+=pitch); {
			pixels[start] = col->r;
			pixels[start+1] = col->g;
		}
  break;
	case 3:
		for( ; start <= end; start+=pitch ){
			pixels[start] = col->r;
			pixels[start+1] = col->g;
			pixels[start+2] = col->b;
		}
  break;
	case 4:
		for (; start <= end; start +=pitch ){
			pixels[start] = col->r;
			pixels[start+1] = col->g;
			pixels[start+2] = col->b;
			pixels[start+3] = col->a;
		}
  break;
	}
	
}

void drawvertlineclip(SDL_Surface*surf, SDL_Color*col,int  x1, int y1, int y2) {
  
  int temp;

  if (x1 < surf->clip_rect.x || x1 >= surf->clip_rect.x + surf->clip_rect.w) {
    return;
  }

  if (y2 < y1) {
    temp = y1;
    y1 = y2;
    y2 = temp;
  }
  
  y1 = MAX(y1,surf->clip_rect.y);
  y2 = MIN(y2,surf->clip_rect.y + surf->clip_rect.h-1);
  
  if (y2 -y1 < 1){
    Pixel(surf,col,x1,y1);
  }else{
    drawvertline(surf,col,x1,y1,y2);
  }
}

