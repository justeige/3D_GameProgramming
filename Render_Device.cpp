#include "Render_Device.h"
#include "Window.h"



Render_Device::Render_Device(Window& window)
{
    context = SDL_GL_CreateContext(window.handle);

    /// glewInit();
    /// open gl init parameters?
}

Render_Device::~Render_Device()
{
    SDL_GL_DeleteContext(context);
    /// whats more to delete?
}
