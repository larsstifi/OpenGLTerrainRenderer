#pragma once
#include<stdint.h>
#include <Terrain/NoiseGenerator.h>

struct svoNode {
	svoNode* children = nullptr;
	float density;
	bool isLeaf = false;
};

static glm::ivec3 childrenPos[8]
{
	glm::ivec3(0, 0, 0),
	glm::ivec3(0, 0, 1),
	glm::ivec3(0, 1, 0),
	glm::ivec3(0, 1, 1),
	glm::ivec3(1, 0, 0),
	glm::ivec3(1, 0, 1),
	glm::ivec3(1, 1, 0),
	glm::ivec3(1, 1, 1)

};

class SVO
{
private:
	svoNode root;
	NoiseGenerator ng;
	unsigned int depth;
public:
	SVO(unsigned int depth, NoiseGenerator ng) : depth(depth), ng(ng) { buildOctree(); };
	void buildOctree();
	std::vector<float> getDensity(glm::ivec3 pos, glm::ivec3 size, unsigned int LOD);
	void Update();
};

