#include "raylib.h"
#include "gameutils.h"
#include "remusutils.h"
#include "raylibutils.h"
#include <iostream>

#define LINEOUT(A) std::cout << A << '\n' 

Vector2i winDim = {800, 600};

RemusMap *workingMap = NULL;
TexCache *texCache = NULL;

bool init();
void close();


int main(int argc, char **argv) {
    const int FPS = 60;
    std::string sprDir;
    std::string surfDir;
    workingMap = new RemusMap();

    if(!init()) {
        LINEOUT("ERROR: Failed to initialize program.");
        return -1;
    }

    SetTargetFPS(FPS);
    while(!WindowShouldClose()) {
        BeginDrawing();
            ClearBackground(BLACK);
        EndDrawing();
    }
    
    close();
    return 0;
}

bool init() {
    bool flag = true;
    LINEOUT("Initializing window...");
    InitWindow(winDim.x, winDim.y, "RemMapEdit");
    if(!IsWindowReady()) flag = true;

    texCache = new TexCache();
    
    return flag;
}

void close() {
    LINEOUT("Closing program...");

    LINEOUT("Flushing cache...");
    texCache->flush();
    texCache = NULL;

    LINEOUT("Destroying window...");
    CloseWindow();
}