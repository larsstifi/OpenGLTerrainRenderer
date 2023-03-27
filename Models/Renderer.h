#pragma once
#include <vector>
#include "Drawable.h"
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
public:
    Renderer(std::shared_ptr<ShaderProgram> sp) : shaderProgram(sp) 
    {
        shaderProgram->setInt("AmbientTexture", 0);
        shaderProgram->setInt("DiffuseTexture", 1);
        shaderProgram->setInt("SpecularTexture", 2);
        shaderProgram->setInt("SpecularHighlightTexture", 3);
        shaderProgram->setInt("AlphaTexture", 4);
        shaderProgram->setInt("BumpTexture", 5);
        shaderProgram->setBool("blinn", true);
    }
	void Render();
	void addObject(std::shared_ptr<Drawable> object, uint32_t matIndex);
    uint32_t addMaterial(std::shared_ptr<RenderMaterial> mat);
    std::shared_ptr<ShaderProgram> shaderProgram;

    glm::mat4 view;
    glm::mat4 projection;
    glm::vec3 camPos;

	
};