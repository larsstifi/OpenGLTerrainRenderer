#pragma once
#include <Models/Drawable.h>
#include <Models/MeshRenderer.h>
#include <imgui.h>

class Plane : public Drawable
{
	glm::vec3 pos; 
	glm::vec2 dim;
	MeshRenderer meshRenderer;
public:
	Plane(glm::vec3 pos, glm::vec2 dim);
	void draw(unsigned int spID, glm::mat4& model);
	void drawInstanced(unsigned int spID, glm::mat4& model, unsigned int count);
	void drawImgui();
};

