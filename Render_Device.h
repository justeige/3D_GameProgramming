#pragma once

#include <sdl2/SDL.h>

// forward decl.
struct Window;

// this render device will only use opengl
// a later version might use vulcan/directX /// TODO create an interface
struct Render_Device {
    Render_Device(Window& window);
    ~Render_Device();

    SDL_GLContext context;

    // methods
    /// use_shader(index)
    /// set_fbo
    /// set_vao
    /// create_vertex_array
    /// update_vertex_array_buffer

    /// should I do free functions instead?

};