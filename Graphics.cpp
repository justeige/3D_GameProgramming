#include "Graphics.h"

#include <array>

#include <glad/glad.h>
#include <glfw/glfw3.h>

#pragma region "Module-Internal"

// callbacks
auto framebuffer_size_callback = [](GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
};

uint CompileShader(const char* raw_code, GLenum type)
{
    std::array<char, 512> info = {};

    uint shader_ref = glCreateShader(type);
    glShaderSource(shader_ref, 1, &raw_code, NULL);
    glCompileShader(shader_ref);
    int success;
    glGetShaderiv(shader_ref, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader_ref, info.size(), nullptr, info.data());
        /// TODO error handling!!!
    }
    return shader_ref;
}

#pragma endregion

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

Shader_ID OpenGL::CreateShaderID(const char* vertex_path, const char* fragment_path)
{
    auto[vertex_code, fragment_code] = File::ReadFull(vertex_path, fragment_path);
    if (!vertex_code.has_value() || !fragment_code.has_value()) {
        /// TODO error handling!!!
        assert(false);
        return Bad_Shader;
    }

    uint vertex_shader   = CompileShader(vertex_code.value().c_str(),   GL_VERTEX_SHADER);
    uint fragment_shader = CompileShader(fragment_code.value().c_str(), GL_FRAGMENT_SHADER);

    Shader_ID program_id = glCreateProgram();
    glAttachShader(program_id, vertex_shader);
    glAttachShader(program_id, fragment_shader);
    glLinkProgram(program_id);

    std::array<char, 512> info = {};
    int success;
    glGetProgramiv(program_id, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program_id, info.size(), nullptr, info.data());
        /// TODO error handling!!!
        return Bad_Shader;
    }

    // shaders should be linked to the program and are no longer needed
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
    return program_id;
}

// opengl data
namespace OpenGL {

void Shader::apply() const
{
    glUseProgram(program);
}

void Shader::send_value(const char* name, bool value) const
{
    auto location = glGetUniformLocation(program, name);
    assert(location != -1);
    glUniform1i(location, (int)value);
}

void Shader::send_value(const char* name, int value) const
{
    auto location = glGetUniformLocation(program, name);
    assert(location != -1);
    glUniform1i(location, value);
}

void Shader::send_value(const char* name, float value) const
{
    auto location = glGetUniformLocation(program, name);
    assert(location != -1);
    glUniform1f(location, value);
}

}




#pragma region "Test-Code"

void OpenGL::CreateTestBuffer(uint & VBO, uint & VAO)
{
    // set up vertex data (and buffer(s)) and configure vertex attributes for a single triangle
    float vertices[] = {
        // positions         // colors
        0.5f, -0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   // bottom right
        -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,   // bottom left
        0.0f,  0.5f, 0.0f,   0.0f, 0.0f, 1.0f    // top
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), NULL);
    glEnableVertexAttribArray(0);

    // color
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void OpenGL::RenderTest(Shader_ID shader_ref, uint VAO)
{
    // wipe the drawing surface clear
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(shader_ref);
    glBindVertexArray(VAO);
    // draw points 0-3 from the currently bound VAO with current in-use shader
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

#pragma endregion




#include <fstream>
#include <sstream>

File::Text File::ReadFull(const char* file_name)
{
    std::ifstream fs(file_name);
    if (!fs.is_open()) {
        return {};
    }

    std::stringstream ss;
    ss << fs.rdbuf();
    return { ss.str() };
}

File::Text_Pair File::ReadFull(const char* file_name1, const char* file_name2)
{
    return std::make_pair(File::ReadFull(file_name1), File::ReadFull(file_name2));
}
