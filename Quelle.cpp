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
#include <Terrain/TerrainChunk.h>
#include <Terrain/Octree.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <iostream>
#include <string>
#include <fstream>
#include <memory>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <FastNoise/FastNoise.h>
#include <Models/Renderer.h>
bool Setup();
void createObjects();
void Render();
void RenderImgui();
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

GLFWwindow* window;

Renderer* renderer;
std::shared_ptr<ShaderProgram> sp;
std::shared_ptr<ShaderProgram> dsp;

 
std::shared_ptr<Octree> octree;
std::shared_ptr<Model> refModel;
std::vector<std::shared_ptr<Drawable>> objects;

glm::vec3 positions[] = {
    glm::vec3(0.0f,  0.0f,  0.0f),
};

std::shared_ptr<DirectionalLight> sun;
std::shared_ptr <SpotLight> headlight;

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


FastNoise::SmartNode<> fnGenerator;
NoiseGenerator noiseGenerator;

//imgui params
bool gui_InfoWindow = true;
bool gui_DemoWindow = false;
float gui_CamSpeed = 25.f;
bool gui_TerrainUpdatePlayerPos = true;

int main()
{
    //TODO clean up Model, moveImgui, add foliage and debug to renderer
    if (!Setup()) {
        return -1;
    }

    createObjects();

    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        if(gui_TerrainUpdatePlayerPos) octree->playerPos = camPos;


        // input
        // -----
        processInput(window);

        // update objects
        // --------------
        octree->Update();

        // render
        // ------
        Render();
        RenderImgui();

        glfwSwapBuffers(window);
        // glfw: poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwPollEvents();
    }
    Render();
    
    
    glfwTerminate();
    return 0;
}

void createObjects() {
    std::shared_ptr<RenderMaterial> mat = std::make_shared<RenderMaterial>();
    mat->matType = OPAQUE;
    mat->DiffuseTexture = Model::loadTexture("Models/Solid_white.jpg");
    mat->AmbientTexture = Model::loadTexture("Models/Solid_white.jpg");
    mat->SpecularTexture = Model::loadTexture("Models/Solid_white.jpg");
    mat->AlphaTexture = Model::loadTexture("Models/Solid_white.jpg");

    uint32_t matIndex = renderer->addMaterial(mat);

    octree = std::shared_ptr<Octree>(new Octree(5));
    renderer->addObject(octree, matIndex);
    objects.push_back(octree);

    refModel = std::make_shared<Model>("Models/Tree/tree low.obj", "Models/Solid_white.jpg");
    std::shared_ptr<RenderMaterial> mat2 = std::make_shared<RenderMaterial>();
    matIndex = renderer->addMaterial(mat2);
    renderer->addObject(refModel, matIndex);
    objects.push_back(refModel);
}

void RenderImgui() {

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    static float f = 0.0f;
    static int counter = 0;

    ImGui::Begin("Main Config");    
    if (ImGui::CollapsingHeader("Render Settings")) {// Create a window called "Hello, world!" and append into it.
        renderer->RenderImgui();

        // Edit 1 float using a slider from 0.0f to 1.0f
        ImGui::ColorEdit3("Background color", (float*)&bgCol); // Edit 3 floats representing a color
        ImGui::ColorEdit3("Sun color", (float*)&sun->color);
        ImGui::ColorEdit3("Headlight color", (float*)&headlight->color);
    }

    if (ImGui::CollapsingHeader("Terrain")) {
        ImGui::SliderFloat("LOD Falloff", &octree->LOD_Falloff, 0, 2, "%.2f");
        ImGui::Checkbox("Update Player Position", &gui_TerrainUpdatePlayerPos);
    }
    if (ImGui::CollapsingHeader("Player")) {
        ImGui::SliderFloat("FOV", &fov, 1, 90, "%.1f");
        ImGui::SliderFloat("Velocity", &gui_CamSpeed, 1, 90, "%.1f");
    }

    if (ImGui::CollapsingHeader("Objects")) {
        for (std::shared_ptr<Drawable> o : objects)
        {
            ImGui::Separator();
            o->drawImgui();
        }
        
    }

    if (ImGui::CollapsingHeader("Windows")) {
        ImGui::Checkbox("Info", &gui_InfoWindow);
        ImGui::Checkbox("Demo", &gui_DemoWindow);
    }
    ImGui::End();

    ImGui::Begin("Console Out");
    if (ImGui::Button("Hello")) octree->gui_Cout.append("Hello\n");
    ImGui::TextUnformatted(octree->gui_Cout.begin());
    ImGui::SetScrollHereY(1.0f);
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

    projection = glm::perspective(glm::radians(fov), ((float)windowWidth) / windowHeight, 0.1f, 1000.f);

    renderer->view = view;
    renderer->camPos = camPos;
    renderer->projection = projection;

    headlight->pos = camPos;
    headlight->dir = camFront;
    renderer->Render();
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


    sp = std::make_shared<ShaderProgram>(vertexShaderPath, fragmentShaderPath);
    sp->setInt("AmbientTexture", 0);
    sp->setInt("DiffuseTexture", 1);
    sp->setInt("SpecularTexture", 2);
    sp->setInt("SpecularHighlightTexture", 3);
    sp->setInt("AlphaTexture", 4);
    sp->setInt("BumpTexture", 5);
    sp->setBool("blinn", true);
    dsp = std::make_shared<ShaderProgram>("Shaders/Debug.vert", "Shaders/Debug.geom", "Shaders/Debug.frag");
    dsp->use();
    dsp->setInt("texture1", 0);
    
    //setup cam position
    camPos = glm::vec3(0.0f, 0.0f, 3.0f);
    camFront = glm::vec3(0.0f, 0.0f, -1.0f);
    camUp = glm::vec3(0.0f, 1.0f, 0.0f);

    //setup renderers
    renderer = new Renderer(sp);
    renderer->debugShader = dsp;
    renderer->debugTexture = Model::loadTexture("Models/debug-texture.png", 1);

    //setup lights
    sun = std::shared_ptr<DirectionalLight>(new DirectionalLight());
    sun->dir = glm::vec3(0.f, -0.3f, 0.5f);
    sun->color = glm::vec3(0.6f, 0.4f, 0.3f);
    renderer->addLight(sun);
    headlight = std::shared_ptr<SpotLight>(new SpotLight());
    headlight->dir = camFront;
    headlight->pos = camPos;
    headlight->color = glm::vec3(0.6f, 0.6f, 0.6f);
    headlight->constant = 1.f;
    headlight->linear = 0.19f;
    headlight->quadratic = 0.036f;
    headlight->cutOff = glm::cos(glm::radians(12.5f));
    headlight->outerCutOff = glm::cos(glm::radians(17.5f));
    renderer->addLight(headlight);


    //set world lights params in shaders


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
        sp->setBool("blinn", true);
    }
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
        sp->setBool("blinn", false);
    }
    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) {
        sun->dir = glm::rotate(glm::mat4(1.f), deltaTime, glm::vec3(1.f, 0.f, 0.f)) * glm::vec4(sun->dir, 1.f);
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
