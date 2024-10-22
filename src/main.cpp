#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "gameutils.h"
#include "sdl2utils.h"
#include "remusutils.h"
#include <iostream>

#define LINEOUT(A) std::cout << A << '\n' 

enum InitFlag {
    INIT_SDL = 0b00000001,
    INIT_WIN = 0b00000010,
    INIT_REN = 0b00000100,
    INIT_IMG = 0b00001000,
    INIT_TEX = 0b00010000
};

enum EventFlag {
    E_QUIT = 1
};

const int imgFlags = 0b111111;

Vector2i winDim = {800, 600};

SDL_Window* mainWin = NULL;
SDL_Renderer* mainRend = NULL;
RemusMap *workingMap = NULL;
TexCache *texCache = NULL;

int init();
int initExt(int);
void close();

void handleEvents(SDL_Event*, int*);

int main(int argc, char **argv) {
    int flags = 0;
    SDL_Event e;
    std::string sprDir;
    std::string surfDir;
    workingMap = new RemusMap();

    int initFlags = init();
    if(initFlags > 0) {
        if(initFlags &= INIT_SDL) LINEOUT("ERROR: Failed to initialize SDL2.");
        if(initFlags &= INIT_WIN) LINEOUT("ERROR: Failed to create window.");
        if(initFlags &= INIT_REN) LINEOUT("ERROR: Failed to create renderer.");
        if(initFlags &= INIT_IMG) LINEOUT("ERROR: Failed to initialize SDL_image.");
        if(initFlags &= INIT_TEX) LINEOUT("ERROR: Failed to initialize texture caches.");
        return -1;
    }

    while(!(flags & E_QUIT)) {
        handleEvents(&e, &flags);
    }
    
    close();
    return 0;
}

int init() {
    int flag = 0;
    LINEOUT("Initializing SDL...");
    if(SDL_Init(SDL_INIT_VIDEO) < 0) flag |= INIT_SDL;
    else {
        LINEOUT("Creating window...");
        mainWin = SDL_CreateWindow("RemMapMaker", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, winDim.x, winDim.y, SDL_WINDOW_SHOWN);
        if(mainWin == NULL) flag |= INIT_WIN;
        else {
            LINEOUT("Creating renderer...");
            mainRend = SDL_CreateRenderer(mainWin, -1, SDL_RENDERER_ACCELERATED);
            if(mainRend == NULL) flag |= INIT_REN;
            else {
                LINEOUT("Initializing extensions...");
                flag |= initExt(INIT_IMG);
            }
        }
    }
    return flag;
}

int initExt(int initFlags) {
    int outFlags = 0;
    if(initFlags & INIT_IMG)
        if(!IMG_Init(imgFlags)) outFlags |= INIT_IMG;
        else {
            texCache = new TexCache(mainRend, imgFlags);
            if(texCache == NULL) outFlags |= INIT_TEX;
        }

    return outFlags;
}

void close() {
    LINEOUT("Closing program...");

    texCache->flush();
    texCache = NULL;

    LINEOUT("Destroying window...");
    SDL_DestroyWindow(mainWin);
    mainWin = NULL;

    LINEOUT("Destroying renderer...");
    SDL_DestroyRenderer(mainRend);
    mainRend = NULL;

    SDL_Quit();
}

void handleEvents(SDL_Event* e, int* flags) {
    while(SDL_PollEvent(e)) {
        if(e->type == SDL_QUIT) *flags |= E_QUIT;
    }
    if(*flags > 0) LINEOUT(*flags);
}