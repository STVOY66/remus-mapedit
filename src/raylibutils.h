#pragma once
#include "raylib.h"
#include "gameutils.h"
#include <filesystem>
#include <vector>
#include <string>
#include <map>

namespace fs = std::filesystem;

struct UI_RectButton {
    std::string name;
    std::string tex;
    Rectangle rect;
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