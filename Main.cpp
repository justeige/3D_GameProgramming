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
int  render_triangle_example();

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glfw3dll.lib")

#include <glad/glad.h>
#include <glfw/glfw3.h>


/// TODO refactor into its own files ~ different headers or in one?
struct Renderable : public Component<Renderable> {
    /// TODO fill
};

struct Render_System : public System {

    Render_System() : System()
    {
        add_component_type(Renderable::Id);
    }

    virtual void update(float delta, Component_Base** components) override
    {
        /// TODO fill
        std::cout << "update render system\n";
    }
};


int main()
{
    ecs_core_test();

    int triangle_test = render_triangle_example();
    assert(triangle_test == 0);

    return EXIT_SUCCESS;
}

int render_triangle_example()
{
    auto window = OpenGL::GlobalInit();
    if (window == nullptr) {
        return -1;
    }
    ON_EXIT(OpenGL::GlobalTeardown());


    // render the cannonical open-gl triangle
    Shader_ID test_shader = OpenGL::CreateShaderID("shader/test.vertex", "shader/test.fragment");
    if (test_shader == Bad_Shader) {
        return -1;
    }

    uint VBO, VAO;
    OpenGL::CreateTestBuffer(VBO, VAO);

    u64 counter = 0;

    while (OpenGL::IsOpen(window)) {
        counter++;
        OpenGL::RenderTest(test_shader, VAO);
        OpenGL::PollAndSwap(window);

        if (counter > 200) { break; } // a real timed solution would be better...
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

