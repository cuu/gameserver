#ifndef TYPES_H
#define TYPES_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define TCPBUFF 8192
#define UDPBUFF 8192

typedef enum {
    false, true
}
bool;

typedef enum {
    FALSE, TRUE
} boolean;

typedef struct {

    char Type;
    char Value[256];

} CmdArg;


#define NONE -1
#define NIL -1

typedef struct {
    int x;
    int y;
} Point;

#define MAX_BUFFERS 64

#define AUDIO_BUFFER 1024

#endif
