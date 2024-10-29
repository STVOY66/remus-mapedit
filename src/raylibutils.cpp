#include "raylib.h"
#include "gameutils.h"
#include "raylibutils.h"
#include <filesystem>
#include <iostream>
#include <string>
#include <map>

namespace fs = std::filesystem;

void ScaleRect(Rectangle* rect, float scale) {
    rect->height *= scale; rect->width *= scale;
}

/******** TEXCACHE ********/

TexCache::TexCache() : filetypes {".jpg", ".jpeg", ".png", ".bmp"} {}

TexCache::~TexCache() {
    flush();
}

void TexCache::loadImage(fs::path workPath) {
    bool supported = false;
    std::string pathExt = workPath.extension().string();
    if(cache.find(workPath.filename().string()) == cache.end() && !fs::is_directory(workPath)){
        std::cout << "Loading " << workPath << " into cache..." << std::endl;
        for(std::string ext : filetypes) {
            if(pathExt == ext) { supported = true; break; }
        }

        if(supported) {
            cache.insert({workPath.filename().string(), LoadTexture(workPath.string().c_str())});
            std::cout << workPath << " loaded successfully." << std::endl;
        }
    }
}

void TexCache::loadDir(fs::path workPath) {
    std::cout << "Loading directory \"" << workPath.string() << "\" into texture cache..." << std::endl;
    for( fs::directory_entry entry : fs::directory_iterator(workPath) ) {
        if(fs::is_directory(entry)) loadDir(entry.path());
        else loadImage(entry.path());
    }
}

std::pair<const std::string, Texture>* TexCache::atIndex(int index) {
    auto it = cache.begin();
    std::advance(it, index);
    return &*it;
}

void TexCache::flush() {
    std::cout << "Flushing image cache..." << std::endl;
    if(!cache.empty()) {    
        auto i = cache.begin();
        for(;i != cache.end(); i++)
            UnloadTexture(i->second);
        cache.clear();
    }
}