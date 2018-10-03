#include "Input.h"
#include "Input.h"

#include <glad/glad.h>
#include <glfw/glfw3.h>

// globals
float horizontal_angle  = 3.0;
float vertical_angle    = 0;
float const mouse_speed = 0.005f;
float const PI          = 3.14f;
float const speed       = 3.0f; // 3 units / second

Input_Controller::Input_Controller(GLFWwindow* w)
    : last_time{ glfwGetTime() }, w{800}, h{600}, window{w}
{
    assert(w != nullptr);
}

void Input_Controller::update(float delta_time)
{
    double current_time = glfwGetTime();
    float delta = float(current_time - last_time);
    on_exit(last_time = current_time); // save for next frame

    // fetch mouse pos
    double xpos = 0.0, ypos = 0.0;
    glfwGetCursorPos(window, &xpos, &ypos);

    /// for testing
    xpos = xpos / w;
    ypos = 1 - (ypos / h);
    position.x = xpos;
    position.y = ypos;
    ///

    // reset mouse pos
    /*glfwSetCursorPos(window, w / 2, h / 2);

    horizontal_angle += mouse_speed * float(1024 / 2 - xpos);
    vertical_angle   += mouse_speed * float(768  / 2 - ypos);

    // direction : spherical coordinates -> cartesian
    float3 direction {
        std::cos(vertical_angle) * std::sin(horizontal_angle),
        std::sin(vertical_angle),
        std::cos(vertical_angle) * std::cos(horizontal_angle)
    };

    float3 right {
        std::sin(horizontal_angle -  PI / 2.0f),
        0,
        std::cos(horizontal_angle - PI / 2.0f)
    };

    float3 up = cross_product(right, direction);

    // forward
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        position += direction * delta_time * speed;
    }
    // backward
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        position -= direction * delta_time * speed;
    }
    // right
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        position += right * delta_time * speed;
    }
    // left
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        position -= right * delta_time * speed;
    }*/
}