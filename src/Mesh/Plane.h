#pragma once
#include "Mesh.h"

class Plane : public Mesh {
public:
    Plane(int subdivisionsX = 1, int subdivisionsY = 1, float width = 1.0f, float height = 1.0f);
};