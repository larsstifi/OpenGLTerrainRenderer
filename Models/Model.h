#pragma once
#include <Shaders\ShaderProgram.h>
#include <Models\MeshRenderer.h>
#include <Models\OBJ_Loader.h>

struct Material {
    std::string name;
    glm::vec3 AmbientColor;
    glm::vec3 DiffuseColor;
    glm::vec3 SpecularColor;
    float SpecularExponent;
    float OpticalDensity;
    float Dissolve;
    int Illumination;
    unsigned int AmbientTexture;
    unsigned int DiffuseTexture;
    unsigned int SpecularTexture;
    unsigned int SpecularHightlightTexture;
    unsigned int AlphaTexture;
    unsigned int BumpTexture;

};

class Model
{
public:
    Model() {};
    Model(const char* path, const char* pathDefaultTexture)
    {
        setDefaultTexture(pathDefaultTexture);
        loadModel(path);
    }
    void setDefaultTexture(const char* filePath);
    void draw(ShaderProgram& shader, glm::mat4 model, bool setMat = true);
    static unsigned int loadTexture(std::string filePath);
private:
    // model data
    std::vector<MeshRenderer> meshes;
    unsigned int defaultTexture;
    std::vector<Material> materials;
    std::vector<unsigned int> matIndices;

    void loadModel(std::string path);
};