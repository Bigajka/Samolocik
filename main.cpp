#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "Environment/Mesh.h"
#include "shaders/Shader.h"
#include "Renderer.h"
#include <glm/gtc/matrix_transform.hpp>

constexpr unsigned int WINDOW_WIDTH = 1280;
constexpr unsigned int WINDOW_HEIGHT = 720;

//std::vector<Vertex> vertices =
//{
//    { glm::vec3(-0.5f, -0.5f, 0.0f) },
//    { glm::vec3(0.5f, -0.5f, 0.0f) },
//    { glm::vec3(0.0f,  0.5f, 0.0f) }
//};
//
//std::vector<unsigned int> indices =
//{
//    0, 1, 2
//};
//
//glm::vec3 color = glm::vec3(1.0f, 0.0f, 0.0f);
std::vector<Vertex> vertices;
std::vector<unsigned int> indices;

glm::mat4 model = glm::mat4(1.0f);
glm::mat4 view = glm::mat4(1.0f);
glm::mat4 projection = glm::mat4(1.0f);
float rotationX = 0.0f;
float rotationY = 0.0f;

Renderer renderer;

void FramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);

    if (width > 0 && height > 0)
    {
        projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);
    }
}

void AddFace(
    glm::vec3 normal,
    glm::vec3 color,
    glm::vec3 a,
    glm::vec3 b,
    glm::vec3 c,
    glm::vec3 d
)
{
    unsigned int start = static_cast<unsigned int>(vertices.size());

    vertices.push_back({ a, normal, color });
    vertices.push_back({ b, normal, color });
    vertices.push_back({ c, normal, color });
    vertices.push_back({ d, normal, color });

    indices.insert(indices.end(),
        {
            start, start + 1, start + 2,
            start, start + 2, start + 3
        });
}

void CreateCube()
{
    float s = 0.5f;

    // front
    AddFace(
        glm::vec3(0, 0, 1),
        glm::vec3(0, 0, 1),

        glm::vec3(-s, -s, s),
        glm::vec3(s, -s, s),
        glm::vec3(s, s, s),
        glm::vec3(-s, s, s)
    );

    // back
    AddFace(
        glm::vec3(0, 0, -1),
        glm::vec3(1, 1, 0),

        glm::vec3(-s, -s, -s),
        glm::vec3(-s, s, -s),
        glm::vec3(s, s, -s),
        glm::vec3(s, -s, -s)
    );

    // right
    AddFace(
        glm::vec3(1, 0, 0),
        glm::vec3(1, 0, 0),

        glm::vec3(s, -s, -s),
        glm::vec3(s, s, -s),
        glm::vec3(s, s, s),
        glm::vec3(s, -s, s)
    );

    // left
    AddFace(
        glm::vec3(-1, 0, 0),
        glm::vec3(0, 1, 1),

        glm::vec3(-s, -s, -s),
        glm::vec3(-s, -s, s),
        glm::vec3(-s, s, s),
        glm::vec3(-s, s, -s)
    );

    // up
    AddFace(
        glm::vec3(0, 1, 0),
        glm::vec3(0, 1, 0),

        glm::vec3(-s, s, -s),
        glm::vec3(-s, s, s),
        glm::vec3(s, s, s),
        glm::vec3(s, s, -s)
    );

    // down
    AddFace(
        glm::vec3(0, -1, 0),
        glm::vec3(1, 0, 1),

        glm::vec3(-s, -s, -s),
        glm::vec3(s, -s, -s),
        glm::vec3(s, -s, s),
        glm::vec3(-s, -s, s)
    );
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
    view = glm::lookAt(
        glm::vec3(0.0f, 0.0f, 3.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f)
    );

    projection = glm::perspective(
        glm::radians(45.0f),
        (float)width / (float)height,
        0.1f,
        100.0f
    );

    // OpenGL state
    glEnable(GL_DEPTH_TEST);

    //Mesh* triangle = new Mesh(vertices, indices);
    CreateCube();
    Mesh* cube = new Mesh(vertices, indices);

    Shader* shader = new Shader("Shaders/basic.vert", "Shaders/basic.frag");

    // Render loop
    float lastFrame = (float)glfwGetTime();
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = (float)glfwGetTime();
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        float rotationSpeed = 45.0f;

        // Input
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        {
            rotationX -= rotationSpeed * deltaTime;
        }

        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        {
            rotationX += rotationSpeed * deltaTime;
        }

        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        {
            rotationY -= rotationSpeed * deltaTime;
        }

        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        {
            rotationY += rotationSpeed * deltaTime;
        }

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window, true);
        }

        // Clear framebuffer
        //glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
        glClearColor(0.0, 0.0, 0.0, 0.0);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        model = glm::mat4(1.0f);

        model = glm::rotate(model, glm::radians(rotationX), glm::vec3(1.0f, 0.0f, 0.0f));

        model = glm::rotate(model, glm::radians(rotationY), glm::vec3(0.0f, 1.0f, 0.0f));

        renderer.Draw(
            *cube,
            *shader,
            model,
            view,
            projection
        );

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Have to be deleted before GL context is destroyed
    delete cube;
    delete shader;

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
