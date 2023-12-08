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
#include <memory>

#define MAX_CHUNKS_READ_PER_FRAME 1
#define MAX_CHUNKS_GEN_PER_FRAME 4
#define MAX_CHUNKS_IN_GENERATION 16
struct OctreeNode {
	struct OctreeNode* children;
	bool leaf = true;
	bool tcSet = false;
	bool isGenerating = false;
	TerrainChunk* tc;
	uint32_t depth;
};

class Octree : public Drawable
{
public:
	float LOD_Falloff = 0.5f;
	ImGuiTextBuffer gui_Cout;
	glm::vec3 playerPos = glm::vec3(0.f);
	glm::vec3 octreePos = glm::vec3(-200, -200, -200);
	Octree(uint32_t treeDepth) : TreeDepth(treeDepth) { root.depth = TreeDepth; ng.setScale(NoiseScale); };
	~Octree();
	void draw(ShaderProgram& shader, glm::mat4& model, bool setMat = true);
	void Update();
	void drawInstanced(ShaderProgram& shader, glm::mat4& model, unsigned int count, bool setMat = true);
	void drawImgui();
	void setDepth(unsigned int depth) { TreeDepth = depth; };
private:
	unsigned int TreeDepth;
	OctreeNode root;
	std::vector<OctreeNode*> activeNodes;
	std::list<std::pair<std::thread*, OctreeNode*>> terrainGenerationThreads;
	NoiseGenerator ng;
	int chunkSize = 16;
	float TerrainScale = 1.f;
	float NoiseScale = 3.f;
	float NoiseGain = 1.f;
	float NoiseLacunarity = 1.f;
	bool UpdateTerrain = true;
	void clearNode(OctreeNode* node);
	void clearChildren(OctreeNode* node);
	void resetOctree();
};