#include "LightManager.h"

int LightManager::addLight(std::shared_ptr<SpotLight> newLight)
{
    if (spotLights.size() >= maxSpotAmt)
        return -1;

    newLight->ID = spotIdTracker.top();
    spotIdTracker.pop();
    spotLights.push_back(newLight);
    return newLight->ID;
}
int LightManager::addLight(std::shared_ptr<PointLight> newLight)
{
    
	if (pointLights.size() >= maxPointAmt)
		return -1;

    newLight->ID = pointIdTracker.top();
    pointIdTracker.pop();
	pointLights.push_back(newLight);
    return newLight->ID;
}
int LightManager::addLight(std::shared_ptr<DirectionalLight> newLight)
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

void LightManager::setLights(unsigned int spID)
{
    ShaderProgram::setInt(spID, "SpotLightAmt", spotLights.size());
    ShaderProgram::setInt(spID, "PointLightAmt", pointLights.size());
    ShaderProgram::setInt(spID, "DirectionalLightAmt", directionalLights.size());
    for (size_t i = 0; i < directionalLights.size(); i++)
    {
        setLight(spID, *(directionalLights[i]));
    }
    for (size_t i = 0; i < spotLights.size(); i++)
    {
        setLight(spID, *(spotLights[i]));
    }
    for (size_t i = 0; i < pointLights.size(); i++)
    {
        setLight(spID, *(pointLights[i]));
    }
}

void LightManager::setLight(unsigned int spID, int id)
{
    ShaderProgram::setInt(spID, "SpotLightAmt", maxSpotAmt);
    ShaderProgram::setInt(spID, "PointLightAmt", maxPointAmt);
    ShaderProgram::setInt(spID, "DirectionalLightAmt", maxDirectionalAmt);
    if (id < maxSpotAmt) {
        setLight(spID, *getSpotLight(id));
        return;
    }
    if (id < maxPointAmt) {
        setLight(spID, *getPointLight(id));
        return;
    }
    setLight(spID, *getDirectionalLight(id));
}

void LightManager::setLight(unsigned int spID, SpotLight& light)
{
    std::string loc = "SpotLights[";
    loc += std::to_string(light.ID);
    loc += "].";
    ShaderProgram::setVec3(spID, loc + "pos", light.pos);
    ShaderProgram::setVec3(spID, loc + "dir", light.dir);
    ShaderProgram::setVec3(spID, loc + "color", light.color);
    ShaderProgram::setFloat(spID, loc + "constant", light.constant);
    ShaderProgram::setFloat(spID, loc + "linear", light.linear);
    ShaderProgram::setFloat(spID, loc + "quadratic", light.quadratic);
    ShaderProgram::setFloat(spID, loc + "cutOff", light.cutOff);
    ShaderProgram::setFloat(spID, loc + "outerCutOff", light.outerCutOff);
}
void LightManager::setLight(unsigned int spID, DirectionalLight& light)
{
    std::string loc = "DirectionalLights[";
    loc += std::to_string(light.ID - maxSpotAmt - maxPointAmt);
    loc += "].";
    ShaderProgram::setVec3(spID, loc + "dir", light.dir);
    ShaderProgram::setVec3(spID, loc + "color", light.color);
}
void LightManager::setLight(unsigned int spID, PointLight& light)
{
    std::string loc = "PointLights[";
    loc += std::to_string(light.ID - maxSpotAmt);
    loc += "].";
    ShaderProgram::setVec3(spID, loc + "pos", light.pos);
    ShaderProgram::setVec3(spID, loc + "color", light.color);
    ShaderProgram::setFloat(spID, loc + "constant", light.constant);
    ShaderProgram::setFloat(spID, loc + "linear", light.linear);
    ShaderProgram::setFloat(spID, loc + "quadratic", light.quadratic);
}

std::shared_ptr<SpotLight> LightManager::getSpotLight(int id) {
    for (size_t i = 0; i < spotLights.size(); i++)
    {
        if (spotLights[i]->ID == id) {
            return spotLights[i];
        }
    }
}

std::shared_ptr<PointLight> LightManager::getPointLight(int id)
{
    for (size_t i = 0; i < pointLights.size(); i++)
    {
        if (pointLights[i]->ID == id) {
            return (pointLights[i]);
        }
    }
}

std::shared_ptr<DirectionalLight> LightManager::getDirectionalLight(int id)
{
    for (size_t i = 0; i < directionalLights.size(); i++)
    {
        if (directionalLights[i]->ID == id) {
            return directionalLights[i];
        }
    }
}


