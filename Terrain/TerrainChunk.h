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
    TerrainChunk() { this->pos = glm::vec3(0); }
    TerrainChunk(glm::vec3 pos) { this->pos = pos; }
    void setMat(unsigned int texture);
	void draw(ShaderProgram& shader, glm::mat4& model, bool setMat = true);
	void drawInstanced(ShaderProgram& shader, glm::mat4& model, unsigned int count, bool setMat = true);
    void generateChunk(NoiseGenerator ng, unsigned int size, int seed = 0);
};
