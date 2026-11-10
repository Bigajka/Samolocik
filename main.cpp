#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "Environment/Mesh.h"
#include "shaders/Shader.h"
#include "Renderer.h"

constexpr unsigned int WINDOW_WIDTH = 1280;
constexpr unsigned int WINDOW_HEIGHT = 720;

std::vector<Vertex> vertices =
{
    { glm::vec3(-0.5f, -0.5f, 0.0f) },
    { glm::vec3(0.5f, -0.5f, 0.0f) },
    { glm::vec3(0.0f,  0.5f, 0.0f) }
};

std::vector<unsigned int> indices =
{
    0, 1, 2
};

glm::vec3 color = glm::vec3(1.0f, 0.0f, 0.0f);

glm::mat4 model = glm::mat4(1.0f);
glm::mat4 view = glm::mat4(1.0f);
glm::mat4 projection = glm::mat4(1.0f);

Renderer renderer;

void FramebufferSizeCallback( GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

int main()
{
    // GLFW initialization
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW\n";
        return -1;
    }

    // OpenGL version and profile
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    glfwWindowHint(
        GLFW_OPENGL_PROFILE,
        GLFW_OPENGL_CORE_PROFILE
    );

    GLFWwindow* window = glfwCreateWindow(
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        "OpenGL Renderer",
        nullptr,
        nullptr
    );

    if (!window)
    {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    // V-Sync
    glfwSwapInterval(1);

    // GLAD initialization
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Failed to initialize GLAD\n";
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);

    // Initial viewport
    int width, height;

    glfwGetFramebufferSize(
        window,
        &width,
        &height
    );

    glViewport(0, 0, width, height);

    // OpenGL state
    glEnable(GL_DEPTH_TEST);

    Mesh* triangle = new Mesh(vertices, indices);

    Shader* shader = new Shader("Shaders/basic.vert", "Shaders/basic.frag");

    // Render loop
    while (!glfwWindowShouldClose(window))
    {
        // Input
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window, true);
        }

        // Clear framebuffer
        glClearColor(0.1f, 0.1f, 0.15f, 1.0f);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        renderer.Draw(
            *triangle,
            *shader,
            model,
            view,
            projection,
            color
        );

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Have to be deleted before GL context is destroyed
    delete triangle;
    delete shader;

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
