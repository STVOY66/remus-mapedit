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
void initWorkGrid();

void draw();
void drawSquares();

void update();

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

    workingMap->mapSquareData.push_back({Vector2i{0, 127}, true, 0, 0, 0});

    SetTargetFPS(FPS);
    initWorkGrid();
    
    while(!WindowShouldClose()) {
        draw();
        update();
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

void initWorkGrid() {
    BeginTextureMode(workTex);
        ClearBackground(DARKGRAY);
        for(int x = 1; x < 8320; x += 65) {
            for(int y = 1; y < 8320; y += 65) {
                DrawRectangle(x, y, 64, 64, BLACK);
            }
        }
    EndTextureMode();
}

void draw() {
    BeginDrawing();
        drawSquares();
        DrawTexturePro(workTex.texture, Rectangle{0, 0, 400, 300}, Rectangle{0, 0, 800, 600}, Vector2{0, 0}, 0.0, WHITE);
    EndDrawing();
}

void drawSquares() {
    BeginTextureMode(workTex);
        for(auto sqr : workingMap->mapSquareData) {
            DrawRectangle(sqr.pos.x*65 + 1, sqr.pos.y*65 + 1, 64, 64, WHITE);
        }
    EndTextureMode();
}

void update() {
    
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