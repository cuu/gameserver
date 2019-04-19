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
    mill_msleep(mill_now()+30);
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

    mill_msleep( mill_now()+ (int)((1/30.0)*1000.0) );
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
