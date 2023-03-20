#include <glad/glad.h> 
#include <GLFW/glfw3.h>
#include <imgui/imgui.h>
#include <imgui/imgui_stdlib.h>
#include <imgui/imgui_impl_opengl3.h>
#include <imgui/imgui_impl_glfw.h>
//#include <imgui/imgui_impl_opengl3_loader.h>
#include <Shaders/ShaderProgram.h>
#include <Models/MeshRenderer.h>
#include <Models/Model.h>
#include <Lighting/LightManager.h>
#include <Terrain/NoiseGenerator.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <iostream>
#include <string>
#include <fstream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <FastNoise/FastNoise.h>
bool Setup();
void Render();
void RenderImgui();
void generateTerrain(int seed);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
std::string readFile(const char* filePath);

const char * vertexShaderPath = "Shaders/vs.vert";
const char* fragmentShaderPath = "Shaders/fragment.frag";

unsigned int windowWidth = 2000;
unsigned int windowHeight = 1500;
glm::vec3 bgCol(0.2f, 0.2f, 0.2f);

unsigned int grassTexture;
unsigned int debugTexture;

GLFWwindow* window;
ShaderProgram objectShader;
ShaderProgram foliageShader;
ShaderProgram debugShader;


Model terrain; 
std::vector<Model *> objects;

glm::vec3 positions[] = {
    glm::vec3(0.0f,  0.0f,  0.0f),
};

ShaderProgram lightingShader;
DirectionalLight sun;
SpotLight headlight;
LightManager lm(5, 5, 5);

float fov = 45.f;
glm::vec3 camPos;
glm::vec3 camUp;
glm::vec3 camFront;
bool cursorEnabled = true;

glm::mat4 view;
glm::mat4 projection;

bool firstMouse = true;
float lastX = 400, lastY = 300;
float yaw, pitch;

float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame


int terrainSize = 32;
float sphereSize = 100;
float sphereIntensity = 0.5f;
FastNoise::SmartNode<> fnGenerator;
NoiseGenerator noiseGenerator;

//imgui params
bool gui_RenderDebug = false;
bool gui_RenderFoliage = false;
bool gui_RenderObjects = true;
bool gui_InfoWindow = true;
bool gui_DemoWindow = false;
float gui_CamSpeed = 2.5f;
std::string gui_FoliageTexturePath{ "Models/grass3.png" };

int main()
{

    if (!Setup()) {
        return -1;
    }
    Model m("Models/Tree/tree low.obj", "Models/Solid_white.jpg");
    objects.push_back(&m);
    fnGenerator = FastNoise::NewFromEncodedNodeTree("DQAFAAAAAAAAQAgAAAAAAD8AAAAAAA==");
    generateTerrain(rand());
    objects.push_back(&terrain);
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        //std::cout << "FPS: " << 1 / deltaTime << std::endl;
        // input
        // -----
        processInput(window);

        // render
        // ------
        Render();

        glfwSwapBuffers(window);
        // glfw: poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwPollEvents();
    }
    Render();
    
    
    glfwTerminate();
    return 0;
}

void RenderObjects() {
    objectShader.use();
    objectShader.setMat4("view", view);
    objectShader.setMat4("projection", projection);
    objectShader.setVec3("viewPos", camPos);


    //set headlightlight positions
    headlight.pos = camPos;
    headlight.dir = camFront;
    lm.setLight(objectShader, headlight);


    //render each object
    for (unsigned int i = 0; i < objects.size(); i++)
    {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, positions[0]);
        model = glm::scale(model, glm::vec3(1));
        objects[i]->draw(objectShader, model);
    }
}

void RenderFoliage() {
    //draw foliage
    foliageShader.use();
    foliageShader.setMat4("view", view);
    foliageShader.setMat4("projection", projection);
    foliageShader.setVec3("viewPos", camPos);
    foliageShader.setFloat("dt", (float)glfwGetTime());
    //set headlightlight positions
    lm.setLight(foliageShader, headlight);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, grassTexture);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, grassTexture);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, grassTexture);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, grassTexture);


    //render each object
    for (unsigned int i = 0; i < objects.size(); i++)
    {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, positions[0]);
        model = glm::scale(model, glm::vec3(1));
        objects[i]->draw(foliageShader, model, false);
    }
}

void RenderDebug() {
    debugShader.use();
    debugShader.setMat4("view", view);
    debugShader.setMat4("projection", projection);
    debugShader.setVec3("viewPos", camPos);
    debugShader.setFloat("dt", (float)glfwGetTime());
    //set headlightlight positions
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, debugTexture);


    //render each object
    for (unsigned int i = 0; i < objects.size(); i++)
    {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, positions[0]);
        model = glm::scale(model, glm::vec3(1));
        objects[i]->draw(debugShader, model, false);
    }
}

void RenderImgui() {

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    static float f = 0.0f;
    static int counter = 0;

    ImGui::Begin("Main Config");    
    if (ImGui::CollapsingHeader("Render Settings")) {// Create a window called "Hello, world!" and append into it.
        ImGui::Checkbox("Show Objects", &gui_RenderObjects);
        ImGui::SameLine();
        ImGui::Checkbox("Show Foliage", &gui_RenderFoliage);
        ImGui::SameLine();
        ImGui::Checkbox("Show Debug", &gui_RenderDebug);

        // Edit 1 float using a slider from 0.0f to 1.0f
        ImGui::ColorEdit3("Background color", (float*)&bgCol); // Edit 3 floats representing a color
        if (ImGui::ColorEdit3("Sun color", (float*)&sun.color)) lm.setLight(objectShader, sun);
        ImGui::ColorEdit3("Headlight color", (float*)&headlight.color);
    }

    if (ImGui::CollapsingHeader("Terrain")) {
        ImGui::DragInt("Terrain size", &terrainSize, 1, 3, 64);
        ImGui::DragFloat("Sphere size squared", &sphereSize, 1, 1, 300);
        ImGui::DragFloat("Sphere Intensity", &sphereIntensity, 0.02, 0, 1);
        if (ImGui::Button("Generate Terrain")) {
            generateTerrain(rand());
        }
    }
    if (ImGui::CollapsingHeader("Foliage")) {
       
        ImGui::InputText("Texture file path", &gui_FoliageTexturePath);
        ImGui::SameLine();
        if (ImGui::Button("Set Texture")) {
            grassTexture = Model::loadTexture(gui_FoliageTexturePath, 1);
            std::cout << "Set foliage texure to: " << grassTexture << " at path: " << gui_FoliageTexturePath << std::endl;
        }
    }
    if (ImGui::CollapsingHeader("Player")) {
        ImGui::SliderFloat("FOV", &fov, 1, 90, "%.1f");
        ImGui::SliderFloat("Velocity", &gui_CamSpeed, 1, 90, "%.1f");
    }

    if (ImGui::CollapsingHeader("Windows")) {
        ImGui::Checkbox("Info", &gui_InfoWindow);
        ImGui::Checkbox("Demo", &gui_DemoWindow);
    }
    ImGui::End();

    //additional windows
    if (gui_InfoWindow) {
        ImGui::Begin("Info", &gui_InfoWindow, ImGuiWindowFlags_NoCollapse);
        ImGui::Text("Framerate %.1f FPS (%.1f ms)", 1 / deltaTime, (double)deltaTime * 1000.f);
        ImGui::Text("Player Position: %.1f, %.1f, %.1f", camPos.x, camPos.y, camPos.z);
        ImGui::Text("Player Forward: %.1f, %.1f, %.1f", camFront.x, camFront.y, camFront.z);
        ImGui::End();
    }
    if (gui_DemoWindow) {
        ImGui::ShowDemoWindow(&gui_DemoWindow);
    }

    
    

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Render() {
    //set background color
    glClearColor(bgCol.x, bgCol.y, bgCol.z, 0.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //set view matrices etc.
    view = glm::lookAt(camPos,
        camFront + camPos,
        camUp);

    
    projection = projection = glm::perspective(glm::radians(fov), ((float)windowWidth) / windowHeight, 0.1f, 300.0f);
    if(gui_RenderObjects)RenderObjects();
    if(gui_RenderFoliage)RenderFoliage();
    if(gui_RenderDebug)RenderDebug();
    RenderImgui();

    //update screen
}

bool Setup() {

    //setup OpenGL
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

    windowHeight = mode->height;
    windowWidth = mode->width;
    
    //create Window
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

    //OpenGL render settings
    glViewport(0, 0, windowWidth, windowHeight);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    //glEnable(GL_BLEND);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    

    //define callbacks
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    //setup generel object shader
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

    //setup foliage shader
    foliageShader = ShaderProgram("Shaders/Foliage.vert", "Shaders/Foliage.geom", "Shaders/Foliage.frag");
    foliageShader.use();
    foliageShader.setInt("AmbientTexture", 0);
    foliageShader.setInt("DiffuseTexture", 1);
    foliageShader.setInt("SpecularTexture", 2);
    foliageShader.setInt("SpecularHighlightTexture", 3);
    foliageShader.setInt("AlphaTexture", 4);
    foliageShader.setInt("BumpTexture", 5);
    foliageShader.setBool("blinn", true);
    grassTexture = Model::loadTexture(gui_FoliageTexturePath, 1);
    foliageShader.setVec3("AmbientColor", glm::vec3(0.7, 0.6, 0.3));
    foliageShader.setVec3("DiffuseColor", glm::vec3(0.7, 0.6, 0.3));
    foliageShader.setVec3("SpecularColor", glm::vec3(0.45, 0.4, 0.2));
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, grassTexture);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, grassTexture);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, grassTexture);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, grassTexture);


    //setup debug shader
    debugShader = ShaderProgram("Shaders/Debug.vert", "Shaders/Debug.geom", "Shaders/Debug.frag");
    debugShader.use();
    debugShader.setInt("texture1", 0);
    debugTexture = Model::loadTexture("Models/debug-texture.png", 1);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, debugTexture);
    
    //setup cam position
    camPos = glm::vec3(0.0f, 0.0f, 3.0f);
    camFront = glm::vec3(0.0f, 0.0f, -1.0f);
    camUp = glm::vec3(0.0f, 1.0f, 0.0f);

    //setup lights
    sun.dir = glm::vec3(0.f, -0.3f, 0.5f);
    sun.color = glm::vec3(0.6f, 0.4f, 0.3f);
    lm.addLight(&sun);
    headlight.dir = camFront;
    headlight.pos = camPos;
    headlight.color = glm::vec3(0.6f, 0.6f, 0.6f);
    headlight.constant = 1.f;
    headlight.linear = 0.19f;
    headlight.quadratic = 0.036f;
    headlight.cutOff = glm::cos(glm::radians(12.5f));
    headlight.outerCutOff = glm::cos(glm::radians(17.5f));
    lm.addLight(&headlight);

    //set world lights params in shaders
    lm.setLights(objectShader);
    lm.setLights(foliageShader);


    return true;
}

void generateTerrain(int seed) {
    std::cout << "Seed " << seed << std::endl;
    std::vector<float> noiseOutput(terrainSize * terrainSize * terrainSize);
    //noiseGenerator.generateSphere(noiseOutput.data(), terrainSize, 50, glm::vec3(0), glm::vec3(terrainSize/2));
    noiseGenerator.generateSphericalNoise(fnGenerator, noiseOutput.data(), terrainSize, sphereSize, sphereIntensity/100.f, glm::vec3(0), glm::vec3(terrainSize / 2));
   // noiseGenerator.generateNoise(fnGenerator, noiseOutput.data(), terrainSize);
    //fnGenerator->GenUniformGrid3D(noiseOutput.data(), 0, 0, 0, terrainSize, terrainSize, terrainSize, 0.01f, seed);
    dc::Mesh mesh = dc::generateMesh(noiseOutput, terrainSize);
    if (terrain.active) { terrain.clearModels(); terrain.loadModel(mesh); }
    else terrain = Model(mesh, "Models/container.jpg");
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

    
    float cameraSpeed = gui_CamSpeed * deltaTime; // adjust accordingly
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

    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS) {
        objectShader.setBool("blinn", true);
        foliageShader.setBool("blinn", true);
    }
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
        objectShader.setBool("blinn", false);
        foliageShader.setBool("blinn", false);
    }
    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) {
        sun.dir = glm::rotate(glm::mat4(1.f), deltaTime, glm::vec3(1.f, 0.f, 0.f)) * glm::vec4(sun.dir, 1.f);
        lm.setLight(objectShader, sun);
        lm.setLight(foliageShader, sun);
    }
    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }
    if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
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
