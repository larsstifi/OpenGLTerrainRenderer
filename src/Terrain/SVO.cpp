#include "SVO.h"
#include <stack>
#include <iostream>

//build svo
void SVO::buildOctree()
{
	struct Node {
		Node(svoNode* svoNode, glm::ivec3 pos, unsigned int depth) : svoNode(svoNode), pos(pos), depth(depth) {};
		svoNode* svoNode;
		glm::ivec3 pos;
		unsigned int depth;
	};

	std::stack<Node> stack;
	stack.push(Node(&root, glm::ivec3(0),0));
	unsigned int nodesAmt = 1;
	while (!stack.empty()) {
		Node curNode = stack.top();

		if (curNode.depth == depth) //if we reach the target depth we generate the density
		{ 
			curNode.svoNode->isLeaf = true;
			curNode.svoNode->density = ng.getDensity(curNode.pos);
			stack.pop();
		}
		else //not at target depth -> move down tree
		{ 
			if (!curNode.svoNode->children) { //first time working on node thus create children an push onto stack
				curNode.svoNode->children = new svoNode[8];
				for (unsigned int i = 0; i < 8; ++i) {
					int halflength = 1 << ((depth - curNode.depth) - 1);
					glm::ivec3 childPos = curNode.pos + childrenPos[i] * halflength;
					stack.push(Node(&curNode.svoNode->children[i], childPos, curNode.depth + 1));
				}
				nodesAmt += 8;
			}
			else //simplify octree
			{
				//check if all children are either full or empty
				bool isEmpty = true; 
				bool isFull = true;
				for (unsigned int i = 0; i < 8; ++i) {
					isEmpty = isEmpty && (curNode.svoNode->children[i].density < 0.f);
					isFull = isFull && (curNode.svoNode->children[i].density >= 0.f);
				}
				// if full or empty set node as leaf and remove children
				if (isFull) {
#ifdef _DEBUG
					std::cout << "Full Node simplified at " << curNode.pos.x << " " << curNode.pos.y << " " << curNode.pos.z << std::endl;
#endif
					delete[] curNode.svoNode->children;
					curNode.svoNode->density = 1.f;
					nodesAmt -= 8;
				}
				if (isEmpty) {
					std::cout << "Empty Node simplified at " << curNode.pos.x << " " << curNode.pos.y << " " << curNode.pos.z << std::endl;
					delete[] curNode.svoNode->children;
					curNode.svoNode->density = -1.f;
					nodesAmt -= 8;
				}
				stack.pop();
			}
		}
	}
}

std::vector<float> SVO::getDensity(glm::ivec3 pos, glm::ivec3 size, unsigned int LOD)
{
	std::vector<float> out;
	out.resize(size.x * size.y * size.z);
	struct Node {
		Node(svoNode* svoNode, glm::ivec3 nodePos, unsigned int depth, glm::ivec3 posInArray, glm::ivec3 sizeInArray) : svoNode(svoNode), nodePos(nodePos), depth(depth), posInArray(posInArray), sizeInArray(sizeInArray) {};
		svoNode* svoNode;
		glm::ivec3 nodePos;
		glm::ivec3 posInArray;
		glm::ivec3 sizeInArray;
		unsigned int depth;
	};
	std::stack<Node> stack;
	stack.push(Node(&root, glm::ivec3(0), 0, -pos, size));

	while (!stack.empty()) {
		Node curNode = stack.top();
		stack.pop();

		// Check if the current node is a leaf or at the desired LOD
		if (curNode.svoNode->isLeaf || curNode.depth == LOD) {
			// Sample the density for the entire cube represented by this node
			for (int x = 0; x < curNode.sizeInArray.x; ++x) {
				for (int y = 0; y < curNode.sizeInArray.y; ++y) {
					for (int z = 0; z < curNode.sizeInArray.z; ++z) {
						glm::ivec3 samplePos = curNode.nodePos + glm::ivec3(x, y, z);
						glm::ivec3 arrayPos = samplePos + curNode.posInArray;
						int index = arrayPos.x + size.x * (arrayPos.y + size.y * arrayPos.z);
						out[index] = curNode.svoNode->density;
					}
				}
			}
		}
		else {
			// Add children to the stack for further traversal
			for (int i = 0; i < 8; ++i) {
				glm::ivec3 childPos = curNode.nodePos + childrenPos[i] * (1 << (LOD - curNode.depth));
				glm::ivec3 childSize = glm::max(glm::ivec3(1), glm::ivec3(1 << (LOD - curNode.depth - 1)));
				stack.push(Node(&curNode.svoNode->children[i], childPos, curNode.depth + 1, curNode.posInArray, childSize));
			}
		}
	}
	return out;


}
