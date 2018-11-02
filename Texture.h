#pragma once

#include "Common.h"

#include <string>
#include <vector>

struct Texture {
    uint id;
    std::string path;
    enum Type {
        diffuse,
        specular,
        normal,
        height
    } type;
};
using Textures = std::vector<Texture>;

namespace std {
inline std::string to_string(Texture::Type type)
{
    switch (type) {
    case Texture::diffuse:
        return "texture_diffuse";
    case Texture::specular:
        return "texture_specular";
    case Texture::normal:
        return "texture_normal";
    case Texture::height:
        return "texture_height";
    default:
        break;
    }
    assert(false); // should not be possible!
    return "";
}
}