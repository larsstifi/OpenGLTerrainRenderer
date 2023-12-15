#include "Models/Plane.h"

Plane::Plane(glm::vec3 pos, glm::vec2 dim) : pos(pos), dim(dim){
	std::vector<dc::Vertex> vertices;
	for (uint32_t x = 0; x < 2; x++) {
		for (uint32_t y = 0; y < 2; y++) {
			dc::Vertex vert;
			vert.pos = glm::vec3(x, 0, y);
			vert.uv = glm::vec2(x, y);
			vert.norm = glm::vec3(0, 1, 0);
			vertices.push_back(vert);
		}
	}

	
	std::vector<uint32_t> indices{ 0, 1, 3, 3, 2, 0 };
	meshRenderer = MeshRenderer();
	meshRenderer.fillBuffers(vertices, indices);
}
void Plane::draw(unsigned int spID, glm::mat4& model)
{
	model = glm::translate(model, pos);
	model = glm::scale(model, glm::vec3(dim.x, 1, dim.y));
	
	ShaderProgram::setMat4(spID, "model", model);
	
	meshRenderer.draw();
}
void Plane::drawInstanced(unsigned int spID, glm::mat4& model, unsigned int count)
{
	model = glm::translate(model, pos);
	model = glm::scale(model, glm::vec3(dim, 1));
	ShaderProgram::setMat4(spID, "model", model);

	meshRenderer.drawInstanced(count);
}

void Plane::drawImgui()
{
	ImGui::Text("Plane Settings");
	ImGui::DragFloat2("Dimensions", glm::value_ptr(dim));
	ImGui::DragFloat3("Position", glm::value_ptr(pos));
}



