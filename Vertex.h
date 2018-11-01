#pragma once

#include "Vector.h" // math-vec
#include <vector>   // stl-vec


struct Vertex {
    float3 position;
    float3 normal;
    float2 tex_coord;
};
using Vertices = std::vector<Vertex>;
