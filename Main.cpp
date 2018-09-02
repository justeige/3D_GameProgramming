#include "Window.h"
#include "Vector.h"

#pragma comment(lib, "SDL2.lib")
#pragma comment(lib, "glew32.lib")


float3 f{ 1,2,3 }; // test compiling

int main()
{

    Window window("3D Game", 1024, 800);

    for (;;) {

        if (window.should_quit()) { break; }

        window.clear();
        window.swap();
    }

    return 0;
}