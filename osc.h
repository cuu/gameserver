#ifndef OSC_H
#define OSC_H

#include "types.h"

typedef struct _osc_noise_closure {

    float lastx;
    float sample;
    boolean update;
    float hz48;

    float (*call)(float x, struct _osc_noise_closure *_closure);

} osc_noise_closure;


typedef float (*osc_func)(float);

typedef struct _oldosc_closure {
    float x;
    osc_func osc;

    float (*call)(float freq, struct _oldosc_closure *_closure);
} oldosc_closure;


float note_to_hz(float note);

float lerp(float a, float b, float t);

float osc_tri(float x);

float osc_uneven_tri(float x);

float osc_saw(float x);

float osc_sqr(float x);

float osc_pulse(float x);

float osc_organ(float x);

float _osc_noise(float x, osc_noise_closure *_closure);

osc_noise_closure *new_osc_noise();

float osc_detuned_tri(float x);

float osc_saw_lfo(float x);

float _oldosc(float freq, oldosc_closure *_closure);

oldosc_closure *new_oldosc_closure(osc_func _osc);


#endif