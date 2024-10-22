#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "gameutils.h"
#include "sdl2utils.h"
#include <filesystem>
#include <iostream>
#include <string>
#include <map>

namespace fs = std::filesystem;

Uint32 getPixelData(Uint32* pixels, int pitch, int x, int y) {
    return pixels[y*(pitch/sizeof(Uint32)) + x];
}

void darkenPixelRGBA8888(Uint32 *color, int percent) {
    if(percent > 100) percent = 100;
    short int r = (*color >> 24) & 0xFF, g = (*color >> 16) & 0xFF, b = (*color >> 8) & 0xFF, a = (*color & 0xFF);
    r = (r * percent)/100; g = (g * percent)/100; b = (b * percent)/100;
    *color = ((r << 24) | (g << 16) | (b << 8) | a);
}

/******** TEXCACHE DEFINITIONS ********/

TexCache::TexCache(SDL_Renderer* renderer) {
    this->programRender = renderer;
    this->flags = IMG_INIT_JPG;
    flag2str();
}

TexCache::TexCache(SDL_Renderer* renderer, int flags) {
    this->programRender = renderer;
    this->flags = (flags > 63) ? IMG_INIT_JPG : flags;
    flag2str();
}

TexCache::TexCache(SDL_Renderer* renderer, std::string path, int flags) {
    this->programRender = renderer;
    this->flags = (flags > 63) ? IMG_INIT_JPG : flags;
    flag2str();

    fs::path workPath = path;

    if(fs::exists(workPath)) {
        if(fs::is_directory(workPath)) loadDir(workPath);
        else {
            bool supported = false;
            std::string pathExt = workPath.extension().string();

            for(std::string ext : filetypes) {
                if(pathExt == ext) {
                    supported = true;
                    break;
                }
            }

            loadImage(workPath);
        }
    }
}

TexCache::~TexCache() {
    flush();
}

void TexCache::loadImage(fs::path workPath) {
    bool supported = false;
    std::string pathExt = workPath.extension().string();
    if(cache.find(workPath.filename().string()) == cache.end()){
        std::cout << "Loading " << workPath << " into cache..." << std::endl;
        for(std::string ext : filetypes) {
            if(pathExt == ext) { supported = true; break; }
        }

        if(supported) {
            cache.insert({workPath.filename().string(), IMG_LoadTexture(programRender, workPath.string().c_str())});
            std::cout << workPath << " loaded successfully." << std::endl;
        } else {
            std::cout << "ERROR: Filetype \"" << pathExt << "\" not supported." << std::endl;
        }
    }
}

void TexCache::loadDir(fs::path workPath) {
    std::cout << "Loading directory \"" << workPath.string() << "\" into image cache..." << std::endl;
    for( fs::directory_entry entry : fs::directory_iterator(workPath) ) {
        if(fs::is_directory(entry)) loadDir(entry.path());
        else loadImage(entry.path());
    }
}

void TexCache::flag2str() {
    if(flags & IMG_INIT_JPG) { filetypes.push_back(".jpg"); filetypes.push_back(".jpeg"); }
    if(flags & IMG_INIT_PNG) { filetypes.push_back(".png"); }
    if(flags & IMG_INIT_TIF) { filetypes.push_back(".tif"); filetypes.push_back(".tiff");}
    if(flags & IMG_INIT_WEBP) { filetypes.push_back(".webp"); }
    if(flags & IMG_INIT_JXL) { filetypes.push_back(".jxl"); }
    if(flags & IMG_INIT_AVIF) { filetypes.push_back(".avif"); filetypes.push_back(".avifs"); }
}

int TexCache::getWidth(int index) {
    return getWidth(atIndex(index).first);
}

int TexCache::getWidth(std::string key) {
    int output;
    SDL_QueryTexture(cache.at(key), NULL, NULL, &output, NULL);
    return output;
}

int TexCache::getHeight(int index) {
    return getHeight(atIndex(index).first);
}

int TexCache::getHeight(std::string key) {
    int output;
    SDL_QueryTexture(cache.at(key), NULL, NULL, NULL, &output);
    return output;
}

Vector2i TexCache::getDim(int index) {
    return getDim(atIndex(index).first);
}

Vector2i TexCache::getDim(std::string key) {
    Vector2i output;
    SDL_QueryTexture(cache.at(key), NULL, NULL, &output.x, &output.y);
    return output;
}

std::pair<std::string, SDL_Texture*> TexCache::atIndex(int index) {
    auto it = cache.begin();
    std::advance(it, index);
    return *it;
}

void TexCache::flush() {
    std::cout << "Flushing image cache..." << std::endl;
    std::map<std::string, SDL_Texture*>::iterator i = cache.begin();
    for(;i != cache.end(); i++)
        SDL_DestroyTexture(i->second);
    cache.clear();
}

/******** IMGCACHE DEFINITIONS ********/

ImgCache::ImgCache() {
    this->flags = IMG_INIT_JPG;
    flag2str();
}

ImgCache::ImgCache(int flags) {
    this->flags = (flags > 63) ? IMG_INIT_JPG : flags;
    flag2str();
}

ImgCache::ImgCache(std::string path, int flags) {
    this->flags = (flags > 63) ? IMG_INIT_JPG : flags;
    flag2str();

    fs::path workPath = path;

    if(fs::exists(workPath)) {
        if(fs::is_directory(workPath)) loadDir(workPath);
        else {
            bool supported = false;
            std::string pathExt = workPath.extension().string();

            for(std::string ext : filetypes) {
                if(pathExt == ext) {
                    supported = true;
                    break;
                }
            }

            loadImage(workPath);
        }
    }
}

ImgCache::~ImgCache() {
    flush();
}

void ImgCache::loadImage(fs::path workPath) {
    bool supported = false;
    std::string pathExt = workPath.extension().string();
    if(cache.find(workPath.filename().string()) == cache.end()){
        std::cout << "Loading " << workPath << " into cache..." << std::endl;
        for(std::string ext : filetypes) {
            if(pathExt == ext) { supported = true; break; }
        }

        if(supported) {
            SDL_Surface* workingSurf = IMG_Load(workPath.string().c_str());
            SDL_Surface* optimSurf = SDL_ConvertSurfaceFormat(workingSurf, SDL_PIXELFORMAT_RGBA8888, 0);

            cache.insert({workPath.filename().string(), optimSurf});

            SDL_FreeSurface(workingSurf);
            workingSurf = NULL;

            std::cout << workPath << " loaded successfully." << std::endl;
        } else {
            std::cout << "ERROR: Filetype \"" << pathExt << "\" not supported." << std::endl;
        }
    }
}

void ImgCache::loadDir(fs::path workPath) {
    std::cout << "Loading directory \"" << workPath.string() << "\" into image cache..." << std::endl;
    for( fs::directory_entry entry : fs::directory_iterator(workPath) ) {
        if(fs::is_directory(entry)) loadDir(entry.path());
        else loadImage(entry.path());
    }
}

void ImgCache::flag2str() {
    if(flags & IMG_INIT_JPG) { filetypes.push_back(".jpg"); filetypes.push_back(".jpeg"); }
    if(flags & IMG_INIT_PNG) { filetypes.push_back(".png"); }
    if(flags & IMG_INIT_TIF) { filetypes.push_back(".tif"); filetypes.push_back(".tiff");}
    if(flags & IMG_INIT_WEBP) { filetypes.push_back(".webp"); }
    if(flags & IMG_INIT_JXL) { filetypes.push_back(".jxl"); }
    if(flags & IMG_INIT_AVIF) { filetypes.push_back(".avif"); filetypes.push_back(".avifs"); }
}

int ImgCache::getWidth(int index) {
    return getWidth(atIndex(index).first);
}

int ImgCache::getWidth(std::string key) {
    return cache.at(key)->w;
}

int ImgCache::getHeight(int index) {
    return getHeight(atIndex(index).first);
}

int ImgCache::getHeight(std::string key) {
    return cache.at(key)->h;
}

Vector2i ImgCache::getDim(int index) {
    return getDim(atIndex(index).first);
}

Vector2i ImgCache::getDim(std::string key) {
    return Vector2i{cache.at(key)->w, cache.at(key)->h};
}

std::pair<std::string, SDL_Surface*> ImgCache::atIndex(int index) {
    auto it = cache.begin();
    std::advance(it, index);
    return *it;
}

void ImgCache::flush() {
    std::cout << "Flushing image cache..." << std::endl;
    auto i = cache.begin();
    for(;i != cache.end(); i++)
        SDL_FreeSurface(i->second);
    cache.clear();
}