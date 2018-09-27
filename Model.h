#pragma once

#include "Common.h"
#include "Vector.h"

#include <vector>


// data for OBJ format
struct OBJ {
    const char* name = nullptr;
    std::vector<float3> vertices   {};   // v
    std::vector<float3> normals    {};    // vn
    std::vector<float2> tex_coords {}; // vt
};


namespace Model {

OBJ LoadOBJ(const char* file_name);

}