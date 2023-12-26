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

bool CheckCollisionPointRec(Rectangle rec, Vector2 point) {
    if (point.x < rec.x || point.y < rec.y) {
        return false;
    }
    if (point.x - rec.x > rec.width || point.y - rec.y > rec.height) {
        return false;
    }
    return true;
}

float IntersectionArea(Rectangle r1, Rectangle r2) {
    Vector2 lowerLeft = {
        std::max(r1.x, r2.x),
        std::max(r1.y, r2.y),
    };
    Vector2 upperRight = {
        std::min(r1.x + r1.width, r2.x + r2.width),
        std::min(r1.y + r1.height, r2.y + r2.height),
    };
    float xSideLen = upperRight.x - lowerLeft.x;
    float ySideLen = upperRight.y - lowerLeft.y;
    if (xSideLen > 0 && ySideLen > 0) {
        return xSideLen * ySideLen;
    } else {
        return 0;
    }
}