#include "osc.h"


float note_to_hz(float note) {
    return 440.0 * powf(2, ((note - 33.0) / 12.0));
}

float lerp(float a, float b, float t) {
    return (b - a) * t + a;
}


float osc_tri(float x) {
    float t = fmod(x, 1.0);
    return (fabs(t * 2.0 - 1.0) * 2.0 - 1.0) * 0.5;
}

float osc_uneven_tri(float x) {
    float t = fmod(x, 1.0);
    return (((t < 0.875) ? (t * 16.0 / 7.0) : ((1.0 - t) * 16.0)) - 1.0) * 0.5;
}

float osc_saw(float x) {
    float t = fmod(x, 1.0);
    return (t - 0.5) * 2.0 / 3.0;
}

float osc_sqr(float x) {
    float t = fmod(x, 1.0);
    return ((t < 0.5) ? 1.0 : -1.0) * 0.25;
}

float osc_pulse(float x) {
    float t = fmod(x, 1.0);
    return ((t < 0.3125) ? 1.0 : -1.0) * 0.25;
}

float osc_organ(float x) {
    x = x * 4.0;
    float t = fmod(x, 2.0);
    return (fabs(t - 1.0) - 0.5 + (fabs((fmod(x * 0.5, 2.0)) - 1.0) - 0.5) / 2.0 - 0.1) * 0.5;
}

float _osc_noise(float x, osc_noise_closure *_closure) {
    float hz, scale, output;

    hz = fmod(x - _closure->lastx, 1.0) * 22050.0; //__sample_rate = 22050
    _closure->lastx = x;
    scale = hz * (131072.0 / 343042875.0) + (16.0 / 889.0);
    _closure->update = !_closure->update;
    if (_closure->update) {
        _closure->sample = _closure->sample + scale * (((float) rand() / (float) (RAND_MAX)) * 2.0 - 1.0);
    }
    output = _closure->sample * (45.0 / 32.0);
    if (hz > _closure->hz48) {
        output = output *
                 (1.1659377442658412e+000 - 2.3350687035974510e-004 * hz + 8.3385655344351036e-008 * powf(hz, 2.0) -
                  1.1509506025078735e-011 * powf(hz, 3.0));
    }
    _closure->sample = fmax(fmin(_closure->sample, (6143.0 / 31115.0)), -(6143.0 / 31115.0));
    return output;
}

osc_noise_closure *new_osc_noise() {
    osc_noise_closure *func = (osc_noise_closure *) malloc(sizeof(osc_noise_closure));
    func->lastx = 0;
    func->sample = 0;
    func->update = FALSE;
    func->hz48 = note_to_hz(48);

    func->call = _osc_noise;
    return func;
}

float osc_detuned_tri(float x) {
    x = x * 2.0;
    float t;
    t = fmod(x, 2.0);

    return (fabs(fmod(x * 127.0 / 128.0, 2.0) - 1.0) / 2.0 + fabs(t - 1.0) - 1.0) * 2.0 / 3.0;
}


float osc_saw_lfo(float x) {
	return fmod(x,1.0);
}

float _oldosc(float freq, oldosc_closure *_closure) {
    _closure->x = _closure->x + freq / 22050.0;
    return _closure->osc(_closure->x);
}

oldosc_closure *new_oldosc_closure(osc_func _osc) {
    oldosc_closure *func = (oldosc_closure *) malloc(sizeof(oldosc_closure));
    func->x = 0;
    func->osc = _osc;
    func->call = _oldosc;
    return func;
}
