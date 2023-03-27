#pragma once
#include<Models/Drawable.h>
#include<Terrain/DualContouring.h>
#include<Models/MeshRenderer.h>
#include <FastNoise/FastNoise.h>
#include<Terrain/NoiseGenerator.h>
#include <glm/glm.hpp>
#include <Models/Model.h>

class TerrainChunk : public Drawable 
{
protected:
	dc::Mesh mesh;
    MeshRenderer meshRenderer;
    Material mat;
public:
    glm::vec3 pos;
    TerrainChunk() { this->pos = glm::vec3(0); mesh.vertices = std::vector<dc::Vertex>();   mesh.indices = std::vector<unsigned int>(); }
    TerrainChunk(glm::vec3 pos) { this->pos = pos; mesh.vertices = std::vector<dc::Vertex>();   mesh.indices = std::vector<unsigned int>();}
    void setMat(unsigned int texture);
	void draw(ShaderProgram& shader, glm::mat4& model, bool setMat = true);
	void drawInstanced(ShaderProgram& shader, glm::mat4& model, unsigned int count, bool setMat = true);
    void generateChunk(NoiseGenerator ng, unsigned int size, float freq = 0.2f, float scale = 1.f, int seed = 0);
    void fillBuffers();
    void clear();
};
