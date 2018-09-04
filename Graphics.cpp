#include "Graphics.h"


#include <glad/glad.h>
#include <glfw/glfw3.h>

// callbacks
auto framebuffer_size_callback = [](GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
};

/// TODO do actual error reporting
GLFWwindow* OpenGL::GlobalInit()
{
    if (glfwInit() == GL_FALSE) {
        return nullptr;
    }

    // set version/properties
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // create the window and make it the context
    int width = 800, height = 600;
    GLFWwindow* window = glfwCreateWindow(width, height, "3D_Game", NULL, NULL);
    if (window == nullptr) {
        return nullptr;
    }
    glfwMakeContextCurrent(window);

    // ----------------------------------------------------------------
    // IMPORTANT! all other opengl code depends on this call!!!
    // loads all opengl function pointer - needs an existing window!
    // ----------------------------------------------------------------

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        // "Failed to initialize GLAD"
        return nullptr;
    }

    // tell GL to only draw onto a pixel if the shape is closer to the viewer
    glEnable(GL_DEPTH_TEST); // enable depth-testing
    glDepthFunc(GL_LESS);    // depth-testing interprets a smaller value as "closer"

    // gets called if the window is resized
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);



    return window;
}

void OpenGL::GlobalTeardown()
{
    glfwTerminate();
}

bool OpenGL::IsOpen(GLFWwindow* window)
{
    assert(window != nullptr);
    return !glfwWindowShouldClose(window);
}

void OpenGL::PollAndSwap(GLFWwindow* window)
{
    assert(window != nullptr);
    glfwPollEvents();
    glfwSwapBuffers(window);
}

Shader_Ref OpenGL::CreateTestShader()
{
    int success;
    char info_log[512];

    auto vertex_shader_code = "#version 330 core\n"
                              "layout (location = 0) in vec3 aPos;\n"
                              "void main()\n"
                              "{\n"
                              "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
                              "}\0";
    auto fragment_shader_code = "#version 330 core\n"
                                "out vec4 FragColor;\n"
                                "void main()\n"
                                "{\n"
                                "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
                                "}\n\0";

    int vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_code, NULL);
    glCompileShader(vertex_shader);
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertex_shader, 512, NULL, info_log);
        /// TODO error logging for info_log
        return Bad_Shader;
    }
    ON_EXIT(glDeleteShader(vertex_shader));


    int fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_code, NULL);
    glCompileShader(fragment_shader);
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragment_shader, 512, NULL, info_log);
        /// TODO error logging for info_log
        return Bad_Shader;
    }
    ON_EXIT(glDeleteShader(fragment_shader));

    int shader_ref = glCreateProgram();
    glAttachShader(shader_ref, vertex_shader);
    glAttachShader(shader_ref, fragment_shader);
    glLinkProgram(shader_ref);
    glGetProgramiv(shader_ref, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shader_ref, 512, NULL, info_log);
        /// TODO error logging for info_log
        return Bad_Shader;
    }

    return shader_ref;
}

void OpenGL::CreateTestBuffer(uint & VBO, uint & VAO)
{
    // set up vertex data (and buffer(s)) and configure vertex attributes for a single triangle
    float vertices[] = {
        0.0f,  0.5f,  0.0f,
        0.5f, -0.5f,  0.0f,
        -0.5f, -0.5f,  0.0f
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void OpenGL::RenderTest(Shader_Ref shader_ref, uint VAO)
{
    // wipe the drawing surface clear
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(shader_ref);
    glBindVertexArray(VAO);
    // draw points 0-3 from the currently bound VAO with current in-use shader
    glDrawArrays(GL_TRIANGLES, 0, 3);
}


