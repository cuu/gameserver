#include "gamethread.h"
#include "lisp_parser.h"

GameThread*NewGameThread() {
  GameThread*p = NULL;
  
  p = (GameThread*)malloc(sizeof(GameThread));
  
  p->Width = 320;
  p->Height = 240;
  
  p->Inited = false;
  
  p->Frames = 0;
  p->PrevTime = 0;
  p->CurrentTime = 0;
  
  p->state = STATE_DRAW;
  
  return p;
}


void GameThread_InitWindow(GameThread*self) {
  if( self->Inited == false) {

    if (SDL_Init( SDL_INIT_EVERYTHING ) != 0) {
      SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
      return;
    }
    
     	
    if(TTF_Init()==-1) {
      printf("TTF_Init: %s\n", TTF_GetError());
      exit(2);
    }

    self->window = SDL_CreateWindow("gameserver", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, self->Width,self->Height, SDL_WINDOW_SHOWN);

    if (self->window == NULL) {
        SDL_Quit();
        return;
    }

    self->renderer = SDL_CreateRenderer(self->window, 0, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    
    self->big_surface = SDL_CreateRGBSurface(0,self->Width, self->Height, 32,0,0,0,0);
    if (self->big_surface == NULL) {
        SDL_Log("SDL_CreateRGBSurface() failed: %s", SDL_GetError());
        exit(1);
    }
    
    self->texture = SDL_CreateTextureFromSurface(self->renderer,self->big_surface);
    
    if (self->texture == NULL) {
      fprintf(stderr, "CreateTextureFromSurface failed: %s\n", SDL_GetError());
      exit(1);
    }
    
    self->big_surface_pixels = self->big_surface->pixels;
    
    self->ThePico8 = NewPico8();
    self->ThePico8->HWND = self->big_surface;
    
    self->Inited= true;
  }
  
}

void GameThread_QuitWindow(GameThread*self) {
  self->Inited = false;
  
  SDL_DestroyWindow(self->window);
  SDL_DestroyRenderer(self->renderer);

  SDL_Quit();
}


void GameThread_EventLoop(GameThread*self) {
  
  SDL_Event event;

  while(self->Inited) {
    while (SDL_PollEvent(&event)) {

      if (event.type == SDL_QUIT) {
        GameThread_QuitWindow(self);
        break;
      }

      if(event.type == SDL_KEYDOWN) {
        if ( event.key.keysym.sym == SDLK_q || event.key.keysym.sym == SDLK_ESCAPE) {
          printf("Quiting...\n");
          GameThread_QuitWindow(self);
          break;
        }
      }

    }
    mill_msleep(mill_now()+ (int)((1/self->ThePico8->FPS)*1000.0)  );
  }

}

mill_coroutine void GameThread_FlipLoop(GameThread*self) {
  int fps;
  
  while(self->Inited) {
    SDL_UpdateTexture(self->texture, NULL, self->big_surface_pixels, self->Width * sizeof(int));
    SDL_RenderCopy(self->renderer, self->texture, NULL, NULL);
    SDL_RenderPresent(self->renderer);
    
    self->Frames+=1;
    self->CurrentTime = SDL_GetTicks();
    if( (self->CurrentTime - self->PrevTime ) > 10000) {
      fps = self->Frames/10;
      printf("fps is %d\n",fps);
      self->Frames=0;
      self->PrevTime= self->CurrentTime;
    }

    mill_msleep( mill_now()+ (int)((1/self->ThePico8->FPS)*1000.0) );
  }

}

char *GameThread_Btn(GameThread*self,LispCmd*lisp_cmd) {
  
  int keycode_idx;

  if(lisp_cmd->Argc < 2) {
    return "FALSE";
  }

  keycode_idx = CmdArg_GetInt(&lisp_cmd->Args[0]);
  if( keycode_idx< 8 && self->KeyLog[keycode_idx] >= 0) {
    return "TRUE";
  }

  return "FALSE";
}

void GameThread_Run(GameThread*self) {
  GameThread_InitWindow(self);
  mill_go(GameThread_FlipLoop(self));
  
  GameThread_EventLoop(self);
  
}

char* GameThread_ProcessLispCmd(GameThread*self,char*cmd) {
  
  LispCmd *lisp_cmd=NULL;
  
  
  lisp_cmd = lisp_parser(cmd);
  
  if(lisp_cmd != NULL) {
   
    if(strcmp(lisp_cmd->Func,"res") == 0) {
      printf("set resource....\n");
      self->state = STATE_RES;
      Pico8_Res(self->ThePico8,lisp_cmd);
    }else if(strcmp(lisp_cmd->Func,"resdone") == 0) {

      Pico8_ResDone(self->ThePico8,lisp_cmd);

    }else if(strcmp(lisp_cmd->Func,"pico8") == 0) {
      //printf("%s\n",cmd);
      Pico8_Version(self->ThePico8,lisp_cmd);

    }else if(strcmp(lisp_cmd->Func,"spr")==0) {

      Pico8_Spr(self->ThePico8,lisp_cmd);

    }else if(strcmp(lisp_cmd->Func,"map") == 0) {

      Pico8_Map(self->ThePico8,lisp_cmd);

    }else if(strcmp(lisp_cmd->Func,"color")==0) {

      Pico8_Color(self->ThePico8,lisp_cmd);

    }else if(strcmp(lisp_cmd->Func,"flip")==0) {

      Pico8_Flip(self->ThePico8,lisp_cmd);

    }else if(strcmp(lisp_cmd->Func,"print")==0) {

      Pico8_Print(self->ThePico8,lisp_cmd);

    }else if(strcmp(lisp_cmd->Func,"rectfill")==0) {

      Pico8_Rectfill(self->ThePico8,lisp_cmd);

    }else if(strcmp(lisp_cmd->Func,"rect") == 0) {

      Pico8_Rect(self->ThePico8,lisp_cmd);

    }else if(strcmp(lisp_cmd->Func,"palt") == 0) {

      Pico8_Palt(self->ThePico8,lisp_cmd);

    }else if(strcmp(lisp_cmd->Func,"pal") == 0 ) {

      Pico8_Pal(self->ThePico8,lisp_cmd);
    }


    free(lisp_cmd);
  }
  
  return "O";
}

char* GameThread_ProcessLispCmds(GameThread*self,char*cmds) {

  char*pch= NULL;
  char*tmp = NULL;

  if(strlen(cmds)==0 ) {
    return "Error";
  }
  
  if(self->state == STATE_DRAW) {

    tmp = trim(cmds,"\n");
    pch = strtok(tmp,"|");
    while (pch != NULL)
    {
      //printf ("%s\n",pch);
      GameThread_ProcessLispCmd(self,pch);
      pch = strtok (NULL, "|");
    }

  }else {
    if(strstr(cmds,"(resover)") != NULL) {
      printf("find resover,%s\n",cmds);
      self->state = STATE_DRAW;
      Pico8_ResOver(self->ThePico8,NULL);
    }else {
      Pico8_SetResource(self->ThePico8,cmds);
    }
  }
  return "O";
}
