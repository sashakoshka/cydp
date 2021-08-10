#include <stdio.h>
#include <SDL2/SDL.h>

#include "el.h"

SDL_Window   *window   = NULL;
SDL_Renderer *renderer = NULL;
SDL_Surface  *surface  = NULL;
SDL_Event     event;
El           *root;

void draw();

int main(/*int argc, char **argv*/) {

  // Initialize SDL
  
  if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0) {
    printf("cant make window\n");
    goto error;
  }
  
  window = SDL_CreateWindow("cydp",
    SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
    640, 480, SDL_WINDOW_SHOWN | SDL_WINDOW_ALWAYS_ON_TOP
  );
  if(window == NULL) {
    printf("%s\n", SDL_GetError());
    goto error;
  }
  SDL_SetWindowBordered(window, SDL_FALSE);
  
  renderer = SDL_CreateRenderer(
    window,
    -1, 0
  );
  if(renderer == NULL) {
    printf("%s\n", SDL_GetError());
    goto error;
  }
  
  surface = SDL_GetWindowSurface(window);
  
  SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
  
  // Initialize element tree
  root = el_make(0);
  
  // Construct a test window
  El *testWin = el_make(2); {
    testWin->x = 32;
    testWin->y = 64;
    testWin->layout = 5;
    El *testWin_sidebar = el_make(1); {
      testWin_sidebar->layout = 4;
      testWin_sidebar->scrollable = 1;
      testWin_sidebar->cellHeight = 32;
      for(int i = 0; i < 8; i++) {
        El *sidebar_item = el_make(1);
        sidebar_item->width = -100;
        el_adopt(testWin_sidebar, sidebar_item);
      }
      el_adopt(testWin, testWin_sidebar);
    }
    el_adopt(root, testWin);
  }
  
  // Construct a test dock
  El *testDock = el_make(1); {
    testDock->layout = 4;
    testDock->scrollable = 1;
    testDock->x = -48;
    testDock->y = 0;
    testDock->width = 48;
    testDock->height = -100;
    el_adopt(root, testDock);
  }
  
  draw();
  
  // Loop  
  
  while(SDL_WaitEvent(&event)) {
    switch (event.type) {
      case SDL_QUIT:
        goto exit;
    }
  }
  
  exit:
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
  return 0;
  
  error:
  return 1;
}

void draw() {
  el_calc(root);
  el_draw(root, renderer);
  
  SDL_RenderPresent(renderer);
}
