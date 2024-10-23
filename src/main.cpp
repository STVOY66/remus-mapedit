#include "raylib.h"
#include "gameutils.h"
#include "remusutils.h"
#include "raylibutils.h"
#include <iostream>

#define LINEOUT(A) std::cout << A << '\n' 

Vector2i winDim = {800, 600};

RemusMap *workingMap = NULL;
TexCache *texCache = NULL;
RenderTexture2D workTex;

bool init();
void drawGrid();
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
    NPatchInfo testPatch = {Rectangle{0, 0, 400, 400}, 0, 0, 0, 0, NPATCH_NINE_PATCH};

    SetTargetFPS(FPS);
    drawGrid();
    while(!WindowShouldClose()) {
        BeginDrawing();
            DrawTexturePro(workTex.texture, Rectangle{0, 0, 400, 300}, Rectangle{0, 0, 800, 600}, Vector2{0, 0}, 0.0, WHITE);
        EndDrawing();
    }

    // Image exportImage = LoadImageFromTexture(workTex.texture);
    // ExportImage(exportImage, "frame.png");
    // UnloadImage(exportImage);
    
    
    close();
    return 0;
}

bool init() {
    bool flag = true;
    LINEOUT("Initializing window...");
    InitWindow(winDim.x, winDim.y, "RemMapEdit");
    if(!IsWindowReady()) flag = true;

    workTex = LoadRenderTexture(8321, 8321);
    texCache = new TexCache();
    
    return flag;
}

void drawGrid() {
    BeginTextureMode(workTex);
        ClearBackground(DARKGRAY);
        for(int x = 1; x < 8320; x += 65) {
            for(int y = 1; y < 8320; y += 65) {
                DrawRectangle(x, y, 64, 64, BLACK);
            }
        }
    EndTextureMode();
}

void close() {
    LINEOUT("Closing program...");

    LINEOUT("Flushing cache...");
    texCache->flush();
    texCache = NULL;

    UnloadRenderTexture(workTex);

    LINEOUT("Destroying window...");
    CloseWindow();
}