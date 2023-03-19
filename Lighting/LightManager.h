#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <Shaders/ShaderProgram.h>
#include <stack>

struct DirectionalLight {
    int ID;
    glm::vec3 dir;
};

struct SpotLight {
    int ID;
    glm::vec3 pos;
    glm::vec3 dir;
    float cutOff;
    float outerCutOff;

    float constant;
    float linear;
    float quadratic;
};

struct PointLight {
    int ID;
    glm::vec3 pos;

    float constant;
    float linear;
    float quadratic;
};

class LightManager {
	unsigned int maxSpotAmt, maxPointAmt, maxDirectionalAmt;
    std::vector<DirectionalLight> directionalLights;
    std::vector<PointLight> pointLights;
    std::vector<SpotLight> spotLights;
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
    int addLight(SpotLight& newLight);
    int addLight(PointLight& newLight);
    int addLight(DirectionalLight& newLight);

    void removeLight(int id);

    void setLights(ShaderProgram& shaderProgram);
    void setLight(ShaderProgram& shaderProgram, int id);
    void setLight(ShaderProgram& shaderProgram, SpotLight& light);
    void setLight(ShaderProgram& shaderProgram, PointLight& light);
    void setLight(ShaderProgram& shaderProgram, DirectionalLight& light);
private:
    std::stack<int> directionalIdTracker, pointIdTracker, spotIdTracker;
    
    SpotLight& getSpotLight(int id);
    PointLight& getPointLight(int id);
    DirectionalLight& getDirectionalLight(int id);
};