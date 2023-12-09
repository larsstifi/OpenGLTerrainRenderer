#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include<unordered_map>

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "glm/gtx/hash.hpp"

namespace dc{
	struct Vertex {
		glm::vec3 pos;
		glm::vec3 norm;
		glm::vec2 uv;
	};

	struct Mesh {
		std::vector <Vertex> vertices;
		std::vector<unsigned int> indices;
	};

	/*
	cornerIndices
	   6-----------7
	  /|          /|
	 / |         / |
	4-----------5  |
	|  |        |  |
	|  2--------|--3
	| /         | /
	|/          |/
	0-----------1
	*/
	//holds the positions of the corners
	static const glm::uvec3 cornerPos[8] = {
		glm::uvec3(0,0,0),
		glm::uvec3(1,0,0),
		glm::uvec3(0,1,0),
		glm::uvec3(1,1,0),
		glm::uvec3(0,0,1),
		glm::uvec3(1,0,1),
		glm::uvec3(0,1,1),
		glm::uvec3(1,1,1)
	};
	//holds the edges defined by corner indices
	static const int edges[12][2] = {
		{0,1},
		{0,2},
		{0,4},
		{1,3},
		{1,5},
		{2,3},
		{2,6},
		{3,7},
		{4,5},
		{4,6},
		{5,7},
		{6,7}
	};

	Mesh generateMesh(std::vector<float>& density, unsigned int size);
}