#pragma once
#include <Shaders\ShaderProgram.h>
#include <Models\MeshRenderer.h>
#include <Models\OBJ_Loader.h>
#include <Terrain/DualContouring.h>
#include <Models/Drawable.h>

struct Material {
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
    unsigned int SpecularTexture= 0;
    unsigned int SpecularHightlightTexture = 0;
    unsigned int AlphaTexture = 0;
    unsigned int BumpTexture = 0;

};

class Model : public Drawable
{
public:
    bool active = false;
    Model() {};
    Model(const char* path, const char* pathDefaultTexture)
    {
        setDefaultTexture(pathDefaultTexture);
        loadModel(path);
        active = true;
    }
    Model(dc::Mesh& mesh, const char* pathDefaultTexture)
    {
        setDefaultTexture(pathDefaultTexture);
        loadModel(mesh);
        active = true;
    }
    void setDefaultTexture(const char* filePath);
    void draw(ShaderProgram& shader, glm::mat4& model, bool setMat = true);
    void drawInstanced(ShaderProgram& shader, glm::mat4& model, unsigned int count, bool setMat = true);
    static unsigned int loadTexture(std::string filePath, int mode = 0);
    void loadModel(dc::Mesh& mesh);
    void clearModels();
private:
    // model data
    std::vector<MeshRenderer> meshes;
    unsigned int defaultTexture;
    std::vector<Material> materials;
    std::vector<unsigned int> matIndices;

    void loadModel(std::string path);
};