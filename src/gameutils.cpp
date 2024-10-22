#include "gameutils.h"
#include <cmath>

float fVector2Length(Vector2f vector) {
    return sqrtf((vector.x * vector.x) + (vector.y * vector.y));
}

Vector2f fVector2Normalize(Vector2f vector) {
    float len = fVector2Length(vector);
    return Vector2f{vector.x/len, vector.y/len};
}

Vector2f fVector2Rotate(Vector2f vector, float amt) {
    float sinAmt = sinf(amt);
    float cosAmt = cosf(amt);
    Vector2f output = {(vector.x*cosAmt) - (vector.y*sinAmt), (vector.x*sinAmt) + (vector.y*cosAmt)};
    return output;
}