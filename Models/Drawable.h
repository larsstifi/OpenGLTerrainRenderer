#pragma once
#include <Shaders\ShaderProgram.h>
#include <Models\MeshRenderer.h>

class Drawable {
public:
	virtual void draw(ShaderProgram& shader, glm::mat4& model, bool setMat = true) = 0;
	virtual void drawInstanced(ShaderProgram& shader, glm::mat4& model, unsigned int count, bool setMat = true) = 0;
};