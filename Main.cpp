#include "Vector.h"
#include "ECS.h"
#include "Graphics.h"

#include <iostream>

struct TestComponent1 : public Component<TestComponent1> {
    int x, y, z;
} pos_test;

struct TestComponent2 : public Component<TestComponent2> {
    const char* label;
} label_test;

void ecs_core_test();

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glfw3dll.lib")

#include <glad/glad.h>
#include <glfw/glfw3.h>



int main()
{
    ecs_core_test();

    auto window = OpenGL::GlobalInit();
    if (window == nullptr) {
        return -1;
    }
    ON_EXIT(OpenGL::GlobalTeardown());


    // render the cannonical open-gl triangle
    Shader_Ref test_shader = OpenGL::CreateShader("shader/test.vertex", "shader/test.fragment");
    if (test_shader == Bad_Shader) {
        return -1;
    }

    uint VBO, VAO;
    OpenGL::CreateTestBuffer(VBO, VAO);

    while (OpenGL::IsOpen(window)) {
        OpenGL::RenderTest(test_shader, VAO);
        OpenGL::PollAndSwap(window);
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

