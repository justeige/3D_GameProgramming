#include "Window.h"
#include "Vector.h"
#include "ECS.h"

#pragma comment(lib, "SDL2.lib")


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