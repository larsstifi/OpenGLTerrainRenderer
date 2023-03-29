#pragma once
#include <vector>
#include "Drawable.h"
#include <Lighting/LightManager.h>
enum MAT_TYPE {
	DISABLED,
	OPAQUE
};
struct RenderMaterial {
	MAT_TYPE matType = DISABLED;
    std::string name;
    glm::vec3 AmbientColor = glm::vec3(1.f);
    glm::vec3 DiffuseColor = glm::vec3(1.f);
    glm::vec3 SpecularColor = glm::vec3(1.f);
    float SpecularExponent = 1.f;
    float OpticalDensity = 1.f;
    float Dissolve = 0.f;
    int Illumination = 0;
    unsigned int AmbientTexture = 0;
    unsigned int DiffuseTexture = 0;
    unsigned int SpecularTexture = 0;
    unsigned int SpecularHightlightTexture = 0;
    unsigned int AlphaTexture = 0;
    unsigned int BumpTexture = 0;};

class Renderer{
private:
    
	std::vector<std::weak_ptr<Drawable>> objects;
	std::vector<uint32_t> materialIndices;
	std::vector<std::shared_ptr<RenderMaterial>> materials;
    LightManager* lightManager;

    bool gui_RenderDebug = false;
    bool gui_RenderObjects = true;
    void RenderObjects();
    void RenderDebug();
public:
    Renderer(std::shared_ptr<ShaderProgram> sp) : shaderProgram(sp) { lightManager = new LightManager(1,1,1); }
    ~Renderer() { delete lightManager; }
	void Render();
    void RenderImgui();
	void addObject(std::shared_ptr<Drawable> object, uint32_t matIndex);
    void addLight(std::shared_ptr<SpotLight> sl);
    void addLight(std::shared_ptr<PointLight> pl);
    void addLight(std::shared_ptr<DirectionalLight> dl);
    uint32_t addMaterial(std::shared_ptr<RenderMaterial> mat);
    std::shared_ptr<ShaderProgram> shaderProgram;
    std::shared_ptr<ShaderProgram> debugShader;
    uint32_t  debugTexture = 0;

    glm::mat4 view;
    glm::mat4 projection;
    glm::vec3 camPos;

	
};