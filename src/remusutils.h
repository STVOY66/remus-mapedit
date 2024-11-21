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

        std::string name;

        std::vector<std::string> surfTexNames;
        std::vector<std::string> sprTexNames;

        std::vector<RemusMapSquare> mapSquareData;
        std::vector<RemusSprite> mapSpriteData;

        Vector2f GetStartPos();
        void SetStartPos(Vector2f);
        
        void loadFileNames(std::string, std::string);
        void loadMap(std::string);

        void saveMap(std::string);

        bool WallCheck(int, int);
        bool FloorCheck(int, int);
        bool CeilCheck(int, int);
    private:
        Vector2f startPos;
        std::bitset<64> isWallPos[64]; //max size is 64x64
        std::bitset<64> isFloorPos[64];
        std::bitset<64> isCeilPos[64];
};