#include "Model.h"
#include "Profiling.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <glad/glad.h>
#include <stb/stb_image.h>

#include <fstream>
#include <string>
#include <sstream>
#include <array>
#include <set>

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


OBJ Load_OBJ(const char* file_name)
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


// Assimp import

uint Texture_From_File(char const* path, std::string const& directory, bool gamma = false)
{
    const std::string filename = directory + '/' + std::string{ path };

    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, component_count;
    unsigned char *data = stbi_load(filename.c_str(), &width, &height, &component_count, 0);
    if (data) {
        GLenum format;
        if (component_count == 1) {
            format = GL_RED;
        }
        else if (component_count == 3) {
            format = GL_RGB;
        }
        else if (component_count == 4) {
            format = GL_RGBA;
        }

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

// the required info is returned as a Texture struct.
Textures Load_Texture(aiMaterial *mat, aiTextureType type, Texture::Type ttype, std::string const& directory)
{
    static std::vector<Texture> texture_cache;
    Textures textures;

    for (uint i = 0; i < mat->GetTextureCount(type); i++) {
        aiString str;
        mat->GetTexture(type, i, &str);
        // check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
        bool skip = false;
        const std::string current_tex { str.C_Str() };
        for (uint j = 0; j < texture_cache.size(); j++) {
            const std::string cached_tex{ texture_cache[j].path.data() };
            if (cached_tex == current_tex) {
                textures.push_back(texture_cache[j]);
                skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
                break;
            }
        }
        if (!skip) {
            // if texture hasn't been loaded already, load it
            Texture texture;
            texture.id = Texture_From_File(str.C_Str(), directory);
            texture.type = ttype;
            texture.path = str.C_Str();
            textures.push_back(texture);
            texture_cache.push_back(texture);  // store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
        }
    }
    return textures;
}


Mesh Process_Mesh(aiMesh *mesh, aiScene const* scene, std::string const& directory)
{
    Vertices vertices {};
    Indices  indices  {};
    Textures textures {};

    for (uint i = 0; i < mesh->mNumVertices; i++) {
        Vertex vertex {};

        // positions
        vertex.position.x = mesh->mVertices[i].x;
        vertex.position.y = mesh->mVertices[i].y;
        vertex.position.z = mesh->mVertices[i].z;

        // normals
        vertex.normal.x = mesh->mNormals[i].x;
        vertex.normal.y = mesh->mNormals[i].y;
        vertex.normal.z = mesh->mNormals[i].z;

        // texture coordinates
        if (mesh->mTextureCoords[0]) { // does the mesh contain texture coordinates?
            // a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't
            // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
            vertex.tex_coord.x = mesh->mTextureCoords[0][i].x;
            vertex.tex_coord.y = mesh->mTextureCoords[0][i].y;
        }
        else {
            vertex.tex_coord = {};
        }

        // tangent
        if (mesh->mTangents != nullptr) {
            vertex.tangent.x = mesh->mTangents[i].x;
            vertex.tangent.y = mesh->mTangents[i].y;
            vertex.tangent.z = mesh->mTangents[i].z;
        }
        else {
            vertex.tangent = {}; // default to 0,0,0
        }

        // bitangent
        if (mesh->mBitangents != nullptr) {
            vertex.bitangent.x = mesh->mBitangents[i].x;
            vertex.bitangent.y = mesh->mBitangents[i].y;
            vertex.bitangent.z = mesh->mBitangents[i].z;
        }
        else {
            vertex.bitangent = {}; // default to 0,0,0
        }

        vertices.push_back(vertex);
    }

    // process indices
    for (uint n = 0; n < mesh->mNumFaces; ++n) {
        aiFace face = mesh->mFaces[n];
        for (uint i = 0; i < face.mNumIndices; ++i) {
            indices.push_back(face.mIndices[i]);
        }
    }

    // process material
    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
    // naming convention:
    // diffuse: texture_diffuseN
    // specular: texture_specularN
    // normal: texture_normalN
    // N is a number between 1 and MAX_SAMPLER_NUMBER

    Textures diffuseMaps = Load_Texture(material, aiTextureType_DIFFUSE, Texture::diffuse, directory);
    textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

    Textures specularMaps = Load_Texture(material, aiTextureType_SPECULAR, Texture::specular, directory);
    textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

    Textures normalMaps = Load_Texture(material, aiTextureType_HEIGHT, Texture::normal, directory);
    textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

    Textures heightMaps = Load_Texture(material, aiTextureType_AMBIENT, Texture::height, directory);
    textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

    return Mesh { vertices, indices, textures };
}

void Process_Node(Meshes& meshes, aiNode* node, aiScene const* scene, std::string const& directory)
{
    // process all the node's meshes (if any)
    for (uint n = 0; n < node->mNumMeshes; n++) {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[n]];
        meshes.push_back(Process_Mesh(mesh, scene, directory));
    }
    // then do the same for each of its children
    for (uint n = 0; n < node->mNumChildren; n++) {
        Process_Node(meshes, node->mChildren[n], scene, directory);
    }
}

Generic_Model Load_Model(std::string const& path)
{
    Assimp::Importer import;
    const aiScene *scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cerr << "ERROR::ASSIMP::" << import.GetErrorString() << '\n';
        return {};
    }
    std::string directory = path.substr(0, path.find_last_of('/'));

    Meshes meshes {};
    Process_Node(meshes, scene->mRootNode, scene, directory);
    return meshes;
}
