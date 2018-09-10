#pragma once

#include "Common.h"

#include <map>
#include <string>


struct GLFWwindow;
using Window = GLFWwindow;

using Shader_ID = int;
const Shader_ID Bad_Shader = 0;

using Uniform_Map = std::map<std::string, int>;

namespace GL {

// generel opengl
Window* Global_Init();     // if the init fails, the function return nullptr
void    Global_Teardown(); // needs to be called at the end of an successfull program
bool    Is_Open(Window* window);
void    Poll_And_Swap(Window* window); // poll for new events and swap the drawing buffer
void    Close_On_Escape(Window* window);
void    Clear_Screen();

// shader specific
Shader_ID   Create_Shader_ID(const char* vertex_path, const char* fragment_path);
Uniform_Map Map_Uniform_Locations(Shader_ID shader_id, std::initializer_list<std::string> uniform_names);

// test code for the triangle example
void Create_Test_Buffer(uint& VBO, uint& VAO);
void Create_Cube_Buffer(uint& VBO, uint& VAO);
void Render_Test(Shader_ID shader_ref, uint VAO, uint size);


struct Shader {
    Shader(Shader_ID prg, Uniform_Map u) : program(prg), uniforms(u) {}

    void apply() const;

    void send_value(const char* name, bool  value) const;
    void send_value(const char* name, int   value) const;
    void send_value(const char* name, float value) const;

    const Shader_ID   program;
    const Uniform_Map uniforms;
};

}

#include <string>
#include <optional>  // needs c++17 compiler...

/// TODO move into file specific module (if any such file emerges!)
namespace File {

using Text  = std::optional<std::string>;
using Text_Pair = std::pair<Text, Text>;

Text      ReadFull(const char* file_name);
Text_Pair ReadFull(const char* file_name1, const char* file_name2);

}