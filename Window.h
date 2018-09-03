#pragma once

#include "Common.h"

// forward decl.
struct SDL_Window;
struct SDL_Renderer;

struct Window {

    SDL_Window* handle = nullptr;
    SDL_Renderer* renderer = nullptr; /// TODO move into renderer context
    i32 w = 800, h = 600;


    Window(const char* title, i32 width, i32 height);
    ~Window();

    no_copy_and_assign(Window);
    no_move_and_assign(Window);

    bool should_quit();
    void swap();

    void clear(float value = 0);
    void clear(float r, float g, float b, float a);

    std::pair<i32, i32> size() const { return { w, h }; }
};
