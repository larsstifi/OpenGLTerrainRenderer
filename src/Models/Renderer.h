#pragma once
#include <vector>
#include "Drawable.h"
#include <Lighting/LightManager.h>
#include <Shaders/ShaderProgram.h>
#include <map>
enum MAT_TYPE {
	DISABLED,
	OPAQUE
};
struct ShaderTexture {
    unsigned int index;
    unsigned int tex;
};
struct ShaderVec3 {
    std::string paramName;
    glm::vec3 val;
};
struct ShaderFloat {
    std::string paramName;
    float val;
};
union ShaderParamData {
    ShaderVec3 sv3;
    ShaderFloat f;
    ShaderTexture tex;
};
enum ShaderParamType {
    VEC3,
    FLOAT,
    TEXTURE
};

struct RenderMaterial {
    std::shared_ptr<std::vector<std::pair<ShaderParamType, ShaderParamData>>> shaderParams;
    unsigned int spID;
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
    unsigned int BumpTexture = 0;
};



struct Texture {
    unsigned int id = 0;
    std::string path;  // we store the path of the texture to compare with other textures
};

class Renderer{
private:
    
	std::vector<std::weak_ptr<Drawable>> objects;
	std::vector<uint32_t> materialIndices;
	std::vector<std::shared_ptr<RenderMaterial>> materials;
    std::vector<std::unique_ptr<ShaderProgram>> shaders;
    LightManager* lightManager;

    bool gui_RenderDebug = false;
    bool gui_RenderObjects = true;
    void RenderObjects();
public:
    Renderer(){ lightManager = new LightManager(1,1,1); }
    ~Renderer() { delete lightManager; }
	void Render();
    void RenderImgui();
	void addObject(std::shared_ptr<Drawable> object, uint32_t matIndex);
    void addLight(std::shared_ptr<SpotLight> sl);
    void addLight(std::shared_ptr<PointLight> pl);
    void addLight(std::shared_ptr<DirectionalLight> dl);
    unsigned int loadTexture(std::string filePath, int mode = 0);
    uint32_t addMaterial(std::shared_ptr<RenderMaterial> mat);
    uint32_t  debugTexture = 0;

    glm::mat4 view;
    glm::mat4 projection;
    glm::vec3 camPos;
    std::vector<Texture> loadedTextures;
};