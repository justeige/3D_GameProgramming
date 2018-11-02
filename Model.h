#pragma once

#include "Common.h"
#include "Vector.h"
#include "Mesh.h"

#include <vector>


// data for OBJ format
struct OBJ {
    const char* name = nullptr;
    std::vector<float3> vertices   {};   // v
    std::vector<float3> normals    {};    // vn
    std::vector<float2> tex_coords {}; // vt
};

OBJ Load_OBJ(const char* file_name);



// this model data representation should work with every format and is created/imported with Assimp
using Generic_Model = Meshes;
Generic_Model Load_Model(std::string const& path);