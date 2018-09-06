#pragma once

#include "Common.h"

#include <map>
#include <string>


struct GLFWwindow;

using Shader_ID = int;
const Shader_ID Bad_Shader = 0;

using Uniform_Map = std::map<std::string, int>;

namespace OpenGL {

// generel opengl
GLFWwindow* GlobalInit();     // if the init fails, the function return nullptr
void        GlobalTeardown(); // needs to be called at the end of an successfull program
bool        IsOpen(GLFWwindow* window);
void        PollAndSwap(GLFWwindow* window); // poll for new events and swap the drawing buffer

// shader specific
Shader_ID   CreateShaderID(const char* vertex_path, const char* fragment_path);
Uniform_Map MapUniformLocations(Shader_ID shader_id, std::initializer_list<std::string> uniform_names);

// test code for the triangle example
void        CreateTestBuffer(uint& VBO, uint& VAO);
void        RenderTest(Shader_ID shader_ref, uint VAO);


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