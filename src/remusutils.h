#pragma once
#include "gameutils.h"

enum HitType {
    NS,
    EW
};

enum RenderType {
    SURFACE,
    RENDERER
};

struct CastRay {
    Vector2f dir;
    double dist;
    HitType side;
    int mapI;
    double wallX;
};

struct RemusSprite : Sprite {
    int vPos;
    double dist;
    Vector2f ldir;
};