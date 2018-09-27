#include "Common.h"
#include "Vector.h"
#include "Matrix.h"
#include "Graphics.h"
#include "Model.h"

#include <iostream>


#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glfw3dll.lib")

float44 mat;

int main()
{
    /// test the model loading
    auto obj = Model::LoadOBJ("test.blend");

    // try and create the opengl context/window
    auto window = GL::Global_Init();
    if (!window) {
        return -1;
    }
    on_exit(GL::Global_Teardown());

    GL::Shader test_shader("shader/test.vertex", "shader/test.fragment", {"offset"});

    uint VBO, VAO;
    GL::Create_Cube_Buffer(VBO, VAO);

    u64 counter = 0;
    while (GL::Is_Open(window)) {
        counter++;

        GL::Clear_Screen();
        GL::Close_On_Escape(window);
        GL::Render_Test(test_shader, VAO, 36);
        GL::Poll_And_Swap(window);

        if (counter > 2000) { break; } // a real timed solution would be better...
    }


    return EXIT_SUCCESS;
}