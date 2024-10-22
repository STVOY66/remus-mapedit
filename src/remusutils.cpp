#include "gameutils.h"
#include "remusutils.h"
#include <filesystem>
#include <bitset>
#include <vector>
#include <string>

namespace fs = std::filesystem;

RemusMap::RemusMap() {
    name = "untitled";
    startPos = Vector2f{64, 64};
}

RemusMap::RemusMap(std::string n) {
    name = n;
    startPos = Vector2f{64, 64};
}

RemusMap::RemusMap(std::string n, std::string sprDir, std::string surfDir) {
    name = n;
    startPos = Vector2f{64, 64};
    loadFileNames(surfDir, sprDir);
}

RemusMap::RemusMap(std::string n, std::string sprDir, std::string surfDir, Vector2f pos) {
    name = n;
    startPos = pos;
    loadFileNames(surfDir, sprDir);
}

void RemusMap::loadFileNames(std::string surfDir, std::string sprDir) {
    fs::path workSprPath = sprDir;
    fs::path workSurfPath = surfDir;
    const std::string supportedExt[] = {".jpg", ".jpeg", ".png", ".tif", ".tiff", ".webp", ".jxl", ".avif", ".avifs"};

    if(fs::exists(workSprPath))
    for(auto entry : fs::directory_iterator(workSprPath)) {
        if(!fs::is_directory(entry.path())) {
            std::string pathExt = entry.path().extension().string();

            for(std::string ext : supportedExt) {
                if(pathExt == ext) {
                    sprTexNames.push_back(entry.path().filename().string());
                    break;
                }
            }
        }
    }

    if(fs::exists(workSurfPath))
    for(auto entry : fs::directory_iterator(workSurfPath)) {
        if(!fs::is_directory(entry.path())) {
            std::string pathExt = entry.path().extension().string();

            for(std::string ext : supportedExt) {
                if(pathExt == ext) {
                    surfTexNames.push_back(entry.path().filename().string());
                    break;
                }
            }
        }
    }
}