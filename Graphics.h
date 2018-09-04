#pragma once

#include "Common.h"

struct GLFWwindow;

using Shader_Ref = int;
const Shader_Ref Bad_Shader = 0;

namespace OpenGL {

GLFWwindow* GlobalInit();     // if the init fails, the function return nullptr
void        GlobalTeardown(); // needs to be called at the end of an successfull program
bool        IsOpen(GLFWwindow* window);
void        PollAndSwap(GLFWwindow* window); // poll for new events and swap the drawing buffer

// test code for the triangle example
Shader_Ref  CreateTestShader();
void        CreateTestBuffer(uint& VBO, uint& VAO);
void        RenderTest(Shader_Ref shader_ref, uint VAO);

}