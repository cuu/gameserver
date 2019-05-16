#ifndef GAMETHREAD_H
#define GAMETHREAD_H

#include <stdio.h>
#include <stdlib.h>

#include <SDL.h>
#include <SDL_ttf.h>

#define MILL_USE_PREFIX
#include <libmill.h>


#include "types.h"
#include "utils.h"
#include "pico8.h"
#include "lisp_parser.h"
#include "surface.h"

#include "ikcp.h"

#define STATE_DRAW 0
#define STATE_RES 1

#define PASSWORD 256
#define EMAIL 1024 
#define NICK 64

#define LEFT   1
#define RIGHT  2
#define UP     3
#define DOWN   4
#define U      5
#define I      6
#define RETURN 7
#define ESCAPE 8

typedef struct {
  char cmd[32];
  int frame;
  char data[UDPBUFF];
}IRCPackage;

typedef struct {
  int ID;
  char Nick[NICK];
  char Password[PASSWORD];
  char Email[EMAIL];

}User;


typedef struct {

  int Width;
  int Height;
  bool Inited;
  
  Pico8 *ThePico8;

  SDL_Texture*texture;
  SDL_Window *window;
  SDL_Surface *big_surface;
  SDL_Renderer *renderer;
  void *big_surface_pixels;
  

  int Frames;
  int CurrentTime;
  int PrevTime;

  int KeyLog[8];
  int state;
 
  mill_udpsock udpsock;
  mill_ipaddr outaddr;
  
  mill_tcpsock tcpsock;
  int last_keydown_time;
  
  //IRCPackage UdpDataTrash[16];
  //int  UdpDataTrashNumber;
  int LastUDP_PackNumber;
  User* TheUser;
  
  ikcpcb *kcp1;
  ikcpcb *kcp2;

}GameThread;


GameThread* NewGameThread();


void GameThread_InitWindow(GameThread*self);
void GameThread_QuitWindow(GameThread*self);

void  GameThread_SendBtn(GameThread*self,SDL_Event event);
void GameThread_EventLoop(GameThread*self);

mill_coroutine void GameThread_FlipLoop(GameThread*self);
char *GameThread_Btn(GameThread*self,LispCmd*lisp_cmd);

void GameThread_Run(GameThread*self);

char* GameThread_ProcessLispCmd(GameThread*self,char*cmd);
char* GameThread_ProcessLispCmds(GameThread*self,char*cmds);

void GameThread_User_GetID(GameThread*self,char*str );

#endif
