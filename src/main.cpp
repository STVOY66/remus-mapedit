#include "raylib.h"
#include "gameutils.h"
#include "remusutils.h"
#include "raylibutils.h"
#include <iostream>

#define LINEOUT(A) std::cout << A << '\n' 

Vector2i winDim = {800, 600};
Rectangle workSpace = {0, (float)winDim.y*0.05f, (float)winDim.x*0.9f, (float)winDim.y};


RemusMap *workingMap = NULL;
TexCache *texCache = NULL;
RenderTexture2D workTex;
Rectangle workRect;
float zoomScale;

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
    
    if(!init()) {
        LINEOUT("ERROR: Failed to initialize program.");
        return -1;
    }
    NPatchInfo testPatch = {Rectangle{0, 0, 400, 400}, 0, 0, 0, 0, NPATCH_NINE_PATCH};

    workingMap->mapSquareData.push_back({Vector2i{0, 0}, true, 0, 0, 2});

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

    workingMap = new RemusMap();
    workingMap->loadFileNames("./resources/wall_textures", "./resources/spr_textures");
    for(std::string surf : workingMap->surfTexNames) LINEOUT(surf);
    zoomScale = 0.5f;
    workRect = Rectangle{0, -(float)winDim.y*zoomScale, ((float)winDim.x-80)*zoomScale, -(float)winDim.y*zoomScale};
    workTex = LoadRenderTexture(4161, 4161);
    texCache = new TexCache();
    texCache->loadDir("./resources");
    
    return flag;
}

void initWorkGrid() {
    BeginTextureMode(workTex);
        ClearBackground(DARKGRAY);
        for(int x = 1; x < 4160; x += 65) {
            for(int y = 1; y < 4160; y += 65) {
                DrawRectangle(x, y, 64, 64, BLACK);
            }
        }
    EndTextureMode();
}

void draw() {
    BeginDrawing();
        drawSquares();
        //draw palette space
        DrawRectangleLines(workSpace.width, workSpace.y, 80, 570, WHITE);
        //draw toolbar
        DrawRectangle(0, 0, winDim.x, (float)winDim.y*0.05f, LIGHTGRAY);
        //draw workspace
        DrawTexturePro(workTex.texture, workRect, workSpace, Vector2{0, 0}, 0.0, WHITE);
    EndDrawing();
}

void drawSquares() {
    std::vector<std::string>* surfNames = &workingMap->surfTexNames; 
    std::vector<std::string>* sprNames = &workingMap->sprTexNames; 

    BeginTextureMode(workTex);
        for(auto sqr : workingMap->mapSquareData) {
            DrawTexturePro(texCache->cache.at(surfNames->at(sqr.IwallTex)),
                           Rectangle{0, 0, 64, 64},
                           Rectangle{(float)sqr.pos.x*65+1, (float)sqr.pos.y*65+1, 64, 64},
                           Vector2{0, 0}, 0.0, WHITE);
        }
    EndTextureMode();
}

void update() {
    //mouse drag
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