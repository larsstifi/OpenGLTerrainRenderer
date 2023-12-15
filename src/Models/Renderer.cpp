#include "Renderer.h"
#include <imgui.h>

void Renderer::RenderObjects()
{
	uint32_t activeMat = std::numeric_limits<uint32_t>::max();
	std::vector<std::weak_ptr<Drawable>>::iterator itO = objects.begin();
	std::vector<uint32_t>::iterator itM = materialIndices.begin();
	for (; itO != objects.end() && itM != materialIndices.end();) {
		std::shared_ptr<Drawable> curObject;
		if (!(curObject = (*itO).lock())) { itO = objects.erase(itO); itM = materialIndices.erase(itM); continue; }

		uint32_t curMatIndex = *itM;
		RenderMaterial mat = *materials[curMatIndex].get();
		if (activeMat != curMatIndex) {
			activeMat = curMatIndex;
			if (materials[activeMat]->matType != DISABLED) {
				ShaderProgram::use(mat.spID);
				ShaderProgram::setMat4(mat.spID, "view", view);
				ShaderProgram::setMat4(mat.spID, "projection", projection);
				ShaderProgram::setVec3(mat.spID, "viewPos", camPos);
				lightManager->setLights(mat.spID);
				ShaderProgram::setVec3(mat.spID, "AmbientColor", mat.AmbientColor);
				ShaderProgram::setVec3(mat.spID, "DiffuseColor", mat.DiffuseColor);
				ShaderProgram::setVec3(mat.spID, "SpecularColor", mat.SpecularColor);
				ShaderProgram::setFloat(mat.spID, "SpecularExponent", mat.SpecularExponent);
				ShaderProgram::setFloat(mat.spID, "OpticalDensity", mat.OpticalDensity);
				ShaderProgram::setFloat(mat.spID, "Dissolve", mat.Dissolve);
				ShaderProgram::setFloat(mat.spID, "Illumination", mat.Illumination);
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, mat.AmbientTexture);
				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, mat.DiffuseTexture);
				glActiveTexture(GL_TEXTURE2);
				glBindTexture(GL_TEXTURE_2D, mat.SpecularTexture);
				glActiveTexture(GL_TEXTURE3);
				glBindTexture(GL_TEXTURE_2D, mat.SpecularHightlightTexture);
				glActiveTexture(GL_TEXTURE4);
				glBindTexture(GL_TEXTURE_2D, mat.AlphaTexture);
				glActiveTexture(GL_TEXTURE5);
				glBindTexture(GL_TEXTURE_2D, mat.BumpTexture);
			}
		}
		glm::mat4 model = glm::mat4(1.f);
		curObject->draw(mat.spID, model);
		++itM;
		++itO;
	}
}

void Renderer::Render()
{
	if(gui_RenderObjects) RenderObjects();
}

void Renderer::RenderImgui()
{
	ImGui::Checkbox("Show Objects", &gui_RenderObjects);
	ImGui::SameLine();
	ImGui::Checkbox("Show Debug", &gui_RenderDebug);
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

unsigned int Renderer::loadTexture(std::string filePath, int mode) {
	//textures
	if (filePath == "" || filePath.back() == '/') {
		return 0;
	}
	for (int i = 0; i < loadedTextures.size(); i++)
	{
		if (loadedTextures[i].path == filePath) {
			std::cout << "Old Texture at " << filePath << std::endl;
			return loadedTextures[i].id;
		}
	}

	stbi_set_flip_vertically_on_load(true);
	unsigned int texture;
	glGenTextures(1, &texture);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	// set the texture wrapping/filtering options (on the currently bound texture object)
	if (mode == 0) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else if (mode == 1) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		float borderColor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}

	// load and generate the texture
	int width, height, nrChannels;
	unsigned char* data = stbi_load(filePath.c_str(), &width, &height, &nrChannels, 0);
	if (data)
	{
		GLenum format = GL_RGB;
		if (nrChannels == 1)
			format = GL_RED;
		else if (nrChannels == 3)
			format = GL_RGB;
		else if (nrChannels == 4)
			format = GL_RGBA;
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		std::cout << "New Texture at " << filePath << " Number of Channels: " << nrChannels << std::endl;
	}
	else
	{
		std::cout << "Failed to load texture at " << filePath << std::endl;
		stbi_image_free(data);
		return 0;
	}
	stbi_image_free(data);
	Texture t;
	t.id = texture;
	t.path = filePath;
	loadedTextures.push_back(t);

	return texture;
}
