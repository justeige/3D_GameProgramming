#pragma once

#include "Common.h"


struct GLFWwindow;

using Shader_Ref = int;
const Shader_Ref Bad_Shader = 0;

namespace OpenGL {

// generel opengl
GLFWwindow* GlobalInit();     // if the init fails, the function return nullptr
void        GlobalTeardown(); // needs to be called at the end of an successfull program
bool        IsOpen(GLFWwindow* window);
void        PollAndSwap(GLFWwindow* window); // poll for new events and swap the drawing buffer

// shader specific
Shader_Ref  CreateShader(const char* vertex_path, const char* fragment_path);

// test code for the triangle example
void        CreateTestBuffer(uint& VBO, uint& VAO);
void        RenderTest(Shader_Ref shader_ref, uint VAO);


struct Shader {
    Shader(int id) : Id(id) {}

    void send_value(const char* name, bool  value) const;
    void send_value(const char* name, int   value) const;
    void send_value(const char* name, float value) const;

    const int Id;
};

}

#include <vector>
#include <string>
#include <optional>  // needs c++17 compiler...

/// TODO move into file specific module (if any such file emerges!)
namespace File {

using Text  = std::optional<std::string>;
using Text_Pair = std::pair<Text, Text>;

Text      ReadFull(const char* file_name);
Text_Pair ReadFull(const char* file_name1, const char* file_name2);

}