//#include "Common.h"
#include "Vector.h"
#include "ECS.h"

struct TestComponent1 : public Component<TestComponent1> {
    int x, y, z;
} pos_test;

struct TestComponent2 : public Component<TestComponent2> {
    const char* label;
} label_test;

void ecs_core_test();

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glfw3dll.lib")

#include <glfw/glfw3.h>


int main()
{
    ecs_core_test();

    if (glfwInit() == GL_FALSE) {
        return -1;
    }
    ON_EXIT(glfwTerminate());

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    int width = 800, height = 600;
    GLFWwindow* window = glfwCreateWindow(width, height, "3D_Game", NULL, NULL);
    if (window == nullptr) {
        return -1;
    }
    glfwMakeContextCurrent(window);

    auto framebuffer_size_callback = [](GLFWwindow* window, int width, int height) {
        glViewport(0, 0, width, height);
    };
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    while (!glfwWindowShouldClose(window)) {
        // input
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, true);
        }

        // render
        glClearColor(0.0f, 0.7f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    return 0;
}


void ecs_core_test()
{
    ECS ecs;
    auto entity1 = ecs.make_from(pos_test, label_test);
    auto current_components = ecs.m_components;
    auto current_entities = ecs.m_entities;
    u32 index = ecs.handle_to_entity_index(entity1);

    assert(current_components.size() == 2);
    assert(current_entities.size() == 1);
    assert(index == 0);
}

