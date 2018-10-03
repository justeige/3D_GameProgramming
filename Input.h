#pragma once

#include "Common.h"
#include "Vector.h"
#include "Matrix.h"

// simple input controller

struct GLFWwindow;

struct Input_Controller {

    float44 view = {};
    float44 proj = {};

    u32 h = 0;
    u32 w = 0;

    double last_time = 0.0;
    GLFWwindow* window;

    float3 position = {};

    Input_Controller(GLFWwindow* window);

    void update(float delta_time);
};
