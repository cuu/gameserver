#include "gamethread.h"
#include "lisp_parser.h"


#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alext.h>


GameThread *NewGameThread() {
    GameThread *p = NULL;

    p = (GameThread *) malloc(sizeof(GameThread));

    p->Width = 320;
    p->Height = 240;

    p->Inited = false;

    p->Frames = 0;
    p->PrevTime = 0;
    p->CurrentTime = 0;

    p->state = STATE_DRAW;

    if (TTF_Init() == -1) {
        printf("TTF_Init: %s\n", TTF_GetError());
        exit(2);
    }

    p->ThePico8 = NewPico8();

    p->last_keydown_time = 0;
    //p->UdpDataTrashNumber = 0;

    p->LastUDP_PackNumber = 0;

    p->kcp1 = NULL;

    p->TheUser = (User *) malloc(sizeof(User));
    memset(p->TheUser->Nick, 0, NICK);
    memset(p->TheUser->Password, 0, PASSWORD);
    memset(p->TheUser->Email, 0, EMAIL);
    p->TheUser->ID = 1; // non-zero

    return p;
}


void GameThread_InitWindow(GameThread *self) {
    if (self->Inited == false) {

        if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
            SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
            return;
        }


        if (TTF_Init() == -1) {
            printf("TTF_Init: %s\n", TTF_GetError());
            exit(2);
        }

        self->window = SDL_CreateWindow("gameserver", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, self->Width,
                                        self->Height, SDL_WINDOW_SHOWN);

        if (self->window == NULL) {
            SDL_Quit();
            return;
        }

        self->renderer = SDL_CreateRenderer(self->window, 0, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

        self->big_surface = NewSurfaceNoMask(self->Width, self->Height, 32);


        if (self->big_surface == NULL) {
            SDL_Log("SDL_CreateRGBSurface() failed: %s", SDL_GetError());
            exit(1);
        }

        self->texture = SDL_CreateTextureFromSurface(self->renderer, self->big_surface);

        if (self->texture == NULL) {
            fprintf(stderr, "CreateTextureFromSurface failed: %s\n", SDL_GetError());
            exit(1);
        }

        self->big_surface_pixels = self->big_surface->pixels;

        self->ThePico8->HWND = self->big_surface;

        self->Inited = true;
    }

}

void GameThread_QuitWindow(GameThread *self) {
    self->Inited = false;

    SDL_DestroyWindow(self->window);
    SDL_DestroyRenderer(self->renderer);

    SDL_Quit();
}

void GameThread_SendBtn(GameThread *self, SDL_Event event) {

    char buffer[32];
    char *down = "Down";
    char *up = "Up";
    char *p = NULL;
    int now = 0;
    int hr;

    memset(buffer, 0, 32);

    if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
        if (event.type == SDL_KEYDOWN) {
            p = down;
        } else if (event.type == SDL_KEYUP) {
            p = up;
        }

        switch (event.key.keysym.sym) {
            case SDLK_LEFT:
                sprintf(buffer, "(btn %d %d \"%s\")\n", self->TheUser->ID, LEFT, p);
                break;
            case SDLK_RIGHT:
                sprintf(buffer, "(btn %d %d \"%s\")\n", self->TheUser->ID, RIGHT, p);
                break;
            case SDLK_UP:
                sprintf(buffer, "(btn %d %d \"%s\")\n", self->TheUser->ID, UP, p);
                break;
            case SDLK_DOWN:
                sprintf(buffer, "(btn %d %d \"%s\")\n", self->TheUser->ID, DOWN, p);
                break;
            case SDLK_u:
                sprintf(buffer, "(btn %d %d \"%s\")\n", self->TheUser->ID, U, p);
                break;
            case SDLK_i:
                sprintf(buffer, "(btn %d %d \"%s\")\n", self->TheUser->ID, I, p);
                break;
            case SDLK_RETURN:
                sprintf(buffer, "(btn %d %d \"%s\")\n", self->TheUser->ID, RETURN, p);
                break;
            case SDLK_ESCAPE:
                sprintf(buffer, "(btn %d %d \"%s\")\n", self->TheUser->ID, ESCAPE, p);
                break;
        }
    }

    if (strlen(buffer) > 1) {
        //now = SDL_GetTicks();

        //mill_udpsend(self->udpsock, self->outaddr, buffer,strlen(buffer));

        //printf("buffer %s\n",buffer);

        //if( now - self->last_keydown_time > (int)(1.0/self->ThePico8->FPS*2) )
        /*
        {
          mill_tcpsend(self->tcpsock,buffer,strlen(buffer),-1);
          mill_tcpflush(self->tcpsock, -1);
          //self->last_keydown_time = now;
        }
        */
        //printf("%s\n",buffer);
        //ikcp_update(self->kcp1, iclock());
        hr = ikcp_send(self->kcp1, buffer, strlen(buffer));
        if (hr < 0) {
            perror("ikcp_send");
        }
        ikcp_update(self->kcp1, iclock());
    }
}

void GameThread_EventLoop(GameThread *self) {

    SDL_Event event;

    while (self->Inited) {
        while (SDL_PollEvent(&event)) {

            if (event.type == SDL_QUIT) {
                GameThread_QuitWindow(self);
                break;
            }

            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_q || event.key.keysym.sym == SDLK_ESCAPE) {
                    printf("Quiting...\n");
                    GameThread_QuitWindow(self);
                    break;
                }
            }
            if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
                GameThread_SendBtn(self, event);
            }
        }
        mill_msleep(mill_now() + (int) ((1 / self->ThePico8->FPS) * 1000.0));
    }

}

mill_coroutine void GameThread_FlipLoop(GameThread *self) {
    int fps;

    while (self->Inited) {
        SDL_UpdateTexture(self->texture, NULL, self->big_surface_pixels, self->Width * sizeof(int));
        SDL_RenderCopy(self->renderer, self->texture, NULL, NULL);
        SDL_RenderPresent(self->renderer);

        self->Frames += 1;
        self->CurrentTime = SDL_GetTicks();
        if ((self->CurrentTime - self->PrevTime) > 10000) {
            fps = self->Frames / 10;
            printf("fps is %d\n", fps);
            self->Frames = 0;
            self->PrevTime = self->CurrentTime;
        }

        mill_msleep(mill_now() + (int) ((1 / self->ThePico8->FPS) * 1000.0));
    }

}

mill_coroutine void GameThread_AudioLoop(GameThread *self) {
    ALint num_loops;
    num_loops = 1;
    ALuint source,thebuf;
    ALuint buffers[MAX_BUFFERS];
    ALint srate = 22050;
    ALint processed;
    ALint val;
    int i;
    ALCint dev_rate;
    uint8_t *data;

    ALCdevice *device = alcGetContextsDevice(alcGetCurrentContext());
    alcGetIntegerv(device, ALC_FREQUENCY, 1, &dev_rate);
    assert(alcGetError(device) == ALC_NO_ERROR && "Failed to get device sample rate");

    if (srate < 0) { srate = dev_rate; }

    source = 0;
    alGenSources(1, &source);

    alSourcei(source, AL_DIRECT_CHANNELS_SOFT, AL_TRUE);

    for (i = 0; i < MAX_BUFFERS; i++) {
        ALuint buf;
        alGenBuffers(1, &buf);
        if (alGetError() == AL_NO_ERROR) {
            push(&self->unused_buf1, buf);
        }
    }

    assert(alGetError()==AL_NO_ERROR && "Failed to setup sound source");

    alSourcePlay(source);

    data = calloc(1, AUDIO_BUFFER*2);

    while (self->Inited) {

        for (i = 0; i < size(&self->unused_buf1); i++) {

            //ApplyOsc(SAW,data, 1.0, srate, tone_freq);
            Pico8_UpdateAudio(self->ThePico8, data);

            thebuf = peek(&self->unused_buf1);
            pop(&self->unused_buf1);
            alBufferData(thebuf, AL_FORMAT_MONO16, data, AUDIO_BUFFER * 2, srate);
            alSourceQueueBuffers(source, 1, &thebuf);
            alSourcePlay(source);
        }

        alGetSourcei(source, AL_BUFFERS_PROCESSED, &processed);

        while (processed <= 0) { // wait for play at least one sound
            mill_msleep(mill_now() + 10);
            alGetSourcei(source, AL_BUFFERS_PROCESSED, &processed);
        }

        printf("\r %d", processed);
        fflush(stdout);

        alSourceUnqueueBuffers(source, processed, buffers);

        for (i = 0; i < processed; i++) {
            push(&self->unused_buf1, buffers[i]);
        }

        if (alGetError() != AL_NO_ERROR) {
            printf("got open al error\n");
            break;
        }

        alGetSourcei(source, AL_SOURCE_STATE, &val);
        if (val != AL_PLAYING) {
            alSourcePlay(source);
        }

        mill_msleep(mill_now() + 10);
    }


    alDeleteSources(1, &source);

}


char *GameThread_Btn(GameThread *self, LispCmd *lisp_cmd) {

    int keycode_idx;

    if (lisp_cmd->Argc < 2) {
        return "FALSE";
    }

    keycode_idx = CmdArg_GetInt(&lisp_cmd->Args[0]);
    if (keycode_idx < 8 && self->KeyLog[keycode_idx] >= 0) {
        return "TRUE";
    }

    return "FALSE";
}

void GameThread_Run(GameThread *self) {
    GameThread_InitWindow(self);
    mill_go(GameThread_FlipLoop(self));
    mill_go(GameThread_AudioLoop(self));
    GameThread_EventLoop(self);

}

void GameThread_Pong(GameThread *self, LispCmd *lisp_cmd) {
    printf("Pong\n");

}

char *GameThread_ProcessLispCmd(GameThread *self, char *cmd) {

    LispCmd *lisp_cmd = NULL;


    lisp_cmd = lisp_parser(cmd);

    if (lisp_cmd != NULL) {

        if (strcmp(lisp_cmd->Func, "res") == 0) {
            printf("set resource....\n");
            self->state = STATE_RES;
            Pico8_Res(self->ThePico8, lisp_cmd);
        } else if (strcmp(lisp_cmd->Func, "resdone") == 0) {

            Pico8_ResDone(self->ThePico8, lisp_cmd);

        } else if (strcmp(lisp_cmd->Func, "pico8") == 0) {
            //printf("%s\n",cmd);
            Pico8_Version(self->ThePico8, lisp_cmd);

        } else if (strcmp(lisp_cmd->Func, "spr") == 0) {

            Pico8_Spr(self->ThePico8, lisp_cmd);

        } else if (strcmp(lisp_cmd->Func, "map") == 0) {

            //printf("%s\n",cmd);
            Pico8_Map(self->ThePico8, lisp_cmd);

        } else if (strcmp(lisp_cmd->Func, "color") == 0) {

            Pico8_Color(self->ThePico8, lisp_cmd);

        } else if (strcmp(lisp_cmd->Func, "flip") == 0) {

            Pico8_Flip(self->ThePico8, lisp_cmd);

        } else if (strcmp(lisp_cmd->Func, "print") == 0) {
            //printf("%s\n",cmd);
            Pico8_Print(self->ThePico8, lisp_cmd);

        } else if (strcmp(lisp_cmd->Func, "rectfill") == 0) {

            Pico8_Rectfill(self->ThePico8, lisp_cmd);

        } else if (strcmp(lisp_cmd->Func, "rect") == 0) {

            Pico8_Rect(self->ThePico8, lisp_cmd);

        } else if (strcmp(lisp_cmd->Func, "palt") == 0) {

            Pico8_Palt(self->ThePico8, lisp_cmd);

        } else if (strcmp(lisp_cmd->Func, "pal") == 0) {

            Pico8_Pal(self->ThePico8, lisp_cmd);
        } else if (strcmp(lisp_cmd->Func, "ping") == 0) {

            GameThread_Pong(self, lisp_cmd);
        } else if (strcmp(lisp_cmd->Func, "circ") == 0) {
            Pico8_Circ(self->ThePico8, lisp_cmd);
        } else if (strcmp(lisp_cmd->Func, "circfill") == 0) {
            Pico8_Circfill(self->ThePico8, lisp_cmd);
        } else if (strcmp(lisp_cmd->Func, "cls") == 0) {
            Pico8_Cls(self->ThePico8, lisp_cmd);
        } else if (strcmp(lisp_cmd->Func,"sfx") == 0 ) {
            Pico8_Sfx(self->ThePico8,lisp_cmd);
        } else if (strcmp(lisp_cmd->Func,"music") == 0 ) {
            Pico8_Music(self->ThePico8,lisp_cmd);
        }

        if (lisp_cmd->Args != NULL) {
            free(lisp_cmd->Args);
        }
        free(lisp_cmd);
    }

    return "O";
}

char *GameThread_ProcessLispCmds(GameThread *self, char *cmds) {

    char *pch = NULL;
    char *tmp = NULL;

    if (strlen(cmds) == 0) {
        return "Error";
    }

    if (self->state == STATE_DRAW) {

        tmp = trim(cmds, "\n");
        tmp = trim(tmp, "\r\n");

        pch = strtok(tmp, "|");
        while (pch != NULL) {
            //printf ("%s\n",pch);
            GameThread_ProcessLispCmd(self, pch);
            pch = strtok(NULL, "|");
        }

    } else {
        if (strstr(cmds, "(resover)") != NULL) {
            printf("find resover,%s\n", cmds);
            self->state = STATE_DRAW;
            Pico8_ResOver(self->ThePico8, NULL);
        } else {
            //printf("%s\n",cmds);
            Pico8_SetResource(self->ThePico8, cmds);
        }
    }
    return "O";
}

char *GameThread_ProcessIRCPackageUDP(GameThread *self, char *udp_buff) {
/*
 * /pack 172971 (rectfill 1 2 3 4 27)|(rectfill 1 2 3 4 95)|(rectfill 1 2 3 4 35)
 * /pack 172970 (rectfill 1 2 3 4 1)|(rectfill 1 2 3 4 49)|(rectfill 1 2 3 4 53)
 * 
*/
    //skip the packages arrived late

    int start_pos;
    int end_pos;
    char *p;
    char *data;
    char frame[64];
    long int frame_number;
    int n;
    start_pos = strpos(udp_buff, "/", 0);
    end_pos = strpos(udp_buff, "\r\n", 0);

    if (start_pos != -1 && end_pos != -1 && end_pos > start_pos) {
        start_pos = strpos(udp_buff, " ", 0);

        if (start_pos != -1) {
            start_pos += 1;
            end_pos = strpos(udp_buff, " ", start_pos);
            if (end_pos != -1) {
                n = end_pos - start_pos;
                if (n < 64) {
                    strncpy(frame, udp_buff + start_pos, n);
                    frame[n] = '\0';
                    data = udp_buff + end_pos + 1;
                    printf("%s%s %d\n", data, frame, strlen(frame));
                    frame_number = strtol(frame, NULL, 10);
                    if (frame_number > self->LastUDP_PackNumber) {
                        GameThread_ProcessLispCmds(self, data);
                        self->LastUDP_PackNumber = frame_number;
                    }
                }
            }
        }
    }

}

void GameThread_User_GetID(GameThread *self, char *str) {

    unsigned long id;
    id = strtol(str, NULL, 10);

    self->TheUser->ID = (int) id;

}

