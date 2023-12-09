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
	void draw(ShaderProgram& shader, glm::mat4& model, bool setMat = true);
	void drawInstanced(ShaderProgram& shader, glm::mat4& model, unsigned int count, bool setMat = true);
	void drawImgui();
};

