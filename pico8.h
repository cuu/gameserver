#ifndef PICO8_H
#define PICO8_H

#include <stdio.h>
#include <stdlib.h>

#include <SDL.h>
#include <SDL_ttf.h>


#include "types.h"
#include "lisp_parser.h"
#include "font.h"

#include "osc.h"

#define RES_GFX    0
#define RES_GFF    1
#define RES_MAP    2
#define RES_SFX    3
#define RES_MUSIC  4

#define MUSIC_BEGIN 01  // 001
#define MUSIC_END   02  // 010
#define MUSIC_STOP_AT_END 04  //100

enum SFX_Type {
    TRI,
    UNEVEN_TRI,
    SAW,
    SQR,
    PULSE,
    ORGAN,
    NOISE,
    DETUNED_TRI
};


// pitch,waveform,volume,effect aka note,instr,vol,fx in lua
typedef struct _pico8_sfx_note {
    uint16_t pitch; //c-0 to d#-5, chromatic scale
    uint8_t waveform; // : 0 sine, 1 triangle, 2 sawtooth, 3 long square, 4 short square, 5 ringing, 6 noise, 7 ringing
    uint8_t volume; // 0-7
    uint8_t effect; // 0 none, 1 slide, 2 vibrato, 3 drop, 4 fade_in, 5 fade_out, 6 arp fast, 7 arp slow;
} pico8_sfx_note;

typedef struct _pico8_sfx {

    uint8_t mode;
    uint8_t duration;//spd
    uint8_t loop_start;
    uint8_t loop_end;

    pico8_sfx_note notes[32];

} pico8_sfx;


typedef struct _pico8_music {
    uint8_t loop_control;
    uint8_t ch_ids[4];

} pico8_music;


typedef struct _pico8_channel {
    float oscpos;
    float sample;
    osc_noise_closure *noise;

    oldosc_closure *lfo;
    float freq;

    int sfx;
    float offset;//1.0-32.0,ch.offset+7350/(61*sfx.speed*__sample_rate)
    int last_step;
    boolean loop;

    // aka lua
    osc_func osc;
    uint8_t lastnote;
    uint8_t note;
    uint8_t instr;
    uint8_t vol;
    uint8_t fx;

} pico8_channel;


typedef struct _pico8_current_music {
    int music;
    float offset;
    int channel_mask;
    int speed;
} pico8_current_music;


typedef struct {

    int Width;
    int Height;
    int Version;

    SDL_Surface *CanvasHWND;
    SDL_Surface *HWND;

    SDL_Surface *DisplayCanvas;
    SDL_Surface *DrawCanvas;
    SDL_Surface *GfxSurface;

    SDL_Color pal_colors[16];
    SDL_Color draw_colors[16];
    SDL_Color display_colors[16];

    int DrawPaletteIdx[16];


    SDL_Palette *DisplayPalette;
    SDL_Palette *Draw_Palette;

    int PalTransparent[16];

    SDL_Rect ClipRect;
    int PenColor;

    int Cursor[2];
    int CameraDx;
    int CameraDy;

    bool PaletteModified;
    int Uptime;

    TTF_Font *Font;

    unsigned char *Map;//[64*128];//8k,32 shared ,
    unsigned char *Sprite;//[128*128];//16k
    unsigned char *SpriteFlags;//[256];//

    pico8_sfx *sfx_data;
    pico8_music *music_data;
    pico8_channel *audio_channels;
    pico8_current_music *current_music;


    int res_state;
    int res_offset;

    float FPS;
    int frames;
    int curr_time;
    int prev_time;

} Pico8;

Pico8 *NewPico8();

void Pico8_Version(Pico8 *self, LispCmd *lisp_cmd);

void Pico8_Res(Pico8 *self, LispCmd *lisp_cmd);

void Pico8_ResOver(Pico8 *self, LispCmd *lisp_cmd);

void Pico8_ResDone(Pico8 *self, LispCmd *lisp_cmd);

void Pico8_SetResource(Pico8 *self, char *data);

void Pico8_Cls(Pico8 *self, LispCmd *lisp_cmd);

void Pico8_Spr(Pico8 *self, LispCmd *lisp_cmd);

void Pico8_Map(Pico8 *self, LispCmd *lisp_cmd);

int Pico8_Color(Pico8 *self, LispCmd *lisp_cmd);

void Pico8_Flip(Pico8 *self, LispCmd *lisp_cmd);

void Pico8_Print(Pico8 *self, LispCmd *lisp_cmd);

void Pico8_Rectfill(Pico8 *self, LispCmd *lisp_cmd);

void Pico8_Rect(Pico8 *self, LispCmd *lisp_cmd);

void Pico8_Circ(Pico8 *self, LispCmd *lisp_cmd);

void Pico8_Circfill(Pico8 *self, LispCmd *lisp_cmd);

void Pico8_Palt(Pico8 *self, LispCmd *lisp_cmd);

void Pico8_Pal(Pico8 *self, LispCmd *lisp_cmd);

void Pico8_Sfx(Pico8 *pico8, LispCmd *lisp_cmd);

void Pico8_Music(Pico8 *pico8, LispCmd *lisp_cmd);

void Pico8_set_shared_map(Pico8 *self);

void Pico8_draw_map(Pico8 *self, int n, int x, int y);

int Pico8_set_color(Pico8 *self, int p);

void Pico8_set_palt(Pico8 *self, int c, int t);


void Pico8_UpdateAudio(Pico8 *pico8, uint8_t *buffer);

void api_music(Pico8 *pico8, int n, int fade_len, int channel_mask);

#endif
