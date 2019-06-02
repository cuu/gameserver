#include "pico8.h"
#include "draw.h"
#include "utils.h"
#include "lisp_parser.h"
#include "transform.h"
#include "surface.h"
#include "osc.h"

#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alext.h>

pico8_sfx pico8_sfx_data[64];
pico8_music pico8_music_data[64];
pico8_channel pico8_audio_channels[4];

osc_func Osc[9] = {&osc_tri,  &osc_uneven_tri,      &osc_saw,
									&osc_sqr,   &osc_pulse,           &osc_organ,
									NULL,       &osc_detuned_tri,     &osc_saw_lfo};


Pico8 *NewPico8() {
    SDL_Surface *surface;
    int i;

    Pico8 *p = NULL;
    p = (Pico8 *) malloc(sizeof(Pico8));

    p->Width = 128;
    p->Height = 128;
    p->Version = 8;

    /*
    p->pal_colors[0]  = (SDL_Color){0,0,0,255};
    p->pal_colors[1]  = (SDL_Color){29,43,83,255};
    p->pal_colors[2]  = (SDL_Color){126,37,83,255};
    p->pal_colors[3]  = (SDL_Color){0,135,81,255};
    p->pal_colors[4]  = (SDL_Color){171,82,54,255};
    p->pal_colors[5]  = (SDL_Color){95,87,79,255};
    p->pal_colors[6]  = (SDL_Color){194,195,199,255};
    p->pal_colors[7]  = (SDL_Color){255,241,232,255};
    p->pal_colors[8]  = (SDL_Color){255,0,77,255};
    p->pal_colors[9]  = (SDL_Color){255,163,0,255};
    p->pal_colors[10] = (SDL_Color){255,240,36,255};
    p->pal_colors[11] = (SDL_Color){0,231,86,255};
    p->pal_colors[12] = (SDL_Color){41,173,255,255};
    p->pal_colors[13] = (SDL_Color){131,118,156,255};
    p->pal_colors[14] = (SDL_Color){255,119,168,255};
    p->pal_colors[15] = (SDL_Color){255,204,170,255};
    */

    p->pal_colors[0] = (SDL_Color) {0, 0, 0, 255};
    p->pal_colors[1] = (SDL_Color) {83, 43, 29, 255};
    p->pal_colors[2] = (SDL_Color) {83, 37, 126, 255};
    p->pal_colors[3] = (SDL_Color) {81, 135, 0, 255};
    p->pal_colors[4] = (SDL_Color) {54, 82, 171, 255};
    p->pal_colors[5] = (SDL_Color) {79, 87, 95, 255};
    p->pal_colors[6] = (SDL_Color) {199, 195, 194, 255};
    p->pal_colors[7] = (SDL_Color) {232, 241, 255, 255};
    p->pal_colors[8] = (SDL_Color) {77, 0, 255, 255};
    p->pal_colors[9] = (SDL_Color) {0, 163, 255, 255};
    p->pal_colors[10] = (SDL_Color) {36, 240, 255, 255};
    p->pal_colors[11] = (SDL_Color) {86, 231, 0, 255};
    p->pal_colors[12] = (SDL_Color) {255, 173, 41, 255};
    p->pal_colors[13] = (SDL_Color) {156, 118, 131, 255};
    p->pal_colors[14] = (SDL_Color) {168, 119, 255, 255};
    p->pal_colors[15] = (SDL_Color) {170, 204, 255, 255};

    surface = NewSurfaceNoMask(p->Width, p->Height, 32);
    if (surface == NULL) {
        SDL_Log("SDL_CreateRGBSurface() failed: %s", SDL_GetError());
        exit(1);
    }
    p->DisplayCanvas = surface;

    surface = NewSurfaceNoMask(p->Width, p->Height, 32);
    if (surface == NULL) {
        SDL_Log("SDL_CreateRGBSurface() failed: %s", SDL_GetError());
        exit(1);
    }

    p->DrawCanvas = surface;

    surface = NewSurfaceNoMask(p->Width, p->Height, 32);
    if (surface == NULL) {
        SDL_Log("SDL_CreateRGBSurface() failed: %s", SDL_GetError());
        exit(1);
    }

    p->GfxSurface = surface;

    for (i = 0; i < 16; i++) {
        p->draw_colors[i] = p->pal_colors[i];
        p->display_colors[i] = p->pal_colors[i];
    }

    p->Draw_Palette = SDL_AllocPalette(16);
    p->DisplayPalette = SDL_AllocPalette(16);
    // p->Guu_Palette    = SDL_AllocPalette(16);


    SDL_SetPaletteColors(p->Draw_Palette, p->draw_colors, 0, 16);
    SDL_SetPaletteColors(p->DisplayPalette, p->display_colors, 0, 16);


    SDL_SetSurfacePalette(p->DisplayCanvas, p->DisplayPalette);
    SDL_SetSurfacePalette(p->DrawCanvas, p->Draw_Palette);

    for (i = 0; i < 16; i++) {
        p->DrawPaletteIdx[i] = i;
        if (i == 0) {
            p->PalTransparent[i] = 0;
        } else {
            p->PalTransparent[i] = 1;
        }
    }

    p->Font = TTF_OpenFont("PICO-8.ttf", 4);
    if (!p->Font) {
        printf("TTF_OpenFont: %s\n", TTF_GetError());
        exit(-1);
    }

    p->PenColor = 1;

    p->Version = 8;

    p->Uptime = SDL_GetTicks();


    p->res_state = -1;
    p->res_offset = 0;

    p->Cursor[0] = 0;
    p->Cursor[1] = 0;

    // clear resource
    p->Map = (char *) malloc(64 * 128 * sizeof(char));
    p->Sprite = (char *) malloc(128 * 128 * sizeof(char));
    p->SpriteFlags = (char *) malloc(256 * sizeof(char));


    memset(p->Map, 0, 64 * 128); //64*128=8192
    memset(p->Sprite, 0, 128 * 128);
    memset(p->SpriteFlags, 0, 256);


    p->FPS = 60.0;
    p->PaletteModified = false;

    p->sfx_data = pico8_sfx_data;
    p->music_data = pico8_music_data;
    p->audio_channels = pico8_audio_channels;

    for (i = 0; i < 4; i++) {
        p->audio_channels[i].sfx = NIL;

        p->audio_channels[i].noise = new_osc_noise();
        p->audio_channels[i].lfo = new_oldosc_closure(Osc[0]);
        p->audio_channels[i].lastnote = 0;
    }

    p->current_music = NULL;


    p->frames = 0;
    p->curr_time = 0;
    p->prev_time = 0;

    return p;
}

void Pico8_sync_draw_pal(Pico8 *self) {
    int i;
    for (i = 0; i < 16; i++) {
        self->draw_colors[i] = self->pal_colors[self->DrawPaletteIdx[i]];
    }

    SDL_SetPaletteColors(self->Draw_Palette, self->draw_colors, 0, 16);

}

void Pico8_SetGfx(Pico8 *self, char *data) { //data is one line
    int i = 0;
    int col = 0;
    long ret;
    char tmp[2];

    data = trim(data, "\n");

    if (strlen(data) < 3) { return; }

    for (i = 0; i < strlen(data); i++) {
        memset(tmp, 0, 2);
        tmp[0] = data[i];
        ret = strtol(tmp, NULL, 16);
        self->Sprite[col + self->res_offset * 128] = (unsigned char) ret;
        //printf("%ld ",ret);
        col += 1;
        if (col >= 128) {
            break;
        }
    }

    //set shared map
    self->res_offset += 1;
}

void Pico8_SetGff(Pico8 *self, char *data) {
    int i = 0;
    int col = 0;
    long ret;
    char tmp[3];

    data = trim(data, "\n");
    if (self->Version > 2) {
        for (i = 0; i < strlen(data); i += 2) {
            memset(tmp, 0, 3);
            tmp[0] = data[i];
            tmp[1] = data[i + 1];
            ret = strtol(tmp, NULL, 16);
            self->SpriteFlags[self->res_offset] = (unsigned char) ret;
            self->res_offset += 1;
            if (self->res_offset > 255) {
                break;
            }
        }
    }
}

void Pico8_SetMap(Pico8 *self, char *data) { // 32*128, 32*128,two parts
    int i = 0;
    int col = 0;
    long ret;
    char tmp[3];

    for (i = 0; i < strlen(data); i += 2) {
        memset(tmp, 0, 3);
        tmp[0] = data[i];
        tmp[1] = data[i + 1];
        ret = strtol(tmp, NULL, 16);
        //printf("%d ",(unsigned char)ret);
        self->Map[self->res_offset + col * 64] = (unsigned char) ret;
        col++;
        if (col > 127) {
            break;
        }
    }

    //printf("\n");
    self->res_offset += 1;

}

void Pico8_set_shared_map(Pico8 *self) {

    int shared = 0;

    int tx = 0;
    int ty = 32;
    int sx, sy;
    unsigned char lo, hi;
    unsigned char v;
    if (self->Version > 3) {
        for (sy = 64; sy < 128; sy++)
            for (sx = 0; sx < 128; sx += 2) {
                lo = self->Sprite[sx + sy * 128];
                hi = self->Sprite[sx + 1 + sy * 128];
                v = (hi << 4) | lo;
                //printf("%d ",v);
                self->Map[ty + tx * 64] = v;

                shared += 1;
                tx += 1;
                if (tx == 128) {
                    tx = 0;
                    ty += 1;
                }
            }
        printf("Map Shared: %d\n", shared);
    }

}

void Pico8_SetSfx(Pico8 *self, char *line) {

    char tmp[3];
    char tmp_note[6];
    int mark;

    int i, j, u;

    mark = self->res_offset;

    tmp[2] = '\0';
    tmp_note[5] = '\0';
    // 4 byte, 32 of 5 nybbles = 8 + 32*5 = 168 nybbles
    tmp[0] = line[0];
    tmp[1] = line[1];
    self->sfx_data[mark].mode = (uint8_t) strtol(tmp, NULL, 16);
    tmp[0] = line[2];
    tmp[1] = line[3];
    self->sfx_data[mark].duration = (uint8_t) strtol(tmp, NULL, 16);
    tmp[0] = line[4];
    tmp[1] = line[5];
    self->sfx_data[mark].loop_start = (uint8_t) strtol(tmp, NULL, 16);
    tmp[0] = line[6];
    tmp[1] = line[7];
    self->sfx_data[mark].loop_end = (uint8_t) strtol(tmp, NULL, 16);

    for (i = 0; i < 32; i++) {
        j = i * 5 + 4 * 2;
        for (u = 0; u < 5; u++) {
            tmp_note[u] = line[j + u];
        }

        tmp[0] = tmp_note[0];
        tmp[1] = tmp_note[1];
        self->sfx_data[mark].notes[i].pitch = (uint16_t) strtol(tmp, NULL, 16);
        tmp[0] = tmp_note[2];
        tmp[1] = '\0';
        self->sfx_data[mark].notes[i].waveform = (uint8_t) strtol(tmp, NULL, 16);
        tmp[0] = tmp_note[3];
        self->sfx_data[mark].notes[i].volume = (uint8_t) strtol(tmp, NULL, 16);
        tmp[0] = tmp_note[4];
        self->sfx_data[mark].notes[i].effect = (uint8_t) strtol(tmp, NULL, 16);
    }

    self->res_offset += 1;

}

void Pico8_SetMusic(Pico8 *self, char *line) {

    int i, j, u;
    char tmp[3];
    int mark;

    mark = self->res_offset;
    tmp[2] = '\0';

    tmp[0] = line[0];
    tmp[1] = line[1];
    self->music_data[mark].loop_control = (uint8_t) strtol(tmp, NULL, 16);

    for (i = 0; i < 4; i++) {
        j = i * 2 + 3;
        for (u = 0; u < 2; u++) {
            tmp[u] = line[j + u];
        }
        self->music_data[mark].ch_ids[i] = (uint8_t) strtol(tmp, NULL, 16);
    }
    self->res_offset += 1;
}

void Pico8_SetResource(Pico8 *self, char *data) {

    switch (self->res_state) {
        case RES_GFX:
            Pico8_SetGfx(self, data);
            break;
        case RES_GFF:
            Pico8_SetGff(self, data);
            break;
        case RES_MAP:
            Pico8_SetMap(self, data);
            break;
        case RES_SFX:
            Pico8_SetSfx(self, data);
            break;

        case RES_MUSIC:
            Pico8_SetMusic(self, data);
            break;
    }
}

void Pico8_Version(Pico8 *self, LispCmd *lisp_cmd) {

    int tmp;

    if (lisp_cmd->Argc == 0) {
        return;
    }

    tmp = CmdArg_GetInt(&lisp_cmd->Args[0]);
    printf("Set version %d\n", tmp);
    self->Version = tmp;

}

void Pico8_Res(Pico8 *self, LispCmd *lisp_cmd) {

    int tmp;
    tmp = -1;
    self->res_offset = 0;

    tmp = CmdArg_GetInt(&lisp_cmd->Args[0]);

    self->res_state = tmp;

}

void Pico8_ResOver(Pico8 *self, LispCmd *lisp_cmd) {

    self->res_offset = 0;
    self->res_state = -1;
 
    //clean up

}

void Pico8_ResDone(Pico8 *self, LispCmd *lisp_cmd) {

    Pico8_set_shared_map(self);
    printf("res done\n");

/*
 	for(int i=0;i<64;i++){
		printf("%02x%02x%02x%02x",self->sfx_data[i].mode,self->sfx_data[i].duration,self->sfx_data[i].loop_start,self->sfx_data[i].loop_end);
		for(int j=0;j<32;j++) {
			printf("%02x%01x%01x%01x",self->sfx_data[i].notes[j].pitch,
														self->sfx_data[i].notes[j].waveform,
														self->sfx_data[i].notes[j].volume,
														self->sfx_data[i].notes[j].effect);
		}
		printf("\n");
	}	
	
	for(int i=0;i<64;i++) {
		printf("%02x ",self->music_data[i].loop_control);
		for(int j=0;j<4;j++) {
			printf("%02x",self->music_data[i].ch_ids[j]);
		}
		printf("\n");
	}   
*/

}


void Pico8_Cls(Pico8 *self, LispCmd *lisp_cmd) {
    int color_index = 0;
    if (lisp_cmd->Argc == 0) {
        SDL_FillRect(self->DrawCanvas, NULL, SDL_MapRGB(self->DrawCanvas->format, 0, 0, 0));
        return;
    }

    color_index = CmdArg_GetInt(&lisp_cmd->Args[0]);
    if (color_index >= 0 && color_index < 16) {
        SDL_FillRect(self->DrawCanvas, NULL, self->DrawPaletteIdx[color_index]);
    }

    self->Cursor[0] = 0;
    self->Cursor[1] = 0;

}

void Pico8_Spr(Pico8 *self, LispCmd *lisp_cmd) {
    int n, x, y, w, h;
    int flip_x, flip_y;
    int idx, idy;
    int start_x, start_y;
    int _w, _h, _sw, _sh;
    int _x, _y;

    int i;

    bool xflip, yflip;

    SDL_Surface *gfx_piece = NULL;
    SDL_Surface *gfx_piece_new = NULL;
    SDL_Rect _r;

    int addr;
    unsigned char v;

    if (lisp_cmd->Argc == 0) {
        printf("spr no arguments\n");
        return;
    }

    if (lisp_cmd->Argc > 2) {
        n = CmdArg_GetInt(&lisp_cmd->Args[0]);
        x = CmdArg_GetInt(&lisp_cmd->Args[1]);
        y = CmdArg_GetInt(&lisp_cmd->Args[2]);
    }

    if (lisp_cmd->Argc > 3) {
        w = CmdArg_GetInt(&lisp_cmd->Args[3]);
    }

    if (lisp_cmd->Argc > 4) {
        h = CmdArg_GetInt(&lisp_cmd->Args[4]);
    }

    if (lisp_cmd->Argc > 5) {
        flip_x = CmdArg_GetInt(&lisp_cmd->Args[5]);
    }

    if (lisp_cmd->Argc > 6) {
        flip_y = CmdArg_GetInt(&lisp_cmd->Args[6]);
    }

    idx = n % 16;
    idy = n / 16;

    start_x = idx * 8;
    start_y = idy * 8;

    _w = w * 8;
    _h = h * 8;
    _sw = _w;
    _sh = _h;

    if (start_x >= self->Width || start_y >= self->Height) {
        printf("spr start_x or start_y illegl\n");
        return;
    }

    if (start_x + _w > self->Width) {
        _sw = self->Width - start_x;
    }

    if (start_y + _h > self->Height) {
        _sh = self->Height - start_y;
    }

    if (_sw == 0 || _sh == 0) {
        printf("spr _sw or _sh is zero\n");
        return;
    }

    gfx_piece = NewSurfaceNoMask(_sw, _sh, 32);

    SDL_SetSurfacePalette(gfx_piece, self->Draw_Palette);
    SDL_SetColorKey(gfx_piece, SDL_TRUE, SDL_MapRGB(gfx_piece->format, 0, 0, 0));

    Uint32 ColorKey;

    SDL_Color tmp_col;

    /*
    for(i=0;i<16;i++) {
      if(self->PalTransparent[i] == 0) {
        ColorKey = SDL_MapRGB(self->DisplayCanvas->format,self->draw_colors[v].r,self->draw_colors[v].g,self->draw_colors[v].b);
        if(SDL_SetColorKey(gfx_piece,SDL_TRUE,ColorKey) < 0 ) {
          fprintf (stdout, "LoadSprite: Unable to set color key (0x%X), %s\n", ColorKey,SDL_GetError());
        }
      }
    }
    */

    for (_x = 0; _x < _sw; _x++)
        for (_y = 0; _y < _sh; _y++) {
            addr = start_x + _x + (start_y + _y) * self->Width;
            v = self->Sprite[addr];
            //printf("spr %d %d ",addr,v);
            tmp_col = self->draw_colors[v];
            if (self->PalTransparent[v] != 0) {
                Pixel(gfx_piece, &tmp_col, _x, _y);
            }

        }

    xflip = false;
    yflip = false;

    if (flip_x > 0) {
        xflip = true;
    }

    if (flip_y > 0) {
        yflip = true;
    }

    if (xflip == true || yflip == true) {
        gfx_piece_new = Transform_Flip(gfx_piece, xflip, yflip);

        /*
        for(i=0;i<16;i++) {
          if(self->PalTransparent[i] == 0) {
            SDL_SetColorKey(gfx_piece_new,SDL_TRUE,i);
          }
        }
        */
        _r = (SDL_Rect) {x, y, 0, 0};

        Surface_Blit(self->DrawCanvas, gfx_piece_new, &_r, NULL);
        SDL_FreeSurface(gfx_piece_new);
        SDL_FreeSurface(gfx_piece);
    } else {

        _r = (SDL_Rect) {x, y, 0, 0};
        Surface_Blit(self->DrawCanvas, gfx_piece, &_r, NULL);
        SDL_FreeSurface(gfx_piece);
    }

}

void Pico8_draw_map(Pico8 *self, int n, int x, int y) {

    int idx = 0, idy = 0;
    int start_x = 0, start_y = 0;
    int w_ = 0, h_ = 0;
    int addr;
    int _x, _y;
    SDL_Surface *gfx_piece = NULL;
    SDL_Rect _r;
    unsigned char v;

    idx = n % 16;
    idy = n / 16;
    start_x = idx * 8;
    start_y = idy * 8;

    w_ = 8;
    h_ = 8;

    gfx_piece = SDL_CreateRGBSurface(0, w_, h_, 32, 0, 0, 0, 0);
    SDL_SetSurfacePalette(gfx_piece, self->Draw_Palette);
    SDL_SetColorKey(gfx_piece, SDL_TRUE, 0);

    for (_x = 0; _x < w_; _x++)
        for (_y = 0; _y < h_; _y++) {
            addr = start_x + _x + (start_y + _y) * self->Width;
            v = self->Sprite[addr];
            Pixel(gfx_piece, &self->draw_colors[v], _x, _y);
        }


    _r = (SDL_Rect) {x, y, 0, 0};
    Surface_Blit(self->DrawCanvas, gfx_piece, &_r, NULL);
    SDL_FreeSurface(gfx_piece);

}

void Pico8_Map(Pico8 *self, LispCmd *lisp_cmd) {
    int cel_x, cel_y, sx, sy, cel_w, cel_h, bitmask;
    int addr;
    cel_x = cel_y = sx = sy = cel_w = cel_h = bitmask = 0;

    int x, y;
    unsigned char v;

    if (lisp_cmd->Argc == 0) {
        printf("Map has no arguments\n");
        return;
    }

    if (lisp_cmd->Argc > 0) {
        cel_x = CmdArg_GetInt(&lisp_cmd->Args[0]);
    }
    if (lisp_cmd->Argc > 1) {
        cel_y = CmdArg_GetInt(&lisp_cmd->Args[1]);
    }

    if (lisp_cmd->Argc > 2) {
        sx = CmdArg_GetInt(&lisp_cmd->Args[2]);
    }
    if (lisp_cmd->Argc > 3) {
        sy = CmdArg_GetInt(&lisp_cmd->Args[3]);
    }

    if (lisp_cmd->Argc > 4) {
        cel_w = CmdArg_GetInt(&lisp_cmd->Args[4]);
    }
    if (lisp_cmd->Argc > 5) {
        cel_h = CmdArg_GetInt(&lisp_cmd->Args[5]);
    }

    if (lisp_cmd->Argc > 6) {
        bitmask = CmdArg_GetInt(&lisp_cmd->Args[6]);
    }

    //printf("\n\n");
    for (y = 0; y < cel_h; y++) {
        for (x = 0; x < cel_w; x++) {
            addr = cel_y + y + (cel_x + x) * 64;
            if (addr < 8192) {
                v = self->Map[addr];
                //printf(" %d==%d ",addr,v);
                if (v > 0) {
                    if (bitmask == 0) {
                        Pico8_draw_map(self, v, sx + x * 8, sy + y * 8);
                    } else {
                        if ((self->SpriteFlags[v] & bitmask) != 0) {
                            Pico8_draw_map(self, v, sx + x * 8, sy + y * 8);
                        }
                    }
                } else {
                    //printf("v is <=0 %d\n",v);
                }
            } else {
                printf("addr >= 8192,exceeds %d\n", addr);
            }
        }
    }

    //printf("\n");
}

int Pico8_Color(Pico8 *self, LispCmd *lisp_cmd) {
    int p;
    if (lisp_cmd->Argc == 0) {
        return self->PenColor;
    }

    p = CmdArg_GetInt(&lisp_cmd->Args[0]);

    if (p < 0) { return self->PenColor; }
    if (p < 16 && p >= 0) {
        self->PenColor = p;
    }

    return self->PenColor;

}

int Pico8_set_color(Pico8 *self, int p) {

    if (p == -1) {
        return self->PenColor;
    }

    if (p < 0) { return self->PenColor; }
    if (p < 16 && p >= 0) {
        self->PenColor = p;
    }

    return self->PenColor;

}


void Pico8_Flip(Pico8 *self, LispCmd *lisp_cmd) {
    SDL_Rect blit_rect;
    int window_w, window_h;
    int bigger_border;
    int _blit_x, _blit_y;

    SDL_Rect stretchRect;
    stretchRect.x = 0;
    stretchRect.y = 0;

    if (self->HWND != NULL) {
        window_w = self->HWND->w;
        window_h = self->HWND->h;

        blit_rect = (SDL_Rect) {self->CameraDx, self->CameraDy, 0, 0};
        SDL_SetSurfacePalette(self->DisplayCanvas, self->DisplayPalette);

        Surface_Blit(self->DisplayCanvas, self->DrawCanvas, &blit_rect, NULL);

        if (window_w > self->Width && window_h > self->Height) {
            bigger_border = window_w;
            if (bigger_border > window_h) {
                bigger_border = window_h;
            }
            _blit_x = (window_w - bigger_border) / 2;
            _blit_y = (window_h - bigger_border) / 2;


            stretchRect.x = _blit_x;
            stretchRect.y = _blit_y;
            stretchRect.w = bigger_border;
            stretchRect.h = bigger_border;

            if (SDL_BlitScaled(self->DisplayCanvas, NULL, self->HWND, &stretchRect) != 0) {
                printf("Scale blit error %s\n", SDL_GetError());
            }

        } else {
            Surface_Blit(self->HWND, self->DisplayCanvas, &stretchRect, NULL);
        }

        self->CameraDx = 0;
        self->CameraDy = 0;


    }

    self->frames += 1;
    self->curr_time = SDL_GetTicks();
    if ((self->curr_time - self->prev_time) > 10000) {
        int fps = self->frames / 10;
        printf("pico8 fps is %d\n", fps);
        self->frames = 0;
        self->prev_time = self->curr_time;

    }


}


void Pico8_Circ(Pico8 *self, LispCmd *lisp_cmd) {

    int ox, oy, r, col;
    col = -1;

    if (lisp_cmd->Argc < 3) {
        return;
    }

    if (lisp_cmd->Argc > 2) {
        ox = CmdArg_GetInt(&lisp_cmd->Args[0]);
        oy = CmdArg_GetInt(&lisp_cmd->Args[1]);
        r = CmdArg_GetInt(&lisp_cmd->Args[2]);
    }

    if (lisp_cmd->Argc > 3) {
        col = CmdArg_GetInt(&lisp_cmd->Args[3]);
    }

    Pico8_set_color(self, col);

    Draw_Circle(self->DrawCanvas, &self->draw_colors[self->PenColor], ox, oy, r, 1);

}


void Pico8_Circfill(Pico8 *self, LispCmd *lisp_cmd) {
    int cx, cy, r, col;
    col = -1;
    if (lisp_cmd->Argc < 3) {
        return;
    }

    if (lisp_cmd->Argc > 2) {
        cx = CmdArg_GetInt(&lisp_cmd->Args[0]);
        cy = CmdArg_GetInt(&lisp_cmd->Args[1]);
        r = CmdArg_GetInt(&lisp_cmd->Args[2]);
    }

    if (lisp_cmd->Argc > 3) {
        col = CmdArg_GetInt(&lisp_cmd->Args[3]);
    }

    Pico8_set_color(self, col);

    Draw_Circle(self->DrawCanvas, &self->draw_colors[self->PenColor], cx, cy, r, 0);

}


void Pico8_Rectfill(Pico8 *self, LispCmd *lisp_cmd) {
    int x0, y0, x1, y1, col;
    int w, h;
    SDL_Rect rect_;

    x0 = y0 = x1 = y1 = 0;
    col = -1;

    if (lisp_cmd->Argc < 4) {
        return;
    }

    if (lisp_cmd->Argc > 3) {
        x0 = CmdArg_GetInt(&lisp_cmd->Args[0]);
        y0 = CmdArg_GetInt(&lisp_cmd->Args[1]);
        x1 = CmdArg_GetInt(&lisp_cmd->Args[2]);
        y1 = CmdArg_GetInt(&lisp_cmd->Args[3]);
    }

    if (lisp_cmd->Argc > 4) {
        col = CmdArg_GetInt(&lisp_cmd->Args[4]);
    }

    Pico8_set_color(self, col);

    w = (x1 - x0) + 1;
    h = (y1 - y0) + 1;

    if (w < 0) {
        w = -w;
        x0 = x0 - w;
    }
    if (h < 0) {
        h = -h;
        y0 = y0 - h;
    }

    rect_ = (SDL_Rect) {x0, y0, w, h};

    Draw_Rect(self->DrawCanvas, &self->draw_colors[self->PenColor], &rect_, 0);
    /*
    SDL_Color *draw_color = &self->draw_colors[self->PenColor];

    SDL_FillRect(self->DrawCanvas, &rect_,SDL_MapRGBA(self->DrawCanvas->format,
                draw_color->r,draw_color->g,draw_color->b,draw_color->a));
    */
}

void Pico8_Rect(Pico8 *self, LispCmd *lisp_cmd) {
    int x0, y0, x1, y1, col;
    SDL_Rect rect_;

    col = -1;

    if (lisp_cmd->Argc < 4) {
        return;
    }

    if (lisp_cmd->Argc > 3) {
        x0 = CmdArg_GetInt(&lisp_cmd->Args[0]);
        y0 = CmdArg_GetInt(&lisp_cmd->Args[1]);
        x1 = CmdArg_GetInt(&lisp_cmd->Args[2]);
        y1 = CmdArg_GetInt(&lisp_cmd->Args[3]);
    }
    if (lisp_cmd->Argc > 4) {
        col = CmdArg_GetInt(&lisp_cmd->Args[4]);
    }


    Pico8_set_color(self, col);

    rect_ = (SDL_Rect) {x0 + 1, y0 + 1, x1 - x0, y1 - y0};

    Draw_Rect(self->DrawCanvas, &self->draw_colors[self->PenColor], &rect_, 1);

}


void Pico8_Palt(Pico8 *self, LispCmd *lisp_cmd) {
    int c, t;
    int i;

    if (lisp_cmd->Argc == 0) {
        for (i = 0; i < 16; i++) {
            if (i == 0) {
                self->PalTransparent[i] = 0;
            } else {
                self->PalTransparent[i] = 1;
            }
        }
    }

    if (lisp_cmd->Argc == 2) {
        c = CmdArg_GetInt(&lisp_cmd->Args[0]);
        t = CmdArg_GetInt(&lisp_cmd->Args[1]);
        c = c % 16;
        if (t == 1) {
            self->PalTransparent[c] = 0;
        } else {
            self->PalTransparent[c] = 1;
        }
    }
}

void Pico8_set_palt(Pico8 *self, int c, int t) {
    int i;

    if (c == NONE && t == NONE) {
        for (i = 0; i < 16; i++) {
            if (i == 0) {
                self->PalTransparent[i] = 0;
            } else {
                self->PalTransparent[i] = 1;
            }
        }
        return;
    }

    c = c % 16;
    if (t == 1) {
        self->PalTransparent[c] = 0;
    } else {
        self->PalTransparent[c] = 1;
    }

}

void Pico8_Pal(Pico8 *self, LispCmd *lisp_cmd) {
    int c0, c1, p;
    int i;

    if (lisp_cmd->Argc == 0) {
        if (self->PaletteModified == false) {
            return;
        }

        for (i = 0; i < 16; i++) {
            self->DrawPaletteIdx[i] = i;
            self->display_colors[i] = self->pal_colors[i];
            self->draw_colors[i] = self->pal_colors[i];
        }

        SDL_SetPaletteColors(self->Draw_Palette, self->draw_colors, 0, 16);
        SDL_SetPaletteColors(self->DisplayPalette, self->display_colors, 0, 16);

        Pico8_set_palt(self, NONE, NONE);
        Pico8_sync_draw_pal(self);

        SDL_SetSurfacePalette(self->DisplayCanvas, self->DisplayPalette);
        SDL_SetSurfacePalette(self->DrawCanvas, self->Draw_Palette);

        self->PaletteModified = false;

    } else if (lisp_cmd->Argc == 3) {
        c0 = CmdArg_GetInt(&lisp_cmd->Args[0]);
        c1 = CmdArg_GetInt(&lisp_cmd->Args[1]);
        p = CmdArg_GetInt(&lisp_cmd->Args[2]);
        c0 = c0 % 16;
        c1 = c1 % 16;

        if (p == 1) {
            self->display_colors[c0] = self->pal_colors[c1];
            self->PaletteModified = true;
            SDL_SetPaletteColors(self->DisplayPalette, self->display_colors, 0, 16);
            SDL_SetSurfacePalette(self->DisplayCanvas, self->DisplayPalette);
        } else if (p == 0) {

            self->DrawPaletteIdx[c0] = c1;
            self->PaletteModified = true;
            Pico8_sync_draw_pal(self);
            SDL_SetSurfacePalette(self->DrawCanvas, self->Draw_Palette);
        }
    }

}

void Pico8_Print(Pico8 *self, LispCmd *lisp_cmd) {
    char *text = NULL;
    int x, y, c;
    SDL_Rect rect_;

    x = self->Cursor[0];
    y = self->Cursor[1];
    c = 1;


    if (lisp_cmd->Argc == 0 || lisp_cmd->Argc == 2) {
        return;
    }

    if (lisp_cmd->Argc > 0) {
        text = CmdArg_GetStr(&lisp_cmd->Args[0]);
        self->Cursor[1] += 6;
    }
    if (lisp_cmd->Argc > 2) {
        x = CmdArg_GetInt(&lisp_cmd->Args[1]);
        y = CmdArg_GetInt(&lisp_cmd->Args[2]);
        self->Cursor[0] = x;
        self->Cursor[1] = y;
    }

    if (lisp_cmd->Argc > 3) {
        c = CmdArg_GetInt(&lisp_cmd->Args[3]);
    }

    Pico8_set_color(self, c);

    SDL_Surface *imgText = Font_Render(self->Font, text, false,
                                       &self->draw_colors[self->DrawPaletteIdx[self->PenColor]], NULL);
    SDL_SetColorKey(imgText, SDL_TRUE, 0);

    rect_ = (SDL_Rect) {x, y, 0, 0};
    Surface_Blit(self->DrawCanvas, imgText, &rect_, NULL);
    SDL_FreeSurface(imgText);

}

/*
* api_sfx
* n
* The number of the sound effect to play (0-63), -1 to stop playing sound on the given channel, or -2 to release the sound of the given channel from looping.
* channel
The channel to use for the sound effect (0-3). The default is -1, which chooses an available channel automatically. Can be -2 to stop playing the given sound effect on any channels it plays on.
* offset
The note position in the sound effect to start playing (0-31). The default is 0 (the beginning).
* length
The number of notes in the sound effect to play (0-31). The default is to play the entire sound effect.
*/
void Pico8_Sfx(Pico8 *pico8, LispCmd *lisp_cmd) {
    /*
     n=-1 stop sound on channel
     n=-2 to stop looping on channel
    */

    int n, channel, offset, length;
    pico8_channel *ch;
    int i;

    ch = NULL;

    if (lisp_cmd->Argc == 0) {
        return;
    }

    if (lisp_cmd->Argc > 0) {
        n = CmdArg_GetInt(&lisp_cmd->Args[0]);
    }

    if (lisp_cmd->Argc > 1) {
        channel = CmdArg_GetInt((&lisp_cmd->Args[1]));
    }

    if (lisp_cmd->Argc > 2) {
        offset = CmdArg_GetInt((&lisp_cmd->Args[2]));
    }

    if (lisp_cmd->Argc > 3) {
        length = CmdArg_GetInt((&lisp_cmd->Args[3]));
    }

    if (n == -1) {
        if (channel >= 0) {
            pico8->audio_channels[channel].sfx = NIL;
            return;
        }
    } else if (n == -2) {
        if (channel >= 0) {
            pico8->audio_channels[channel].loop = FALSE;
            return;
        }
    }

    if (n > 63) {
        n = 63;
    } else if (n < 0) {
        n = 0;
    }
    printf("n = %d\n", n);
    if (offset > 31) { offset = 31; }
    else if (offset < 0) { offset = 0; }

    if (channel == NIL) {
        //find a free channel
        for (i = 0; i < 4; i++) {
            if (pico8->audio_channels[i].sfx == NIL) {
                channel = i;
                break;
            } else if (pico8->audio_channels[i].sfx == n) {
                channel = i;
            }
        }
    }

    if (channel == NIL) return;

    printf("channel %d\n", channel);
    ch = &pico8->audio_channels[channel];
    ch->sfx = n;
    ch->offset = offset;
    ch->last_step = offset - 1;
    ch->loop = TRUE;

    printf("api_sfx %d\n", ch->sfx);
}

/*
 n
 The pattern number to start playing (0-63), or -1 to stop playing music.

 fadems
 If not 0, fade in (or out) the music volume over a duration, given as a number of milliseconds.

 channelmask
 A bitfield indicating which of the four sound channels should be reserved for music. The default is 0 (no channels reserved).

*/
void Pico8_Music(Pico8 *pico8, LispCmd *lisp_cmd) {

    int n;
    int fade_len;
    int channel_mask;

    if (lisp_cmd->Argc == 0) {
        return;
    }

    if (lisp_cmd->Argc > 0) {
        n = CmdArg_GetInt(&lisp_cmd->Args[0]);
    }

    if (lisp_cmd->Argc > 1) {
        fade_len = CmdArg_GetInt((&lisp_cmd->Args[1]));
    }

    if (lisp_cmd->Argc > 2) {
        channel_mask = CmdArg_GetInt((&lisp_cmd->Args[2]));
    }

    api_music(pico8,n,fade_len,channel_mask);
}

void api_music(Pico8*pico8,int n,int fade_len, int channel_mask) {

    int i;
    int music_speed;
    int music_channel;
    pico8_music *m = NULL;
    pico8_sfx *sfx = NULL;

    if (n == -1) {
        if (pico8->current_music != NULL) {
            for (i = 0; i < 4; i++) {
                if (pico8->music_data[pico8->current_music->music].ch_ids[i] < 64) { // 64=0x40,has no sounds
                    pico8->audio_channels[i].sfx = NIL;
                    pico8->audio_channels[i].offset = 0;
                    pico8->audio_channels[i].last_step = NIL;
                }
            }
            free(pico8->current_music);
            pico8->current_music = NULL;
        }
        return;
    }

    if (n > 63) {
        n = 63;
    } else if (n < 0) {
        n = 0;
    }

    m = &pico8->music_data[n];
    music_speed = NIL;
    music_channel = NIL;

    for (i = 0; i < 4; i++) {
        if (m->ch_ids[i] < 64) {
            sfx = &pico8->sfx_data[m->ch_ids[i]];
            if (sfx->loop_start >= sfx->loop_end) {
                music_speed = sfx->duration;
                music_channel = i;
                break;
            } else if (music_speed == NIL || music_speed > sfx->duration) {
                music_speed = sfx->duration;
                music_channel = i;
            }
        }
    }

    if (music_channel == NIL) {
        return api_music(pico8, NIL, NIL, NIL);
    }

    pico8->audio_channels[music_channel].loop = FALSE;
    if (pico8->current_music == NULL) {
        pico8->current_music = (pico8_current_music *) malloc(sizeof(pico8_current_music));
        pico8->current_music->speed = 0;
    }

    pico8->current_music->music = n;
    pico8->current_music->offset = 0;

    if (channel_mask == NIL) {
        channel_mask = 15;//00001111,right now,api_sfx does not honor the channel_mask
    }

    pico8->current_music->channel_mask = channel_mask;
    pico8->current_music->speed = music_speed;

    for (i = 0; i < 4; i++) {
        if (pico8->music_data[n].ch_ids[i] < 64) {
            pico8->audio_channels[i].sfx = pico8->music_data[n].ch_ids[i];
            pico8->audio_channels[i].offset = 0;
            pico8->audio_channels[i].last_step = -1;
        }
    }


}


void Pico8_UpdateAudio(Pico8 *pico8, uint8_t *buffer) {

    int bufferpos;
    int buffer_size;

    int __sample_rate;
    int next_track;

    pico8_music *music;
    float sample = 0.0;
    int channel;
    pico8_channel *ch;
    pico8_sfx *sfx;
    int16_t *data16 = (int16_t *) buffer;

    __sample_rate = 22050;
    buffer_size = AUDIO_BUFFER;

    for (bufferpos = 0; bufferpos < buffer_size; bufferpos++) {
        if (pico8->current_music != NULL) {
            pico8->current_music->offset = pico8->current_music->offset +
                                           (7350.0 / (61.0 * (float) (pico8->current_music->speed * __sample_rate)));
            if ((int) pico8->current_music->offset >= 32) {
                next_track = pico8->current_music->music;
                if (pico8->music_data[next_track].loop_control == MUSIC_END) {
                    //go back untile we find the loop start
                    while (TRUE) {
                        if (pico8->music_data[next_track].loop_control == MUSIC_BEGIN || next_track == 0) {
                            break;
                        }
                        next_track = next_track - 1;
                    }
                } else if (pico8->music_data[pico8->current_music->music].loop_control == MUSIC_STOP_AT_END) {
                    next_track = NIL;//-1 == nil
                } else if (pico8->music_data[pico8->current_music->music].loop_control <= MUSIC_BEGIN) {
                    next_track = next_track + 1;
                }

                if (next_track > NIL) {
                    printf("next_track %d\n", next_track);
                    api_music(pico8, next_track, NIL, NIL);
                }
            }
        }// pico8->current_music!=NULL

        music = (pico8->current_music != NULL) ? &pico8->music_data[pico8->current_music->music] : NULL;

        sample = 0.0;
        for (channel = 0; channel < 4; channel++) {
            ch = &pico8->audio_channels[channel];

            if (ch->sfx > NIL && ch->sfx < 64) {
                sfx = &pico8->sfx_data[ch->sfx];
                ch->offset = ch->offset + (7350.0 / (61.0 * (float) (sfx->duration * __sample_rate)));
                if (sfx->loop_end != 0 && (int) ch->offset >= sfx->loop_end) {
                    if (ch->loop == TRUE) {
                        ch->last_step = -1;
                        ch->offset = (float) sfx->loop_start;
                    } else {
                        pico8->audio_channels[channel].sfx = NIL;
                    }
                } else if (ch->offset >= 32) {
                    pico8->audio_channels[channel].sfx = NIL;
                }
            }//

            if (ch->sfx > NIL && ch->sfx < 64) {
                sfx = &pico8->sfx_data[ch->sfx];
                //when we pass a new step
                if ((int) ch->offset > ch->last_step) {
                    ch->lastnote = ch->note;
                    ch->note = sfx->notes[(int) ch->offset].pitch;
                    ch->instr = sfx->notes[(int) ch->offset].waveform;
                    ch->vol = sfx->notes[(int) ch->offset].volume;
                    ch->fx = sfx->notes[(int) ch->offset].effect;

                    if (ch->instr != NOISE) {
                        ch->osc = Osc[ch->instr];
                    } else {
                        // use ch->noise->call(x, ch->noise);
                    }
                    if (ch->fx == 2) {
                        ch->lfo->osc = Osc[0];
                    } else if (ch->fx >= 6) {
                        ch->lfo->osc = osc_saw_lfo;
                    }
                    if (ch->vol > 0) {
                        ch->freq = note_to_hz(ch->note);
                    }
                    ch->last_step = (int) ch->offset;
                }
                if (ch->vol != NIL && ch->vol > 0) {
                    int vol = ch->vol;
                    if (ch->fx == 1) {
                        // slide from previous note over the length of a step
                        ch->freq = lerp(note_to_hz(ch->lastnote), note_to_hz(ch->note), fmod(ch->offset, 1.0));
                    } else if (ch->fx == 2) {
                        // vibrato one semitone?
                        ch->freq = lerp(note_to_hz(ch->note), note_to_hz((float) ch->note + 0.5),
                                        ch->lfo->call(8, ch->lfo));
                    } else if (ch->fx == 3) {
                        //drop/bomb slide from note to c-0
                        ch->freq = lerp(note_to_hz(ch->note), 0, fmod(ch->offset, 1.0));
                    } else if (ch->fx == 4) {
                        // fade in
                        vol = lerp(0, ch->vol, fmod(ch->offset, 1.0));
                    } else if (ch->fx == 5) {
                        //fade out
                        vol = lerp(ch->vol, 0, fmod(ch->offset, 1.0));
                    } else if (ch->fx == 6) {
                        //fast appreggio over 4 steps
                        int _off = (int) ch->offset & 0xfc;
                        int _lfo = (int) (ch->lfo->call(sfx->duration <= 8 ? 16 : 8, ch->lfo) * 4.0);
                        _off = _off + _lfo;
                        ch->freq = note_to_hz(sfx->notes[_off].pitch);
                    } else if (ch->fx == 7) {
                        //slow appreggio over 4 steps
                        int _off = (int) ch->offset & 0xfc;
                        int _lfo = (int) (ch->lfo->call(sfx->duration <= 8 ? 8 : 4, ch->lfo) * 4.0);
                        _off += _lfo;
                        ch->freq = note_to_hz(sfx->notes[_off].pitch);
                    }
                    if (ch->instr != NOISE) {
                        ch->sample = ch->osc(ch->oscpos) * vol / 7.0;
                    } else {
                        ch->sample = ch->noise->call(ch->oscpos, ch->noise) * vol / 7.0;
                    }
                    ch->oscpos = ch->oscpos + ch->freq / (float) __sample_rate;
                } else {
                    ch->sample = 0;
                }
            } else {
                ch->sample = 0;
            }

            sample = sample + ch->sample;
        }

        //buffer:setSample
        // PICO-8 limits max volume to 80%,but we opt for increasing the volume
        data16[bufferpos] = (int16_t)(((ALfloat) fmin(fmax(sample * 1.25, -1), 1)) * (float) 0x7fff);
    }

}

void Pico8_SyncMusic(Pico8 *pico8, LispCmd *lisp_cmd) {
    //music,offset,channel_mask,speed
    int music,offset,channel_mask,speed;

    int i;
    int music_speed;
    int music_channel;
    pico8_music *m = NULL;
    pico8_sfx *sfx = NULL;

    music = -1;
    offset = 0;
    channel_mask = 15;
    speed = 0;

    if (lisp_cmd->Argc == 0) {
        printf("SyncMusic no arguments\n");
        return;
    }

    if(lisp_cmd->Argc > 0 ) {
        music = CmdArg_GetInt(&lisp_cmd->Args[0]);
    }

    if(lisp_cmd->Argc > 1) {
        offset = CmdArg_GetFloat(&lisp_cmd->Args[1]);
    }
    if(lisp_cmd->Argc > 2 ) {
        channel_mask = CmdArg_GetInt(&lisp_cmd->Args[2]);
    }

    if(lisp_cmd->Argc > 3) {
        speed = CmdArg_GetInt(&lisp_cmd->Args[3]);
    }

    if (music == -1) {
        if (pico8->current_music != NULL) {
            for (i = 0; i < 4; i++) {
                if (pico8->music_data[pico8->current_music->music].ch_ids[i] < 64) { // 64=0x40,has no sounds
                    pico8->audio_channels[i].sfx = NIL;
                    pico8->audio_channels[i].offset = 0;
                    pico8->audio_channels[i].last_step = NIL;
                }
            }
            free(pico8->current_music);
            pico8->current_music = NULL;
        }
        return;
    }

    if (music > 63) {
        music = 63;
    } else if (music < 0) {
        music = 0;
    }

    m = &pico8->music_data[music];
    music_speed = NIL;
    music_channel = NIL;

    for (i = 0; i < 4; i++) {
        if (m->ch_ids[i] < 64) {
            sfx = &pico8->sfx_data[m->ch_ids[i]];
            if (sfx->loop_start >= sfx->loop_end) {
                music_speed = sfx->duration;
                music_channel = i;
                break;
            } else if (music_speed == NIL || music_speed > sfx->duration) {
                music_speed = sfx->duration;
                music_channel = i;
            }
        }
    }

    if (music_channel == NIL) {
        return api_music(pico8, NIL, NIL, NIL);
    }

    pico8->audio_channels[music_channel].loop = FALSE;
    if (pico8->current_music == NULL) {
        pico8->current_music = (pico8_current_music *) malloc(sizeof(pico8_current_music));
        pico8->current_music->speed = 0;
    }

    pico8->current_music->music = music;
    pico8->current_music->offset = offset;

    if (channel_mask == NIL) {
        channel_mask = 15;//00001111,right now,api_sfx does not honor the channel_mask
    }

    pico8->current_music->channel_mask = channel_mask;
    pico8->current_music->speed = music_speed;

    for (i = 0; i < 4; i++) {
        if (pico8->music_data[music].ch_ids[i] < 64) {
            pico8->audio_channels[i].sfx = pico8->music_data[music].ch_ids[i];
            pico8->audio_channels[i].offset = offset;
            pico8->audio_channels[i].last_step = (int)offset;
        }
    }

}
