#pragma once
#include "gameutils.h"
#include <bitset>
#include <vector>
#include <string>

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

struct RemusMapSquare {
    Vector2i pos;
    bool isWall;
    int IfloorTex;
    int IceilTex;
    int IwallTex;
};

class RemusMap {
    public:
        RemusMap();
        RemusMap(std::string);
        RemusMap(std::string, std::string, std::string);
        RemusMap(std::string, std::string, std::string, Vector2f);

        std::vector<std::string> surfTexNames;
        std::vector<std::string> sprTexNames;

        std::string GetName();
        void SetName(std::string);
        Vector2f GetStartPos();

        void SquarePush(RemusMapSquare);
        void SpritePush(RemusSprite);

        bool WallCheck(int, int);
        bool FloorCheck(int, int);
        bool CeilCheck(int, int);
    private:
        Vector2f startPos;
        std::vector<RemusMapSquare> mapSquareData;
        std::vector<RemusSprite> mapSpriteData;
        std::string name;
        std::bitset<128> isWallPos[128]; //max size is 128x128
        std::bitset<128> isFloorPos[128];
        std::bitset<128> isCeilPos[128];

        void loadFileNames(std::string, std::string);
};