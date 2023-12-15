#pragma once
#include <Shaders\ShaderProgram.h>
#include <Models\MeshRenderer.h>

class Drawable {
public:
	virtual void draw(unsigned int spID, glm::mat4& model) = 0;
	virtual void drawInstanced(unsigned int spID, glm::mat4& model, unsigned int count) = 0;
	virtual void drawImgui() {};
};