#pragma once
#include "raylib.h"
#include "gameutils.h"
#include <filesystem>
#include <vector>
#include <string>
#include <map>

namespace fs = std::filesystem;

enum buttState {
    BUTT_ISIDLE,
    BUTT_ISHOVER,
    BUTT_ISPRESS
};

struct UI_RectButton {
    std::string tex;
    Rectangle destRect;
    Rectangle srcRect;
    buttState state;
};

void ScaleRect(Rectangle*, float);

// Class for texture storage
class TexCache {
    public:
        std::map<std::string, Texture> cache;

        TexCache();
        ~TexCache();

        void loadDir(fs::path);
        void loadImage(fs::path);
        std::pair<const std::string, Texture>* atIndex(int);
        void flush();

    private:
        std::string filetypes[4];
};