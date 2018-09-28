#include "Model.h"
#include "Profiling.h"

#include <fstream>
#include <string>
#include <sstream>
#include <array>

using Tokens = std::vector<std::string>;
using Triplet   = std::array<int, 3>;

constexpr std::size_t V  = 0;
constexpr std::size_t UV = 1;
constexpr std::size_t VT = 2;

Tokens split(std::string text, char const delimiter)
{
    std::stringstream ss { text };
    std::vector<std::string> tokens {};

    for (std::string token{}; std::getline(ss, token, delimiter);/**/) {
        if (token.empty()) {
            return {};
        }
        tokens.push_back(token);
    }

    return std::move(tokens);
}

float3 to_vec3(Tokens const& tokens)
{
    assert(tokens.size() == 4);
    float const x = std::stof(tokens[1].c_str());
    float const y = std::stof(tokens[2].c_str());
    float const z = std::stof(tokens[3].c_str());
    return { x, y, z };
}

float2 to_vec2(Tokens const& tokens)
{
    assert(tokens.size() == 3);
    float const x = std::stof(tokens[1].c_str());
    float const y = std::stof(tokens[2].c_str());
    return { x, y };
}

Triplet to_triplet(std::string line)
{
    auto tokens = split(line, '/');
    assert(tokens.size() == 3);
    return {
        std::atoi(tokens[0].c_str()),
        std::atoi(tokens[1].c_str()),
        std::atoi(tokens[2].c_str())
    };
}


OBJ Model::LoadOBJ(const char* file_name)
{
    measure_time();

    std::ifstream file{ file_name };
    assert(file.is_open());

    // obj format uses indices to address positions, normals etc.
    // this way the information can be reduced...
    // example: a cube with 36 vertices can be saved with only 8 vertices (because of the overlapp @ the corners!)
    Indices v_i, uv_i, vt_i;
    std::vector<float3> temp_vertices, temp_normals;
    std::vector<float2> temp_tex;

    for (std::string line{}; std::getline(file, line);/**/) {

        if (line.empty()) { continue; }

        Tokens      const tokens = split(line, ' ');
        std::string const type{ tokens[0] };

        // skip comments
        if (type == "#") { continue; }

        if (type == "v") {
            float3 const vertex = to_vec3(tokens);
            temp_vertices.push_back(vertex);
            continue;
        }

        if (type == "vn") {
            float3 const normal = to_vec3(tokens);
            temp_normals.push_back(normal);
            continue;
        }

        if (type == "vt") {
            float2 const tex = to_vec2(tokens);
            temp_tex.push_back(tex);
            continue;
        }

        if (type == "f") {
            // format: v/uv/vt ...
            Triplet a = to_triplet(tokens[1]);
            Triplet b = to_triplet(tokens[2]);
            Triplet c = to_triplet(tokens[3]);

            v_i.push_back(a[V]);
            v_i.push_back(b[V]);
            v_i.push_back(c[V]);

            uv_i.push_back(a[UV]);
            uv_i.push_back(b[UV]);
            uv_i.push_back(c[UV]);

            vt_i.push_back(c[VT]);
            vt_i.push_back(c[VT]);
            vt_i.push_back(c[VT]);
        }
    }

    // sanity check, the triples have to add up
    assert(v_i.size() == vt_i.size());
    assert(v_i.size() == uv_i.size());

    // process the index data and create the OBJ struct
    OBJ obj{};
    obj.name = file_name;

    for_size (n, v_i) {
        int    const vertex_index = v_i[n];
        float3 const vertex = temp_vertices[vertex_index - 1]; // obj starts index with 1, c++ with 0!
        obj.vertices.push_back(vertex);

        int    const normal_index = vt_i[n];
        float3 const normal = temp_normals[normal_index - 1]; // obj starts index with 1, c++ with 0!
        obj.normals.push_back(normal);

        int    const tex_index = uv_i[n];
        float2 const tex_coord = temp_tex[tex_index - 1]; // obj starts index with 1, c++ with 0!
        obj.tex_coords.push_back(tex_coord);
    }

    return obj;
}
