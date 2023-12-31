#pragma once
#include <Models/Drawable.h>
#include <Terrain/TerrainChunk.h>
#include <Terrain/NoiseGenerator.h>
#include<vector>
#include<glm/glm.hpp>
#include <GLFW/glfw3.h>
#include <stack>
#include <imgui.h>
#include <thread>
#include <queue>
#include <memory>

#define MAX_CHUNKS_READ_PER_FRAME 4
#define MAX_CHUNKS_GEN_PER_FRAME 8
#define MAX_CHUNKS_IN_GENERATION 8
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
	float LOD_Falloff = 1.f;
	ImGuiTextBuffer gui_Cout;
	glm::vec3 playerPos = glm::vec3(0.f);
	glm::vec3 octreePos = glm::vec3(0.f);
	Octree(uint32_t treeDepth) : TreeDepth(treeDepth) {
		root.depth = TreeDepth;
		ng.setScale(NoiseScale);
		ng.setGain(NoiseGain);
		ng.setLacunarity(NoiseLacunarity);
		octreePos = glm::vec3(-(1 << (treeDepth - 1)));
	};
	~Octree();
	void draw(unsigned int spID, glm::mat4& model);
	void Update();
	void drawInstanced(unsigned int spID, glm::mat4& model, unsigned int count);
	void drawImgui();
	void setDepth(unsigned int depth) { TreeDepth = depth; };
private:
	unsigned int TreeDepth;
	OctreeNode root;
	std::vector<OctreeNode*> activeNodes;
	std::list<std::pair<std::thread*, OctreeNode*>> terrainGenerationThreads;
	NoiseGenerator ng;
	int chunkSize =  16;
	float TerrainScale = 1.f; 
	float NoiseScale = 0.5f;
	float NoiseGain = 0.9f;
	float NoiseLacunarity = 0.9f;
	bool UpdateTerrain = true;
	void clearNode(OctreeNode* node);
	void clearChildren(OctreeNode* node);
	void resetOctree();
};