#pragma once
#include "Mesh.h"

class Cylinder : public Mesh {
public:
    Cylinder(int sectors = 20, int stacks = 1, float height = 1.0f, float radius = 0.5f);
};