#include "utils.hpp"

#include <algorithm>

Vector2 RectOrigin(Rectangle rec) {
    return Vector2 { rec.x, rec.y };
}

Vector2 RectEnd(Rectangle rec) {
    return Vector2 { rec.x + rec.width, rec.y + rec.height };
}

float Center(float objectSize, Vector2 region) {
    return (region.x + region.y - objectSize) / 2;
}

Vector2 Center(Vector2 objectSize, Rectangle region) {
    return {
        Center(objectSize.x, Vector2 { region.x, region.x + region.width }),
        Center(objectSize.y, Vector2 { region.y, region.y + region.height })
    };
}

float IntersectionArea(Rectangle r1, Rectangle r2) {
    Vector2 upperLeft = {
        std::max(r1.x, r2.x),
        std::max(r1.y, r2.y),
    };
    Vector2 lowerRight = {
        std::min(r1.x + r1.width, r2.x + r2.width),
        std::min(r1.y + r1.height, r2.y + r2.height),
    };
    float xSideLen = lowerRight.x - upperLeft.x;
    float ySideLen = lowerRight.y - upperLeft.y;
    if (xSideLen > 0 && ySideLen > 0) {
        return xSideLen * ySideLen;
    } else {
        return 0;
    }
}