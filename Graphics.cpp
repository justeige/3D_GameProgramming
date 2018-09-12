#include "Graphics.h"

#include <array>

#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <stb/stb_image.h>

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
GLFWwindow* GL::Global_Init()
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

void GL::Global_Teardown()
{
    glfwTerminate();
}

bool GL::Is_Open(Window* window)
{
    assert(window != nullptr);
    return !glfwWindowShouldClose(window);
}

void GL::Poll_And_Swap(Window* window)
{
    assert(window != nullptr);
    glfwPollEvents();
    glfwSwapBuffers(window);
}

void GL::Close_On_Escape(Window* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

void GL::Clear_Screen()
{
    // wipe the drawing surface clear
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

std::optional<GL::Texture> GL::Allocate_Texture(std::string const& file_path)
{
    // try loading the texture first, no point in allocating any buffer on the gpu otherwise!
    int x, y, channels;
    uchar* image_data = stbi_load(file_path.c_str(), &x, &y, &channels, 0);
    if (image_data == nullptr) {
        return {}; /// TODO maybe return an error also?
    }
    ON_EXIT(stbi_image_free(image_data));

    // image should be loaded, now allocate buffer
    uint texture_id;
    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);   // set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, x, y, 0, GL_RGB, GL_UNSIGNED_BYTE, image_data);
    glGenerateMipmap(GL_TEXTURE_2D);

    GL::Texture t = {};
    t.id = texture_id;
    t.type = GL::Texture::normal;
    t.path = file_path;
    return t;
}

GL::Mesh GL::Allocate_Mesh(Vertices v, Indices i, Textures t)
{
    Mesh m = {}; /*VAO, VBO and EBO will be init from opengl routines!*/
    m.vertices = v;
    m.indices  = i;
    m.textures  = t;

    glGenVertexArrays(1, &m.VAO);
    glGenBuffers(1, &m.VBO);
    glGenBuffers(1, &m.EBO);

    glBindVertexArray(m.VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m.VBO);
    glBufferData(GL_ARRAY_BUFFER, m.vertices.size() * sizeof(Vertex), &m.vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m.indices.size() * sizeof(uint), &m.indices[0], GL_STATIC_DRAW);

    // vertex pos
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    // vertex normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    // vertex texture coords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tex_coord));

    glBindVertexArray(0);
    return m;
}

void GL::Render_Mesh(Mesh const& m, Shader const& s)
{
    s.apply();

    //for (unsigned int i = 0; i < m.textures.size(); i++) {
    //    glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding
    //
    //    /// find uniform in shader
    //
    //    // bind the texture
    //    glBindTexture(GL_TEXTURE_2D, m.textures[i].id);
    //}

    // draw mesh
    glBindVertexArray(m.VAO);
    glDrawElements(GL_TRIANGLES, m.indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // set everything back to defaults
    glActiveTexture(GL_TEXTURE0);
}

Shader_ID GL::Create_Shader_ID(const char* vertex_path, const char* fragment_path)
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

Uniform_Map GL::Map_Uniform_Locations(Shader_ID shader_id, std::initializer_list<std::string> uniform_names)
{
    Uniform_Map mapped_Data;
    for (auto name : uniform_names) {
        auto location = glGetUniformLocation(shader_id, name.c_str());
        assert(location != -1);
        mapped_Data[name] = location;
    }

    return mapped_Data;
}

// opengl data
namespace GL {

void Shader::apply() const
{
    glUseProgram(program);
}

void Shader::send_value(const char* name, bool value) const
{
    int location = uniforms.at(name);
    glUniform1i(location, (int)value);
}

void Shader::send_value(const char* name, int value) const
{
    int location = uniforms.at(name);
    glUniform1i(location, value);
}

void Shader::send_value(const char* name, float value) const
{
    int location = uniforms.at(name);
    glUniform1f(location, value);
}

}




#pragma region "Test-Code"

void GL::Create_Test_Buffer(uint & VBO, uint & VAO)
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

void GL::Create_Cube_Buffer(uint& VBO, uint& VAO)
{
    // buffer layout:
    // [x y z a b][x y z a b][...]
    // |----||---|...
    // 3      2

    const int stride = 5 * sizeof(float);
    float cube_vertices[] = {
        // positions          // texture
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
        0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);

    // map positions to buffer
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);
    // map texture coords to buffer
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

void GL::Render_Test(Shader_ID shader_ref, uint VAO, uint size)
{
    glUseProgram(shader_ref);
    glBindVertexArray(VAO);
    // draw points 0-3 from the currently bound VAO with current in-use shader
    glDrawArrays(GL_TRIANGLES, 0, size);
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
