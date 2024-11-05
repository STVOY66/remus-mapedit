#include "raylib.h"
#include "gameutils.h"
#include "remusutils.h"
#include "raylibutils.h"
#include <iostream>

#define LINEOUT(A) std::cout << A << '\n'

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

enum {
    ST_DRAW_CLR = 0b0001,
    ST_SQR_SPR = 0b0010,
    ST_WALL = 0b0100,
    ST_CEIL = 0b1000,
    ST_FLOOR = 0b1100
};

Vector2i winDim = {800, 600};
Rectangle workSpace = {0, (float)winDim.y*0.05f, (float)winDim.x*0.9f, (float)winDim.y*0.95f};
char state;

RemusMap *workingMap = NULL;
TexCache *texCache = NULL;
RenderTexture2D workTex;
Rectangle workRect;
float zoomScale;
float zoomDelt;

Vector2 mouseDelt;
Vector2i mouseMap;
bool mInSpace;

bool init();
void initWorkGrid();

void draw();
void drawSquares();
void drawToolBar();

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

    workingMap->mapSquareData.push_back({Vector2i{0, 0}, true, -1, -1, 2});

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
        DrawRectangleLines(workSpace.width, workSpace.y, 80, 570, WHITE);
        //draw toolbar
        DrawRectangle(0, 0, winDim.x, (float)winDim.y*0.05f, LIGHTGRAY);
        Vector2 mousePos = GetMousePosition();
        DrawText(TextFormat("MP: (%.3f, %.3f) ; RP: (%.3f, %.3f) ; GP: (%d, %d)", mousePos.x, mousePos.y - workSpace.y, workRect.x, workRect.y, int(mousePos.x + workRect.x)/65, int(mousePos.y - workSpace.y + workRect.y)/65), 0, 0, 20, BLACK);
        //draw workspace
        DrawTexturePro(workTex.texture, workRect, workSpace, Vector2{0, 0}, 0.0, WHITE);
    EndDrawing();
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
            if(sqr.IfloorTex != -1)
                DrawTexturePro(texCache->cache.at(surfNames->at(sqr.IfloorTex)),
                               Rectangle{0, 0, 64, -64},
                               Rectangle{(float)sqr.pos.x*65+1, 4161-(float)sqr.pos.y*65-65, 64, 64},
                               Vector2{0, 0}, 0.0, ((state & 0b1100) == ST_FLOOR) ? WHITE : Color{255, 255, 255, 64});
        }
        //Draw working tile
        if(mInSpace)
            DrawTexturePro(texCache->cache.at(surfNames->front()),
                           Rectangle{0, 0, 64, -64},
                           Rectangle{(float)mouseMap.x*65+1, 4161-(float)mouseMap.y*65-65, 64, 64},
                           Vector2{0, 0}, 0.0, Color{255, 255, 255, 64});
    EndTextureMode();
}

void update() {
    int penI = 0;

    RemusMapSquare palSqr = {mouseMap, state >> 2 == ST_WALL, (state >> 2 == ST_FLOOR) ? penI : -1, (state >> 2 == ST_CEIL) ? penI : -1, (state >> 2 == ST_WALL) ? penI : -1};
    Rectangle newRect;

    mInSpace = CheckCollisionPointRec(GetMousePosition(), workSpace);
    mouseMap = {int(GetMousePosition().x*zoomScale + workRect.x)/65, int((GetMousePosition().y - workSpace.y)*zoomScale + workRect.y)/65};
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

    switch (GetKeyPressed()) {
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

    if(mouseWheel != 0 && mInSpace) {
        if(zoomScale - mouseWheel*zoomDelt > 0.4f && zoomScale - mouseWheel*zoomDelt < 6.0f) zoomScale -= mouseWheel*zoomDelt;
    }

    if(zoomScale > 1.5f) zoomDelt = 0.25f;
    else zoomDelt = 0.05f;

    newRect.width = workSpace.width*zoomScale;
    newRect.height = workSpace.height*zoomScale;
    newRect.x = workRect.x - mouseDelt.x*zoomScale;
    newRect.y = workRect.y - mouseDelt.y*zoomScale;

    workRect = newRect;
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