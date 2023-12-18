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
public:
    glm::vec3 pos;
    TerrainChunk() { this->pos = glm::vec3(0); mesh.vertices = std::vector<dc::Vertex>();   mesh.indices = std::vector<unsigned int>(); }
    TerrainChunk(glm::vec3 pos) { this->pos = pos; mesh.vertices = std::vector<dc::Vertex>();   mesh.indices = std::vector<unsigned int>();}
	void draw(unsigned int spID, glm::mat4& model);
	void drawInstanced(unsigned int spID, glm::mat4& model, unsigned int count);
    void generateChunk(NoiseGenerator ng, unsigned int size, float freq = 0.2f, float scale = 1.f, int seed = 0);
    void fillBuffers();
    void clear();
};
