#include <glad/glad.h> 
#include <GLFW/glfw3.h>
#include <Shaders/ShaderProgram.h>
#include <Models/MeshRenderer.h>
#include <Models/Model.h>
#include <Lighting/LightManager.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <iostream>
#include <string>
#include <fstream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
bool Setup();
void Render();
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
std::string readFile(const char* filePath);

const char * vertexShaderPath = "Shaders/vs.vert";
const char* fragmentShaderPath = "Shaders/fragment.frag";

unsigned int windowWidth = 2000;
unsigned int windowHeight = 1500;

unsigned int VAO;
unsigned int VBO;
unsigned int EBO;

unsigned int texture1;

GLFWwindow* window;
ShaderProgram objectShader;
Model m;

ShaderProgram lightingShader;
glm::vec3 DirLightDir;
DirectionalLight sun;
SpotLight headlight;
LightManager lm(5, 5, 5);

float fov = 45.f;
glm::vec3 camPos;
glm::vec3 camUp;
glm::vec3 camFront;

bool firstMouse = true;
float lastX = 400, lastY = 300;
float yaw, pitch;

float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

int main()
{
    if (!Setup()) {
        return -1;
    }
    m = Model("Models/Backpack/backpack.obj", "Models/Solid_white.jpg");

    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        // input
        // -----
        processInput(window);

        // render
        // ------
        Render();

        // glfw: poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwPollEvents();
    }
    Render();
    
    
    glfwTerminate();
    return 0;
}

void Render() {
    //stores positions of model
    glm::vec3 positions[] = {
    glm::vec3(0.0f,  0.0f,  0.0f),
    glm::vec3(5.0f,  0.0f,  0.0f),
    glm::vec3(-5.0f,  0.0f,  0.0f)
    };
    //set background color
    glClearColor(0.2f, 0.2f, 0.2f, 0.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //set view matrices etc.
    glm::mat4 view = glm::lookAt(camPos,
        camFront + camPos,
        camUp);

    glm::mat4 projection;
    projection = projection = glm::perspective(glm::radians(fov), ((float)windowWidth) / windowHeight, 0.1f, 100.0f);
    objectShader.use();
    objectShader.setMat4("view", view);
    objectShader.setMat4("projection", projection);
    objectShader.setVec3("viewPos", camPos);

    //set light positions
    
    headlight.pos = camPos;
    headlight.dir = camFront;
    lm.setLight(objectShader, headlight);

    for (unsigned int i = 0; i < 3; i++)
    {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, positions[i]);
        model = glm::scale(model, glm::vec3(0.05));
        float angle = 20.0f * i;
        model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
        m.draw(objectShader, model);
    }

    glfwSwapBuffers(window);
}

bool Setup() {
    glfwInit();


    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    window = glfwCreateWindow(windowWidth, windowHeight, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }
    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return false;
    }

    glViewport(0, 0, windowWidth, windowHeight);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    glClearColor(0.f, 0.f, 0.f, 0.f);
    glClear(GL_COLOR_BUFFER_BIT);
    objectShader = ShaderProgram(vertexShaderPath, fragmentShaderPath);
    objectShader.use();
    objectShader.setInt("AmbientTexture", 0);
    objectShader.setInt("DiffuseTexture", 1);
    objectShader.setInt("SpecularTexture", 2);
    objectShader.setInt("SpecularHighlightTexture", 3);
    objectShader.setInt("AlphaTexture", 4);
    objectShader.setInt("BumpTexture", 5);
    objectShader.setBool("blinn", true);

    camPos = glm::vec3(0.0f, 0.0f, 3.0f);
    camFront = glm::vec3(0.0f, 0.0f, -1.0f);
    camUp = glm::vec3(0.0f, 1.0f, 0.0f);

    DirLightDir = glm::vec3(0.f, -0.3f, 0.5f);
    sun.dir = glm::vec3(0.f, -0.3f, 0.5f);
    lm.addLight(sun);
    headlight.dir = camFront;
    headlight.pos = camPos;
    headlight.constant = 1.f;
    headlight.linear = 0.09f;
    headlight.quadratic = 0.036f;
    headlight.cutOff = glm::cos(glm::radians(12.5f));
    headlight.outerCutOff = glm::cos(glm::radians(17.5f));
    lm.addLight(headlight);
    lm.setLights(objectShader);

    return true;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    windowWidth = width;
    windowHeight = height;
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    
    float cameraSpeed = 2.5f * deltaTime; // adjust accordingly
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT))
        cameraSpeed *= 2.f;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camPos += cameraSpeed * camFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camPos -= cameraSpeed * camFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camPos -= glm::normalize(glm::cross(camFront, camUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camPos += glm::normalize(glm::cross(camFront, camUp)) * cameraSpeed;

    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS)
        objectShader.setBool("blinn", true);
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
        objectShader.setBool("blinn", false);
    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) {
        sun.dir = glm::rotate(glm::mat4(1.f), deltaTime, glm::vec3(1.f, 0.f, 0.f)) * glm::vec4(sun.dir, 1.f);
        lm.setLight(objectShader, sun);
    }
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {

    if (firstMouse) // initially set to true
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates range from bottom to top
    lastX = xpos;
    lastY = ypos;

    const float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 direction;
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    camFront = glm::normalize(direction);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    fov -= (float)yoffset;
    if (fov < 1.0f)
        fov = 1.0f;
    if (fov > 45.0f)
        fov = 45.0f;
}
