#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "gameutils.h"
#include "sdl2utils.h"
#include "remusutils.h"
#include <iostream>

RemusMap *workingMap = NULL;

int main(int argc, char **argv) {
    workingMap = new RemusMap(
        "testmap", 
        "resources\\spr_textures", 
        "resources\\wall_textures"
        );
    
    

    return 0;
}