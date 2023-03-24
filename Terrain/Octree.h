#pragma once
#include <Models/Drawable.h>
#include <Terrain/TerrainChunk.h>
#include <Terrain/NoiseGenerator.h>
#include<vector>
#include<glm/glm.hpp>
#include <GLFW/glfw3.h>
#include <stack>
#include <imgui/imgui.h>
#include <thread>
#include <queue>

#define MAX_CHUNKS_PER_FRAME 8
struct OctreeNode {
	struct OctreeNode* children;
	bool leaf = true;
	bool tcSet = false;
	TerrainChunk* tc;
};

class Octree : public Drawable
{
public:
	float LOD_Falloff = 0.5f;
	ImGuiTextBuffer gui_Cout;
	glm::vec3 playerPos = glm::vec3(0.f);
	glm::vec3 octreePos = glm::vec3(0, -50, 0);
	void draw(ShaderProgram& shader, glm::mat4& model, bool setMat = true);
	void drawInstanced(ShaderProgram& shader, glm::mat4& model, unsigned int count, bool setMat = true);
	void setDepth(unsigned int depth) { TreeDepth = depth; };
	std::vector<OctreeNode*> getActiveNodes(glm::vec3 pos, float LOD_Falloff);
	void setTexture(unsigned int texture) { this->texture = texture; }
private:
	unsigned int TreeDepth;
	OctreeNode root;
	std::vector<TerrainChunk> activeChunks;
	std::list<std::pair<std::thread*, TerrainChunk*>> terrainGenerationThreads;
	NoiseGenerator ng;
	int chunkSize = 16;
	unsigned int texture;
	void clearNode(OctreeNode* node);
	void clearChildren(OctreeNode* node);

};