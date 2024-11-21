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

class rlDialogBox {
    public:
        rlDialogBox();
        rlDialogBox(std::string);
        rlDialogBox(std::string, int);
        
        void getInput(std::string&);
        void getInput(std::string&, int);
        void getInput(std::string, std::string&);
        void getInput(std::string, std::string&, int);

        void changeAnswerLimit(int);
        void changeQuestion(std::string);
    private:
        std::string question;
        int answerLength, quesLines, winHeight, winWidth,
            boxWidth, boxHeight, boxPosX, boxPosY,
            fontSize, inputWidth;
        bool quit;

        void draw();
        void update();

        int numQuesLines();
};