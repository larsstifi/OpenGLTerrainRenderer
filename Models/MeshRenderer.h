#pragma once

#include <glad/glad.h> 
#include <GLFW/glfw3.h>
#include "stb_image.h"
#include <Models\OBJ_Loader.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <Shaders/ShaderProgram.h>

#include <iostream>
#include <fstream>
#include <string>
#include <vector>




class MeshRenderer {
private:
	unsigned int VAO;
	unsigned int VBO;
	unsigned int EBO;
	unsigned int indicesAmt;
	unsigned int texture;
public:
	void draw(ShaderProgram& shaderProgram, glm::mat4 model) const;
	void fillBuffers(std::vector<objl::Vertex>& vert, std::vector<unsigned int>& indi);
	void setTexture(const char * filePath);
};
