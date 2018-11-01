#pragma once

#include "Common.h"
#include "Vertex.h"
#include "Texture.h"

#include <vector>

struct Mesh {

    // model specific data
    Vertices vertices = {};
    Indices  indices  = {};
    Textures textures = {};

    // render specific data
    uint VAO = 0;
    uint VBO = 0;
    uint EBO = 0;

};
