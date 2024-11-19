#include "raylib.h"
#include "gameutils.h"
#include "remusutils.h"
#include "raylibutils.h"
#include <iostream>
#include <map>

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
std::map<std::string, UI_RectButton> buttons;

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
    void initButtons();
    void initWorkGrid();

void draw();
    void drawSquares();
    void drawToolbar();
    void drawPalette();
    void drawButtons();

void update();
    void updInput(int);
    void updButts(Vector2);

void callButt(std::string);

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

    initButtons();

    return flag;
}

void initButtons() {
    buttons.insert({"Change Layers",
                    UI_RectButton{"icon_layerswitch.png",
                                  Rectangle{(winDim.x - (paletteSpace.width/2)) - (toolSpace.height/2), 0, toolSpace.height, toolSpace.height},
                                  Rectangle{(float)(((state & 12)/4) - 1)*16, 0, 16, 16}, BUTT_ISIDLE}});
    buttons.insert({"New Map",
                    UI_RectButton{"icon_new.png",
                                  Rectangle{toolSpace.height*(0.05f + 0.0f), toolSpace.height*0.05f, toolSpace.height*0.9f, toolSpace.height*0.9f},
                                  Rectangle{0, 0, 16, 16}, BUTT_ISIDLE}});
    buttons.insert({"Open...",
                    UI_RectButton{"icon_open.png",
                                  Rectangle{toolSpace.height*(0.05f + 1.0f), toolSpace.height*0.05f, toolSpace.height*0.9f, toolSpace.height*0.9f},
                                  Rectangle{0, 0, 16, 16}, BUTT_ISIDLE}});
    buttons.insert({"Save",
                    UI_RectButton{"icon_save.png",
                                  Rectangle{toolSpace.height*(0.05f + 2.0f), toolSpace.height*0.05f, toolSpace.height*0.9f, toolSpace.height*0.9f},
                                  Rectangle{0, 0, 16, 16}, BUTT_ISIDLE}});
    buttons.insert({"Save As...",
                    UI_RectButton{"icon_saveas.png",
                                  Rectangle{toolSpace.height*(0.05f + 3.0f), toolSpace.height*0.05f, toolSpace.height*0.9f, toolSpace.height*0.9f},
                                  Rectangle{0, 0, 16, 16}, BUTT_ISIDLE}});
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
        //draw buttons
        drawButtons();
    EndDrawing();
}

void drawPalette() {
    DrawRectangleRec(paletteSpace, BLACK);
    DrawRectangleLinesEx(paletteSpace, 1.0f, WHITE);
}

void drawToolbar() {
    DrawRectangleRec(toolSpace, LIGHTGRAY);
}

void drawButtons() {
    Vector2 mousePos = GetMousePosition();

    for(const auto & pair : buttons) {
        DrawTexturePro(texCache->cache.at(pair.second.tex), pair.second.srcRect, pair.second.destRect, Vector2{0, 0}, 0.0, WHITE);
        if(pair.second.state != BUTT_ISIDLE) {
            if(IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
                DrawRectangleRec(pair.second.destRect, Color{0, 0, 0, 40});
            } else DrawRectangleRec(pair.second.destRect, Color{255, 255, 255, 40});
        }
    }

    for(const auto & pair : buttons) {
        if(pair.second.state != BUTT_ISIDLE) {
            const char * buttName = pair.first.c_str();
            int strLen = MeasureText(buttName, 15);
            if(mousePos.x + strLen > winDim.x) {
                DrawRectangle(mousePos.x - strLen - 15, mousePos.y - 2, MeasureText(buttName, 15) + 10, 17, DARKGRAY);
                DrawText(buttName, mousePos.x - strLen - 10, mousePos.y, 15, WHITE);
            } else {
                DrawRectangle(mousePos.x + 10, mousePos.y - 2, MeasureText(buttName, 15) + 10, 17, DARKGRAY);
                DrawText(buttName, mousePos.x + 15, mousePos.y, 15, WHITE);
            }
        }
    }
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
    updButts(GetMousePosition());

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
                    for(auto pair : texCache->cache) {
                        LINEOUT(pair.first);
                    }
                    break;
                default:
                    break;
            } else {
                switch(key) {
                    case KEY_N:
                        callButt("New Map");
                        break;
                    default:
                        break;
                }
            }
            
            buttons.at("Change Layers").srcRect.x = (float)(((state & 12)/4) - 1)*16;
    }
        
}

void updButts(Vector2 mpos) {
    for(auto & pair : buttons) {
        if(CheckCollisionPointRec(mpos, pair.second.destRect)) {
            if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
                pair.second.state = BUTT_ISPRESS;
            else
                pair.second.state = BUTT_ISHOVER;
        } else {
            pair.second.state = BUTT_ISIDLE;
        }
    }

    // "Change Layers" button functionality
    if(buttons.at("Change Layers").state == BUTT_ISPRESS) callButt("Change Layers");

    if(buttons.at("New Map").state == BUTT_ISPRESS) callButt("New Map");
}

void callButt(std::string buttStr) {
    if(buttons.count(buttStr) != 0) {
        auto butt = buttons.at(buttStr);

        if(buttStr == "Change Layers") {
            if((state & 0b1100) == ST_CEIL) updInput(KEY_F);
            else if((state & 0b1100) == ST_WALL) updInput(KEY_V);
            else if((state & 0b1100) == ST_FLOOR) updInput(KEY_R);
        }

        if(buttStr == "New Map") {
            delete workingMap;
            workingMap = new RemusMap();
            workingMap->loadFileNames("./resources/wall_textures", "./resources/spr_textures");
        }
    }
}

void close() {
    LINEOUT("Closing program...");
    delete workingMap;

    LINEOUT("Flushing cache...");
    texCache->flush();
    delete texCache;

    UnloadRenderTexture(workTex);

    LINEOUT("Destroying window...");
    CloseWindow();
}