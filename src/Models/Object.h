#pragma once

#include <glad/glad.h> 
#include <GLFW/glfw3.h>
#include "stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <Shaders/ShaderProgram.h>


class Object {
private:
	glm::mat4 model;
	unsigned int VAO;
	unsigned int VBO;
	unsigned int EBO;

	unsigned int texture;
public:
	void readFromFile(const char* filePath);
	void draw(ShaderProgram shaderProgram, float DT) const;
};
