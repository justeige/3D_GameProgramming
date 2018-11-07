#pragma once

#include "Common.h"
#include "Vector.h"
#include "Vertex.h"
#include "Texture.h"
#include "Mesh.h"

#include <map>
#include <string>
#include <vector>
#include <optional>

struct GLFWwindow;
using Window = GLFWwindow;

using Shader_ID = int;
const Shader_ID Bad_Shader = 0;

using Uniform_Map = std::map<std::string, int>;
using Uniform_List = std::initializer_list<std::string>;

namespace GL {

struct Shader;

// basic functions
Window* Global_Init();     // if the init fails, the function return nullptr
void    Global_Teardown(); // needs to be called at the end of an successfull program
bool    Is_Open(Window* window);
void    Poll_And_Swap(Window* window); // poll for new events and swap the drawing buffer
void    Close_On_Escape(Window* window);
void    Clear_Screen();

// texture specific functions
Texture Allocate_Texture(std::string const& file_path);

struct Image {

    Image(const char* file_path);
    ~Image();

    uchar* data = nullptr;
    int x = 0;
    int y = 0;
    int channels = 0;
};;

void Allocate_Mesh(Mesh& m);
void Render_Mesh(Mesh const& m, Shader const& s);
void Render_Meshes(Meshes const& m, Shader const& s);

// shader specific
Shader_ID   Create_Shader_Program(const char* vertex_path, const char* fragment_path);
Uniform_Map Map_Uniform_Locations(Shader_ID shader_id, Uniform_List uniforms);

// test code for the triangle example
void Create_Test_Buffer(uint& VBO, uint& VAO);
void Create_Cube_Buffer(uint& VBO, uint& VAO);
void Render_Test(Shader& shader, uint VAO, uint size, float3 pos);

struct Shader {

    // create from files
    Shader(const char* vertex_path, const char* fragment_path, Uniform_List uniform_names);

    void apply() const;

    void send_value(const char* name, bool   value) const;
    void send_value(const char* name, int    value) const;
    void send_value(const char* name, float  value) const;
    void send_value(const char* name, float3 value) const;

    Shader_ID   program_id;
    Uniform_Map uniforms;

    std::string vertex_path;
    std::string fragment_path;
    std::string cached_vertex_code;
    std::string cached_fragment_code;
};
}