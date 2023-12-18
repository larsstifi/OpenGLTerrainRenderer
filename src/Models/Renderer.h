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
    ShaderTexture(GLenum paramIndex, unsigned int textureID) : paramIndex(paramIndex), textureID(textureID) {};
    GLenum paramIndex;
    unsigned int textureID;
};
struct ShaderVec3 {
    ShaderVec3(std::string paramName, glm::vec3 val) : paramName(paramName), val(val) {};
    std::string paramName;
    glm::vec3 val;
};
struct ShaderFloat {
    ShaderFloat(std::string paramName, float val) : paramName(paramName), val(val) {};
    std::string paramName;
    float val;
};
struct ShaderBool {
    ShaderBool(std::string paramName, bool val) : paramName(paramName), val(val) {};
    std::string paramName;
    bool val;
};
enum ShaderParamType {
    VEC3,
    FLOAT,
    TEXTURE,
    BOOL
};
struct ShaderParam {
    ShaderParam() {};
    ShaderParam(const ShaderParam& sp) : type(sp.type) {
        switch (type) {
        case VEC3:
            new (&v3) ShaderVec3(sp.v3);
            break;
        case FLOAT:
            new (&f) ShaderFloat(sp.f);
            break;
        case TEXTURE:
            new (&tex) ShaderTexture(sp.tex);
            break;
        case BOOL:
            new (&b) ShaderBool(sp.b);
            break;
        }
    }
    ShaderParam(std::string paramName, glm::vec3 val) : v3(ShaderVec3(paramName, val)), type(VEC3) {};
    ShaderParam(std::string paramName, bool val) : b(ShaderBool(paramName, val)), type(BOOL) {};
    ShaderParam(std::string paramName, float val) : f(ShaderFloat(paramName, val)), type(FLOAT) {};
    ShaderParam(GLenum paramIndex, unsigned int tex) : tex(ShaderTexture(paramIndex, tex)), type(TEXTURE) {};
    ~ShaderParam() {
        if (type == VEC3)
        {
            v3.~ShaderVec3(); 
        }
        else if (type == FLOAT)
        {
            f.~ShaderFloat();
        }
        else if (type == TEXTURE)
        {
            tex.~ShaderTexture();
        }
        else if (type == BOOL) {
            b.~ShaderBool();
        }
    };
    ShaderParamType type;
    union {
        ShaderVec3 v3;
        ShaderFloat f;
        ShaderTexture tex;
        ShaderBool b;
    };
    
};

struct RenderMaterial {
    std::vector<ShaderParam> shaderParams;
    unsigned int spID;
	MAT_TYPE matType = DISABLED;
    void addTexture(GLenum paramIndex, unsigned int textureID) {
        shaderParams.push_back(ShaderParam(paramIndex, textureID));
    }
    void addFloat(std::string paramName, float val) {
        shaderParams.push_back(ShaderParam(paramName, val));
    }
    void addBool(std::string paramName, bool val) {
        shaderParams.push_back(ShaderParam(paramName, val));
    }
    void addVec3(std::string paramName, glm::vec3 val) {
        shaderParams.push_back(ShaderParam(paramName, val));
    }
    void setShaderParams() {
        ShaderProgram::use(spID);
        for (int i = 0; i < shaderParams.size(); i++) {
            if (shaderParams[i].type == FLOAT) ShaderProgram::setFloat(spID, shaderParams[i].f.paramName, shaderParams[i].f.val);
            if (shaderParams[i].type == VEC3) ShaderProgram::setVec3(spID, shaderParams[i].v3.paramName, shaderParams[i].v3.val);
            if (shaderParams[i].type == TEXTURE) {
                glActiveTexture(shaderParams[i].tex.paramIndex);
                glBindTexture(GL_TEXTURE_2D, shaderParams[i].tex.textureID);
            }
        }
    }
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