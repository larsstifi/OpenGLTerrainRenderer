#include "LightManager.h"

int LightManager::addLight(SpotLight* newLight)
{
    if (spotLights.size() >= maxSpotAmt)
        return -1;

    newLight->ID = spotIdTracker.top();
    spotIdTracker.pop();
    spotLights.push_back(newLight);
    return newLight->ID;
}
int LightManager::addLight(PointLight* newLight)
{
    
	if (pointLights.size() >= maxPointAmt)
		return -1;

    newLight->ID = pointIdTracker.top();
    pointIdTracker.pop();
	pointLights.push_back(newLight);
    return newLight->ID;
}
int LightManager::addLight(DirectionalLight * newLight)
{
    if (directionalLights.size() >= maxDirectionalAmt)
        return -1;

    newLight->ID = directionalIdTracker.top();
    directionalIdTracker.pop();
    directionalLights.push_back(newLight);
    return newLight->ID;
}

void LightManager::removeLight(int id)
{
    if (id < maxSpotAmt) {
        for (int i = 0; i < spotLights.size(); i++)
        {
            if (spotLights[i]->ID == id) {
                spotLights.erase(spotLights.begin() + i);
                break;
            }
            spotIdTracker.push(id);
        }
    }
    else if (id < maxSpotAmt + maxPointAmt) {
        for (int i = 0; i < pointLights.size(); i++)
        {
            if (pointLights[i]->ID == id) {
                pointLights.erase(pointLights.begin() + i);
                break;
            }
            pointIdTracker.push(id);
        }
    }
    else {
        for (int i = 0; i < directionalLights.size(); i++)
        {
            if (directionalLights[i]->ID == id) {
                directionalLights.erase(directionalLights.begin() + i);
                break;
            }
            directionalIdTracker.push(id);
        }
    }
}

void LightManager::setLights(ShaderProgram& shaderProgram)
{
    shaderProgram.use();
    shaderProgram.setInt("SpotLightAmt", spotLights.size());
    shaderProgram.setInt("PointLightAmt", pointLights.size());
    shaderProgram.setInt("DirectionalLightAmt", directionalLights.size());
    for (size_t i = 0; i < directionalLights.size(); i++)
    {
        setLight(shaderProgram, *(directionalLights[i]));
    }
    for (size_t i = 0; i < spotLights.size(); i++)
    {
        setLight(shaderProgram, *(spotLights[i]));
    }
    for (size_t i = 0; i < pointLights.size(); i++)
    {
        setLight(shaderProgram, *(pointLights[i]));
    }
}

void LightManager::setLight(ShaderProgram& shaderProgram, int id)
{
    shaderProgram.setInt("SpotLightAmt", maxSpotAmt);
    shaderProgram.setInt("PointLightAmt", maxPointAmt);
    shaderProgram.setInt("DirectionalLightAmt", maxDirectionalAmt);
    if (id < maxSpotAmt) {
        setLight(shaderProgram, getSpotLight(id));
        return;
    }
    if (id < maxPointAmt) {
        setLight(shaderProgram, getPointLight(id));
        return;
    }
    setLight(shaderProgram, getDirectionalLight(id));
}

void LightManager::setLight(ShaderProgram& shaderProgram, SpotLight& light)
{
    std::string loc = "SpotLights[";
    loc += std::to_string(light.ID);
    loc += "].";
    shaderProgram.setVec3(loc + "pos", light.pos);
    shaderProgram.setVec3(loc + "dir", light.dir);
    shaderProgram.setVec3(loc + "color", light.color);
    shaderProgram.setFloat(loc + "constant", light.constant);
    shaderProgram.setFloat(loc + "linear", light.linear);
    shaderProgram.setFloat(loc + "quadratic", light.quadratic);
    shaderProgram.setFloat(loc + "cutOff", light.cutOff);
    shaderProgram.setFloat(loc + "outerCutOff", light.outerCutOff);
}
void LightManager::setLight(ShaderProgram& shaderProgram, DirectionalLight& light)
{
    shaderProgram.use();
    std::string loc = "DirectionalLights[";
    loc += std::to_string(light.ID - maxSpotAmt - maxPointAmt);
    loc += "].";
    shaderProgram.setVec3(loc + "dir", light.dir);
    shaderProgram.setVec3(loc + "color", light.color);
}
void LightManager::setLight(ShaderProgram& shaderProgram, PointLight& light)
{
    std::string loc = "PointLights[";
    loc += std::to_string(light.ID - maxSpotAmt);
    loc += "].";
    shaderProgram.setVec3(loc + "pos", light.pos);
    shaderProgram.setVec3(loc + "color", light.color);
    shaderProgram.setFloat(loc + "constant", light.constant);
    shaderProgram.setFloat(loc + "linear", light.linear);
    shaderProgram.setFloat(loc + "quadratic", light.quadratic);
}

SpotLight& LightManager::getSpotLight(int id) {
    for (size_t i = 0; i < spotLights.size(); i++)
    {
        if (spotLights[i]->ID == id) {
            return *(spotLights[i]);
        }
    }
}

PointLight& LightManager::getPointLight(int id)
{
    for (size_t i = 0; i < pointLights.size(); i++)
    {
        if (pointLights[i]->ID == id) {
            return *(pointLights[i]);
        }
    }
}

DirectionalLight& LightManager::getDirectionalLight(int id)
{
    for (size_t i = 0; i < directionalLights.size(); i++)
    {
        if (directionalLights[i]->ID == id) {
            return *(directionalLights[i]);
        }
    }
}


