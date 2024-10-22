#pragma once
#include "SDL2/SDL.h"
#include "gameutils.h"
#include <filesystem>
#include <vector>
#include <string>
#include <map>

namespace fs = std::filesystem;

// ******** RGBA8888 color constants ********
#define BLACK 0x000000FF;
#define GRAY 0x7D7D7DFF;
#define WHITE 0xFFFFFFFF;
#define RED 0xFF0000FF;
#define DARK_RED 0x7D0000FF;
#define GREEN 0x00FF00FF;
#define DARK_GREEN 0x007D00FF;
#define BLUE 0x0000FFFF;
#define DARK_BLUE 0x00007DFF;

// Retrieve pixel data from position
Uint32 getPixelData(Uint32*, int, int, int);

void darkenPixelRGBA8888(Uint32*, int);

// Class for texture storage
class TexCache {
    public:
        std::map<std::string, SDL_Texture*> cache;

        TexCache(SDL_Renderer*);
        TexCache(SDL_Renderer*, int);
        TexCache(SDL_Renderer*, std::string, int);
        ~TexCache();

        void loadDir(fs::path);
        void loadImage(fs::path);
        int getWidth(std::string);
        int getWidth(int);
        int getHeight(std::string);
        int getHeight(int);
        Vector2i getDim(int);
        Vector2i getDim(std::string);
        std::pair<std::string, SDL_Texture*> atIndex(int);
        void flush();
    private:
        std::vector<std::string> filetypes;
        unsigned int flags;
        SDL_Renderer* programRender;

        void flag2str();
};

// Class for image storage
class ImgCache {
    public:
        std::map<std::string, SDL_Surface*> cache;

        ImgCache();
        ImgCache(int);
        ImgCache(std::string, int);
        ~ImgCache();

        void loadDir(fs::path);
        void loadImage(fs::path);
        int getWidth(std::string);
        int getWidth(int);
        int getHeight(std::string);
        int getHeight(int);
        Vector2i getDim(int);
        Vector2i getDim(std::string);
        std::pair<std::string, SDL_Surface*> atIndex(int);
        void flush();
    private:
        std::vector<std::string> filetypes;
        unsigned int flags;

        void flag2str();
};