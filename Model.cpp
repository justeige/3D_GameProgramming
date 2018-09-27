#include "Model.h"
#include "Profiling.h"

#include <fstream>
#include <string>
#include <sstream>

using Tokens = std::vector<std::string>;

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

OBJ Model::LoadOBJ(const char* file_name)
{
    measure_time();

    std::ifstream file{ file_name };
    assert(file.is_open());

    OBJ obj{};
    obj.name = file_name;

    for (std::string line{}; std::getline(file, line);/**/) {

        if (line.empty()) { continue; }

        Tokens      const tokens = split(line, ' ');
        std::string const first_token{ tokens[0] };

        // skip comments
        if (first_token == "#") { continue; }

        if (first_token == "v") {
            float3 const vertex = to_vec3(tokens);
            obj.vertices.push_back(vertex);
            continue;
        }

        if (first_token == "vn") {
            float3 const normal = to_vec3(tokens);
            obj.normals.push_back(normal);
            continue;
        }

        if (first_token == "vt") {
            float2 const normal = to_vec2(tokens);
            obj.tex_coords.push_back(normal);
            continue;
        }
    }

    return obj;
}
