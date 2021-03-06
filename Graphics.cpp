#include "Graphics.h"
#include "File.h"
#include "Profiling.h"

#include <array>
#include <iostream>
#include <unordered_map>

#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <stb/stb_image.h>


// ---------------------------------------------
// module internal code - forward decl.
// ---------------------------------------------
void Report_Error(uint id, std::string const& text);
uint Compile_Shader(const char* raw_code, GLenum type);
Shader_ID Link_Shader(uint vertex_shader, uint fragment_shader);


// ---------------------------------------------
// basic functions
// ---------------------------------------------
Shader_ID GL::Create_Shader_Program(const char* vertex_path, const char* fragment_path)
{
    measure_time();

    auto[vertex_code, fragment_code] = File::ReadFull(vertex_path, fragment_path);
    uint vertex_shader = Compile_Shader(vertex_code.value().c_str(), GL_VERTEX_SHADER);
    uint fragment_shader = Compile_Shader(fragment_code.value().c_str(), GL_FRAGMENT_SHADER);
    Shader_ID program_id = Link_Shader(vertex_shader, fragment_shader);

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    return program_id;
}

Uniform_Map GL::Map_Uniform_Locations(Shader_ID shader_id, std::initializer_list<std::string> uniform_names)
{
    measure_time();

    Uniform_Map mapped_Data;
    for (auto name : uniform_names) {
        auto location = glGetUniformLocation(shader_id, name.c_str());
        assert(location != -1);
        mapped_Data[name] = location;
    }

    return mapped_Data;
}

Window* GL::Global_Init()
{
    measure_time();

    if (glfwInit() == GL_FALSE) {
        std::cerr << "Failed to init GLFW\n";
        assert(false);
        return nullptr;
    }

    // set version/properties
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // create the window and make it the context
    int width = 800, height = 600;
    Window* window = glfwCreateWindow(width, height, "3D_Game", NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create a window\n";
        assert(false);
        return nullptr;
    }
    glfwMakeContextCurrent(window);

    // ----------------------------------------------------------------
    // IMPORTANT! all other opengl code depends on this call!!!
    // loads all opengl function pointer - needs an existing window!
    // ----------------------------------------------------------------

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD\n";
        assert(false);
        return nullptr;
    }

    // tell GL to only draw onto a pixel if the shape is closer to the viewer
    glEnable(GL_DEPTH_TEST); // enable depth-testing
    glDepthFunc(GL_LESS);    // depth-testing interprets a smaller value as "closer"
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

Texture GL::Allocate_Texture(std::string const& file_path)
{
    // try loading the texture first, no point in allocating any buffer on the gpu otherwise!
    Image image(file_path.c_str());

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

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image.x, image.y, 0, GL_RGB, GL_UNSIGNED_BYTE, image.data);
    glGenerateMipmap(GL_TEXTURE_2D);

    Texture t = {};
    t.id = texture_id;
    t.type = Texture::normal;
    t.path = file_path;
    return t;
}

void GL::Allocate_Mesh(Mesh& mesh)
{
    glGenVertexArrays(1, &mesh.VAO);
    glGenBuffers(1, &mesh.VBO);
    glGenBuffers(1, &mesh.EBO);

    glBindVertexArray(mesh.VAO);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO);
    glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(Vertex), &mesh.vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(uint), &mesh.indices[0], GL_STATIC_DRAW);

    // vertex pos
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    // vertex normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    // vertex texture coords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tex_coord));
    // vertex tangent
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent));
    // vertex bitangent
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, bitangent));


    glBindVertexArray(0);
}

void Render_Mesh_internal(Mesh const& mesh, GL::Shader const& shader)
{
    uint diffuse_count = 1;
    uint specular_count = 1;

    for (uint i = 0; i < mesh.textures.size(); i++) {

        // activate proper texture unit before binding
        glActiveTexture(GL_TEXTURE0 + i);

        // read the texture number
        std::string number{};
        Texture::Type type = mesh.textures[i].type;
        if (type == Texture::diffuse) {
            number = std::to_string(diffuse_count);
            diffuse_count++;
        }
        else if (type == Texture::specular) {
            number = std::to_string(specular_count);
            specular_count++;
        }

        // send information to the shader
        const std::string tex_name = "material." + std::to_string(type) + number;
        shader.send_value(tex_name.c_str(), (float)i);

        // bind the texture
        glBindTexture(GL_TEXTURE_2D, mesh.textures[i].id);
    }

    // draw mesh
    glBindVertexArray(mesh.VAO);
    glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // set everything back to defaults
    glActiveTexture(GL_TEXTURE0);
}

void GL::Render_Mesh(Mesh const& mesh, Shader const& shader)
{
    shader.apply(); // activate!
    Render_Mesh_internal(mesh, shader);
}

void GL::Render_Meshes(Meshes const& meshes, Shader const& shader)
{
    shader.apply(); // activate only once!
    for_size(n, meshes) {
        Render_Mesh_internal(meshes[n], shader);
    }
}



// ---------------------------------------------
// shader code
// ---------------------------------------------
#pragma region "Shader"
GL::Shader::Shader(const char* vertex_path, const char* fragment_path, Uniform_List uniform_names) : vertex_path { vertex_path }, fragment_path { fragment_path }
{
    program_id = GL::Create_Shader_Program(vertex_path, fragment_path);
    uniforms = GL::Map_Uniform_Locations(program_id, uniform_names);
}

void GL::Shader::apply() const
{
    glUseProgram(program_id);
}

void GL::Shader::send_value(const char* name, bool value) const
{
    int location = uniforms.at(name);
    glUniform1i(location, (int)value);
}

void GL::Shader::send_value(const char* name, int value) const
{
    int location = uniforms.at(name);
    glUniform1i(location, value);
}

void GL::Shader::send_value(const char* name, float value) const
{
    //int location = uniforms.at(name);
    //glUniform1f(location, value);
    auto loc = glGetUniformLocation(program_id, name);
    glUniform1f(loc, value);
}

void GL::Shader::send_value(const char* name, float3 value) const
{
    int location = uniforms.at(name);
    glUniform3fv(location, 1, value.data);
}

#pragma endregion

// ---------------------------------------------
// image code
// ---------------------------------------------
#pragma region "Image"
GL::Image::Image(const char* file_name)
{
    // check image cache first
    static std::unordered_map<const char*, uchar*> image_cache {};
    if (image_cache.count(file_name) > 0) {
        data = image_cache.at(file_name);
        return;
    }

    // first time this image is loaded
    data = stbi_load(file_name, &x, &y, &channels, 0);
    if (data != nullptr) { return; }

    // something went wrong - file name changed perhaps?
    std::cerr << "Failed to load image " << file_name << '\n';
    assert(false);
}

GL::Image::~Image()
{
    if (data) {
        stbi_image_free(data);
    }
}
#pragma endregion

// ---------------------------------------------
// test code
// ---------------------------------------------
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
    measure_time();

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

void GL::Render_Test(Shader& shader, uint VAO, uint size, float3 pos)
{
    shader.send_value("offset", pos);
    shader.apply();
    glBindVertexArray(VAO);
    // draw points 0-3 from the currently bound VAO with current in-use shader
    glDrawArrays(GL_TRIANGLES, 0, size);
}

#pragma endregion

// ---------------------------------------------
// module internal code
// ---------------------------------------------
#pragma region "Module internal"

void Report_Error(uint id, std::string const& text)
{
    std::array<char, 512> info = {};
    glGetShaderInfoLog(id, info.size(), nullptr, info.data());
    std::cerr << text << '\n';
    std::cerr << info.data() << '\n';
    assert(false);
}

uint Compile_Shader(const char* raw_code, GLenum type)
{
    uint shader_ref = glCreateShader(type);
    glShaderSource(shader_ref, 1, &raw_code, NULL);
    glCompileShader(shader_ref);
    int success;
    glGetShaderiv(shader_ref, GL_COMPILE_STATUS, &success);
    if (!success) {
        Report_Error(shader_ref, "Failed to compile shader with:");
    }
    return shader_ref;
}

Shader_ID Link_Shader(uint vertex_shader, uint fragment_shader)
{
    Shader_ID program_id = glCreateProgram();
    glAttachShader(program_id, vertex_shader);
    glAttachShader(program_id, fragment_shader);
    glLinkProgram(program_id);

    int success;
    glGetProgramiv(program_id, GL_LINK_STATUS, &success);
    if (!success) {
        Report_Error(program_id, "Failed to link shader program with:");
        return Bad_Shader;
    }

    return program_id;
}

#pragma endregion