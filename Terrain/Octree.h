#pragma once
#include <Models/Drawable.h>
#include <Terrain/TerrainChunk.h>
#include<vector>
#include<glm/glm.hpp>
#include <stack>

struct OctreeNode {
	OctreeNode* children;
	bool leaf = true;
	TerrainChunk* tc;
};

class Octree : public Drawable
{
public:
	void draw(ShaderProgram& shader, glm::mat4& model, bool setMat = true);
	void drawInstanced(ShaderProgram& shader, glm::mat4& model, unsigned int count, bool setMat = true);
	void setDepth(unsigned int depth) { TreeDepth = depth; };
	std::vector<OctreeNode*> getActiveNodes(glm::vec3 pos, float LOD_Falloff);
private:
	unsigned int TreeDepth;
	OctreeNode root;
	std::vector<TerrainChunk> activeChunks;
	
};