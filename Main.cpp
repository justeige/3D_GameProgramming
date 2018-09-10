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


int main()
{
    /*ecs_core_test();

    int triangle_test = render_triangle_example();
    assert(triangle_test == 0);*/

    // ----------------------------
    // render a cube example
    // ----------------------------

    // try and create the opengl context/window
    auto window = GL::Global_Init();
    if (window == nullptr) {
        return -1;
    }
    ON_EXIT(GL::Global_Teardown());

    Shader_ID test_shader = GL::Create_Shader_ID("shader/test.vertex", "shader/test.fragment");
    if (test_shader == Bad_Shader) {
        return -1;
    }

    uint VBO, VAO;
    GL::Create_Cube_Buffer(VBO, VAO);

    u64 counter = 0;
    while (GL::Is_Open(window)) {
        counter++;

        GL::Clear_Screen();
        GL::Close_On_Escape(window);
        GL::Render_Test(test_shader, VAO, 36);
        GL::Poll_And_Swap(window);

        if (counter > 200) { break; } // a real timed solution would be better...
    }


    return EXIT_SUCCESS;
}

#include <glfw/glfw3.h>

// testing the ECS in practice... currently the program feels bloated & over-engineerd,
// but I guess thats to expect with only a single static rendered triangle...
int render_triangle_example()
{
    /// TODO refactor into its own files ~ different headers or in one?
    struct Test_Renderable : public Component<Test_Renderable> {

        struct Data {
            Shader_ID shader_id = -1;
            uint VAO = 0;
            uint VBO = 0;
        } data;

        Test_Renderable(Shader_ID id, uint vao, uint vbo) : data{ id, vao, vbo } {}
    };

    struct Render_Test_System : public System {

        Render_Test_System() : System()
        {
            add_component_type(Test_Renderable::Id);
        }

        virtual void update(float delta, Component_Base** components) override
        {
            not_in_use(delta);

            auto test_render = (Test_Renderable*)components[0];
            GL::Render_Test(test_render->data.shader_id, test_render->data.VAO, 3);
        }
    };

    // set the ecs stuff up
    ECS           ecs;
    System_List   systems;
    Render_Test_System test_system;
    bool add_test = systems.add_system(test_system);
    assert(add_test);


    // try and create the opengl context/window
    auto window = GL::Global_Init();
    if (window == nullptr) {
        return -1;
    }
    ON_EXIT(GL::Global_Teardown());

    Shader_ID test_shader = GL::Create_Shader_ID("shader/test.vertex", "shader/test.fragment");
    if (test_shader == Bad_Shader) {
        return -1;
    }

    uint VBO, VAO;
    GL::Create_Test_Buffer(VBO, VAO);

    // create the triangle as an entity
    Test_Renderable rc { test_shader, VAO, VBO };
    auto triangle = ecs.make_entity_from(rc);

    // render the cannonical open-gl triangle for a while, then break out
    u64 counter = 0;
    while (GL::Is_Open(window)) {
        counter++;

        GL::Clear_Screen();

        GL::Close_On_Escape(window);

        ecs.update_systems(systems, 0.f);

        GL::Poll_And_Swap(window);
        if (counter > 200) { break; } // a real timed solution would be better...
    }

    return 0;
}

void ecs_core_test()
{
    ECS ecs;
    auto entity1 = ecs.make_entity_from(pos_test, label_test);
    auto current_components = ecs.m_components;
    auto current_entities = ecs.m_entities;
    u32 index = ecs.handle_to_entity_index(entity1);

    assert(current_components.size() == 2);
    assert(current_entities.size() == 1);
    assert(index == 0);
}

