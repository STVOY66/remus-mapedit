#include "raylib.h"
#include "gameutils.h"
#include "remusutils.h"
#include "raylibutils.h"
#include <iostream>

#define LINEOUT(A) std::cout << A << '\n'
#define SETBIT(u, n) (u | (1 << n))
#define CLEARBIT(u, n) (u & ~(1 << n))
#define TOGGLEBIT(u, n) (u ^ (1 << n))

/*
    DRAW | CLEAR , SQR | SPR, WALL | CEIL | FLOOR
         0             0            0     0

    STATE: 0010
    ^DRAWING WALL SQUARES
    STATE: 1011
    ^CLEARING FLOOR SQUARES
    STATE: 0110
    ^UNDEF, SPRITES NOT IMPLEMENTED
    STATE: 0000
    ^TEXTINPUT MODE
*/

// anonymous enums for various flags/signals
enum {
    ST_DRAW_CLR = 0b0001,
    ST_SQR_SPR = 0b0010,
    ST_WALL = 0b0100,
    ST_CEIL = 0b1000,
    ST_FLOOR = 0b1100
};

Vector2i winDim = {1280, 720};
Rectangle workSpace = {0, (float)winDim.y*0.05f, (float)winDim.x*0.9f, (float)winDim.y*0.95f};
Rectangle paletteSpace = {workSpace.width, workSpace.y, (float)winDim.x*0.1f, (float)winDim.y*0.95f};
Rectangle toolSpace = {0, 0, winDim.x, (float)winDim.y*0.05f};
char state;

RemusMap *workingMap = NULL;
TexCache *texCache = NULL;
RenderTexture2D workTex;
Rectangle workRect;
float zoomScale;
float zoomDelt;

Vector2 mouseDelt;
Vector2 mouseWorld;
Vector2i mouseMap;
bool mInSpace;
int penI;


bool init();
void initWorkGrid();

void draw();
void drawSquares();
void drawToolbar();
void drawPalette();

void update();
void updInput(int);

void close();


int main(int argc, char **argv) {
    const int FPS = 60;
    std::string sprDir;
    std::string surfDir;
    
    if(!init()) {
        LINEOUT("ERROR: Failed to initialize program.");
        return -1;
    }

    SetTargetFPS(FPS);
    
    while(!WindowShouldClose()) {
        draw();
        if(state) update();
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
    
    state = 0b0100;
    penI = 0;

    zoomScale = 1.0f;
    zoomDelt = 0.05f;

    workRect = Rectangle{0, 0, (workSpace.width)*zoomScale, -(workSpace.height)*zoomScale};
    workTex = LoadRenderTexture(4161, 4161);
    SetTextureWrap(workTex.texture, 1);

    texCache = new TexCache();
    texCache->loadDir("./resources");
    
    return flag;
}

void initWorkGrid() {
        ClearBackground(DARKGRAY);
        for(int x = 1; x < 4160; x += 65) {
            for(int y = 1; y < 4160; y += 65) {
                DrawRectangle(x, y, 64, 64, BLACK);
            }
        }
}

void draw() {
    BeginDrawing();
        drawSquares();
        //draw palette space
        drawPalette();
        //draw toolbar
        drawToolbar();
        //draw workspace
        DrawTexturePro(workTex.texture, workRect, workSpace, Vector2{0, 0}, 0.0, WHITE);
    EndDrawing();
}

void drawPalette() {
    DrawRectangleLinesEx(paletteSpace, 1.0f, WHITE);
}

void drawToolbar() {
    DrawRectangleRec(toolSpace, LIGHTGRAY);
    
    Rectangle layerDest = Rectangle{(winDim.x - (paletteSpace.width/2)) - (toolSpace.height/2), 0, toolSpace.height, toolSpace.height};
    Rectangle layerSource = Rectangle{(float)(((state & 12)/4) - 1)*16, 0, 16, 16};
    DrawTexturePro(texCache->cache.at("icon_layerswitch.png"), layerSource, layerDest, Vector2{0, 0}, 0, WHITE);
}

void drawSquares() {
    std::vector<std::string>* surfNames = &workingMap->surfTexNames; 
    std::vector<std::string>* sprNames = &workingMap->sprTexNames; 

    BeginTextureMode(workTex);
        initWorkGrid();
        //Draw map squares
        for(auto sqr : workingMap->mapSquareData) {
            if(sqr.IwallTex != -1)
                DrawTexturePro(texCache->cache.at(surfNames->at(sqr.IwallTex)),
                               Rectangle{0, 0, 64, -64},
                               Rectangle{(float)sqr.pos.x*65+1, 4161-(float)sqr.pos.y*65-65, 64, 64},
                               Vector2{0, 0}, 0.0, ((state & 0b1100) == ST_WALL) ? WHITE : Color{255, 255, 255, 64});
            if(sqr.IceilTex != -1)
                DrawTexturePro(texCache->cache.at(surfNames->at(sqr.IceilTex)),
                               Rectangle{0, 0, 64, -64},
                               Rectangle{(float)sqr.pos.x*65+1, 4161-(float)sqr.pos.y*65-65, 64, 64},
                               Vector2{0, 0}, 0.0, ((state & 0b1100) == ST_CEIL) ? WHITE : Color{255, 255, 255, 64});
            if((state & 0b1100) != ST_CEIL)
                if(sqr.IfloorTex != -1)
                    DrawTexturePro(texCache->cache.at(surfNames->at(sqr.IfloorTex)),
                                Rectangle{0, 0, 64, -64},
                                Rectangle{(float)sqr.pos.x*65+1, 4161-(float)sqr.pos.y*65-65, 64, 64},
                                Vector2{0, 0}, 0.0, ((state & 0b1100) == ST_FLOOR) ? WHITE : Color{255, 255, 255, 64});
        }
        //Draw working tile
        if(mInSpace) {
            if((state & ST_DRAW_CLR) == 0)
                DrawTexturePro(texCache->cache.at(surfNames->at(penI)),
                            Rectangle{0, 0, 64, -64},
                            Rectangle{(float)mouseMap.x*65+1, 4161-(float)mouseMap.y*65-65, 64, 64},
                            Vector2{0, 0}, 0.0, Color{255, 255, 255, 64});
            else
                DrawRectanglePro(Rectangle{(float)mouseMap.x*65+1, 4161-(float)mouseMap.y*65-65, 64, 64},
                                 Vector2{0, 0}, 0.0, Color{255, 0, 0, 64});
        }
    EndTextureMode();
}

void update() {
    RemusMapSquare palSqr = {mouseMap, state >> 2 == ST_WALL, (state >> 2 == ST_FLOOR) ? penI : -1, (state >> 2 == ST_CEIL) ? penI : -1, (state >> 2 == ST_WALL) ? penI : -1};
    Rectangle newRect = workRect;

    mInSpace = CheckCollisionPointRec(GetMousePosition(), workSpace);
    mouseWorld = {GetMousePosition().x*zoomScale + workRect.x, (GetMousePosition().y - workSpace.y)*zoomScale + workRect.y};
    mouseMap = {int(mouseWorld.x)/65, int(mouseWorld.y)/65};
    float mouseWheel = GetMouseWheelMove();

    // get mouse delta within workspace
    if(IsMouseButtonDown(MOUSE_BUTTON_RIGHT) && mInSpace) {
        mouseDelt = GetMouseDelta();
    } else mouseDelt = Vector2{0, 0};

    if(IsMouseButtonDown(MOUSE_BUTTON_LEFT) && mInSpace) {
        if((state & ST_SQR_SPR) == 0) {
            bool inVec = false;
            int vecPos;
            for(vecPos = 0; vecPos < workingMap->mapSquareData.size(); vecPos++) {
                if(workingMap->mapSquareData.at(vecPos).pos.x == mouseMap.x && workingMap->mapSquareData.at(vecPos).pos.y == mouseMap.y) {inVec = true; break;}
            }

            if(!inVec && ((state & ST_DRAW_CLR) == 0)) workingMap->mapSquareData.push_back(palSqr);
            else if(inVec){
                RemusMapSquare *currSqr = &workingMap->mapSquareData.at(vecPos);
                if((state & 0b1100) == ST_WALL) {
                    if((state & ST_DRAW_CLR) == 0){currSqr->IwallTex = penI; currSqr->isWall = true;}
                    else if((state & ST_DRAW_CLR) == 1){currSqr->IwallTex = -1; currSqr->isWall = false;}
                }
                if((state & 0b1100) == ST_FLOOR) {
                    if((state & ST_DRAW_CLR) == 0){currSqr->IfloorTex = penI;}
                    else if((state & ST_DRAW_CLR) == 1){currSqr->IfloorTex = -1;}
                }
                if((state & 0b1100) == ST_CEIL) {
                    if((state & ST_DRAW_CLR) == 0){currSqr->IceilTex = penI;}
                    else if((state & ST_DRAW_CLR) == 1){currSqr->IceilTex = -1;}
                }
                if(currSqr->IceilTex == -1 && currSqr->IfloorTex == -1 && currSqr->IwallTex == -1)
                    workingMap->mapSquareData.erase(workingMap->mapSquareData.begin()+vecPos);
            }
        }
    }

    updInput(GetKeyPressed());

    if(mouseWheel != 0) {
        if(IsKeyDown(KEY_LEFT_SHIFT)) {
            if((state & ST_SQR_SPR) == 0) {
                if(penI + mouseWheel >= workingMap->surfTexNames.size()) penI = 0;
                else if(penI + mouseWheel < 0) penI = workingMap->surfTexNames.size() - 1;
                else penI += mouseWheel;
            }
        } else if(mInSpace) {
            if(zoomScale - mouseWheel*zoomDelt > 0.4f && zoomScale - mouseWheel*zoomDelt < 6.0f) zoomScale -= mouseWheel*zoomDelt;
        }
    }

    if(zoomScale > 1.5f) zoomDelt = 0.25f;
    else zoomDelt = 0.05f;

    newRect.width = workSpace.width*zoomScale;
    newRect.height = workSpace.height*zoomScale;

    newRect.x -= mouseDelt.x*zoomScale;
    newRect.y -= mouseDelt.y*zoomScale;

    workRect = newRect;
}

void updInput(int key) {
    if((state & ST_SQR_SPR) == 0 && state != 0) {
        if(!IsKeyDown(KEY_LEFT_CONTROL))
            switch(key) {
                case KEY_R:
                    state &= 0b0011;
                    state |= ST_CEIL;
                    break;
                case KEY_F:
                    state &= 0b0011;
                    state |= ST_WALL;
                    break;
                case KEY_V:
                    state &= 0b0011;
                    state |= ST_FLOOR;
                    break;
                case KEY_E:
                    state |= ST_DRAW_CLR;
                    break;
                case KEY_D:
                    state &= ~ST_DRAW_CLR;
                    break;
                case KEY_P:
                    for(auto sqr : workingMap->mapSquareData) {
                        LINEOUT(sqr.pos.x << " " << sqr.pos.y);
                    }
                    break;
                default:
                    break;
            }
    }
        
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