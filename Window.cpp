#include <SDL2/SDL.h>
//#include <GL/glew.h>

#include "Window.h"

Window::Window(const char * title, i32 width, i32 height) : w(width), h(height)
{
    SDL_Init(SDL_INIT_EVERYTHING); /// TODO move into render context
    handle = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_OPENGL);
    renderer = SDL_CreateRenderer(handle, -1, 0);
}

Window::~Window()
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(handle);
}

bool Window::should_quit()
{
    SDL_Event e;
    while (SDL_PollEvent(&e) != 0) {
        if (e.type == SDL_QUIT) { return true; }
        if (e.type == SDL_KEYDOWN) {
            if (e.key.keysym.sym == SDLK_ESCAPE) { return true; }
        }
    }

    return false;
}

void Window::swap()
{
    assert(handle != nullptr);
    SDL_GL_SwapWindow(handle);
}

void Window::clear(float value)
{
    clear(value, value, value, value);
}

void Window::clear(float r, float g, float b, float a)
{
    //glClearColor(r, g, b, a);
}
