#pragma once
#include <string>

// ******** Vector utilities ********

struct Vector2f {
    float x;
    float y;
};

struct Vector2i {
    int x;
    int y;
};

// Returns length of 2D Vector
float fVector2Length(Vector2f);

// Normalizes 2D Vector
Vector2f fVector2Normalize(Vector2f);

// Rotates vector by radians
Vector2f fVector2Rotate(Vector2f, float);

// ******** Sprites ********

struct Sprite {
    float x;
    float y;
    int w;
    int h;
    std::string texName;
};