#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <Shaders/ShaderProgram.h>
#include <stack>

struct DirectionalLight {
    int ID;
    glm::vec3 dir;
    glm::vec3 color;
};

struct SpotLight {
    int ID;
    glm::vec3 pos;
    glm::vec3 dir;
    glm::vec3 color;
    float cutOff;
    float outerCutOff;

    float constant;
    float linear;
    float quadratic;
};

struct PointLight {
    int ID;
    glm::vec3 pos;
    glm::vec3 color;

    float constant;
    float linear;
    float quadratic;
};

class LightManager {
	unsigned int maxSpotAmt, maxPointAmt, maxDirectionalAmt;
    std::vector<std::shared_ptr<DirectionalLight>> directionalLights = {};
    std::vector<std::shared_ptr<PointLight>> pointLights = {};
    std::vector<std::shared_ptr<SpotLight>> spotLights = {};
public:
	LightManager(unsigned int maxSpotAmt, unsigned int maxPointAmt, unsigned int maxDirectionalAmt) {
		this->maxSpotAmt = maxSpotAmt;
		this->maxPointAmt = maxPointAmt;
        this->maxDirectionalAmt = maxDirectionalAmt;

        for (int i = 0; i < maxSpotAmt; i++)
        {
            spotIdTracker.push(maxSpotAmt - i - 1);
        }
        for (int i = 0; i < maxPointAmt; i++)
        {
            pointIdTracker.push(maxPointAmt - i - 1 + maxSpotAmt);
        }
        for (int i = 0; i < maxDirectionalAmt; i++)
        {
            directionalIdTracker.push(maxDirectionalAmt - i - 1 + maxSpotAmt + maxPointAmt);
        }
	};
    int addLight(std::shared_ptr<SpotLight> newLight);
    int addLight(std::shared_ptr<PointLight> newLight);
    int addLight(std::shared_ptr<DirectionalLight> newLight);

    void removeLight(int id);

    void setLights(ShaderProgram& shaderProgram);
    void setLight(ShaderProgram& shaderProgram, int id);
    void setLight(ShaderProgram& shaderProgram, SpotLight& light);
    void setLight(ShaderProgram& shaderProgram, PointLight& light);
    void setLight(ShaderProgram& shaderProgram, DirectionalLight& light);
private:
    std::stack<int> directionalIdTracker, pointIdTracker, spotIdTracker;
    
    std::shared_ptr<SpotLight> getSpotLight(int id);
    std::shared_ptr<PointLight> getPointLight(int id);
    std::shared_ptr<DirectionalLight> getDirectionalLight(int id);
};