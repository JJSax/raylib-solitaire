#pragma once

#include <raylib.h>

Vector2 RectOrigin(Rectangle rec);
Vector2 RectEnd(Rectangle rec);

float Center(float objectSize, Vector2 region);
Vector2 Center(Vector2 objectSize, Rectangle region);