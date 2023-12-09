#include "Renderer.h"
#include <imgui.h>

void Renderer::RenderObjects()
{
	shaderProgram->use();
	shaderProgram->setMat4("view", view);
	shaderProgram->setMat4("projection", projection);
	shaderProgram->setVec3("viewPos", camPos);
	lightManager->setLights(*shaderProgram);
	uint32_t activeMat = std::numeric_limits<uint32_t>::max();
	std::vector<std::weak_ptr<Drawable>>::iterator itO = objects.begin();
	std::vector<uint32_t>::iterator itM = materialIndices.begin();
	for (; itO != objects.end() && itM != materialIndices.end();) {
		std::shared_ptr<Drawable> curObject;
		if (!(curObject = (*itO).lock())) { itO = objects.erase(itO); itM = materialIndices.erase(itM); continue; }

		uint32_t curMatIndex = *itM;

		if (activeMat != curMatIndex) {
			activeMat = curMatIndex;
			if (materials[activeMat]->matType != DISABLED) {

				shaderProgram->setVec3("AmbientColor", materials[activeMat]->AmbientColor);
				shaderProgram->setVec3("DiffuseColor", materials[activeMat]->DiffuseColor);
				shaderProgram->setVec3("SpecularColor", materials[activeMat]->SpecularColor);
				shaderProgram->setFloat("SpecularExponent", materials[activeMat]->SpecularExponent);
				shaderProgram->setFloat("OpticalDensity", materials[activeMat]->OpticalDensity);
				shaderProgram->setFloat("Dissolve", materials[activeMat]->Dissolve);
				shaderProgram->setFloat("Illumination", materials[activeMat]->Illumination);
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, materials[activeMat]->AmbientTexture);
				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, materials[activeMat]->DiffuseTexture);
				glActiveTexture(GL_TEXTURE2);
				glBindTexture(GL_TEXTURE_2D, materials[activeMat]->SpecularTexture);
				glActiveTexture(GL_TEXTURE3);
				glBindTexture(GL_TEXTURE_2D, materials[activeMat]->SpecularHightlightTexture);
				glActiveTexture(GL_TEXTURE4);
				glBindTexture(GL_TEXTURE_2D, materials[activeMat]->AlphaTexture);
				glActiveTexture(GL_TEXTURE5);
				glBindTexture(GL_TEXTURE_2D, materials[activeMat]->BumpTexture);
			}
		}
		glm::mat4 model = glm::mat4(1.f);
		curObject->draw(*shaderProgram, model, materials[activeMat]->matType == DISABLED);
		++itM;
		++itO;
	}
}

void Renderer::Render()
{
	if(gui_RenderObjects) RenderObjects();
	if (gui_RenderDebug) RenderDebug();
}

void Renderer::RenderImgui()
{
	ImGui::Checkbox("Show Objects", &gui_RenderObjects);
	ImGui::SameLine();
	ImGui::Checkbox("Show Debug", &gui_RenderDebug);
}


void Renderer::RenderDebug()
{
	if (gui_RenderDebug && debugShader.get() != nullptr) {
		debugShader->use();
		debugShader->setMat4("view", view);
		debugShader->setMat4("projection", projection);
		debugShader->setVec3("viewPos", camPos);
		debugShader->setFloat("dt", (float)glfwGetTime());
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, debugTexture);
		std::vector<std::weak_ptr<Drawable>>::iterator itO = objects.begin();
		std::vector<uint32_t>::iterator itM = materialIndices.begin();
		for (; itO != objects.end() && itM != materialIndices.end();) {
			glm::mat4 model = glm::mat4(1.f);
			std::shared_ptr<Drawable> curObject;
			if (!(curObject = (*itO).lock())) { itO = objects.erase(itO); itM = materialIndices.erase(itM); continue; }
			curObject->draw(*debugShader, model, false);
			++itM;
			++itO;
		}
	}
}

void Renderer::addObject(std::shared_ptr<Drawable> object, uint32_t matIndex)
{
	objects.push_back(object);
	materialIndices.push_back(matIndex);

}

void Renderer::addLight(std::shared_ptr<SpotLight> sl)
{
	lightManager->addLight(sl);
}

void Renderer::addLight(std::shared_ptr<PointLight> pl)
{
	lightManager->addLight(pl);
}

void Renderer::addLight(std::shared_ptr<DirectionalLight> dl)
{
	lightManager->addLight(dl);
}

uint32_t Renderer::addMaterial(std::shared_ptr<RenderMaterial> mat)
{
	uint32_t index = materials.size();
	materials.push_back(mat);
	return index;
}
