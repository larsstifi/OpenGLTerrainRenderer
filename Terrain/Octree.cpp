#include<Terrain/Octree.h>

static glm::ivec3 ChildrenPos[8]{
	glm::ivec3(0, 0, 0),
	glm::ivec3(0, 0, 1),
	glm::ivec3(0, 1, 0),
	glm::ivec3(0, 1, 1),
	glm::ivec3(1, 0, 0),
	glm::ivec3(1, 0, 1),
	glm::ivec3(1, 1, 0),
	glm::ivec3(1, 1, 1)

};

void Octree::draw(ShaderProgram& shader, glm::mat4& model, bool setMat) {
	unsigned int CreatedChunksAmt = 0;
	bool childNotCreated = false;
	std::stack<OctreeNode*> stack; //keep track of Nodes and how many children we already iterated over
	std::stack<unsigned int> iterationStack; //keep track of Nodes and how many children we already iterated over
	std::stack<unsigned int> childrenIndexStack; //keep track of which children this node is in its parent 
	stack.push(&root);
	iterationStack.push(0);
	childrenIndexStack.push(0);
	unsigned int curDepth = TreeDepth;
	glm::ivec3 curPos = glm::ivec3(0);
	while (!stack.empty()) {
		//get current node values
		unsigned int curChildIndex = childrenIndexStack.top();
		OctreeNode* curNode = stack.top();
		unsigned int currentIteration = iterationStack.top();
		//calculate target LOD for this chunk
		int halfsize = 1 << (curDepth - 1);
		glm::vec3 d = (glm::vec3(curPos) + glm::vec3(halfsize) - playerPos / (float)chunkSize) ;
		int targetDepth = (sqrtf(glm::dot(d, d))- halfsize * 2.f) * LOD_Falloff;
		if (targetDepth < 0) targetDepth = 0;

		//check if last node or below target depth, if so add this node and move up in tree          only check if this is the first iteration
		bool isActive = (currentIteration == 0 && (int)curDepth <= targetDepth) || curDepth == 0;
		if (isActive && !(curNode->tcSet)) {
				if (CreatedChunksAmt == MAX_CHUNKS_PER_FRAME) {
					//move up a level in LOD
					childNotCreated = true;
					stack.pop();
					iterationStack.pop();
					childrenIndexStack.pop();
					curDepth++;
					curPos -= ChildrenPos[curChildIndex] * (1 << (curDepth - 1));
					continue;
				}
				
				TerrainChunk* newTC;
				newTC = new TerrainChunk();
				newTC->pos = glm::vec3(curPos * chunkSize) + octreePos;
				newTC->generateChunk(ng, chunkSize, 0.01f, (1 << curDepth));
				newTC->setMat(texture);
				curNode->tc = newTC;
				curNode->tcSet = true;
				gui_Cout.appendf("New Chunk at: %.0f %.0f %.0f\n", newTC->pos.x, newTC->pos.y, newTC->pos.z);
				CreatedChunksAmt++;
				clearChildren(curNode);
		}
		if (isActive || (childNotCreated && curNode->tcSet)) {
			//draw node
			//std::cout << "Draw Chunk at: " << curNode->tc->pos.x << " " << curNode->tc->pos.y << " " << curNode->tc->pos.z << " Of Size: " << (1<<curDepth) << std::endl;
			childNotCreated = false;
			glm::mat4 modelMat(1.f);
			modelMat = glm::translate(modelMat, (curNode->tc->pos));
			modelMat = glm::scale(modelMat, glm::vec3(glm::ivec3(1.f) * (1 << curDepth)));
			
			modelMat = model * modelMat;
			curNode->tc->draw(shader, modelMat, setMat);
		}
		if (isActive || currentIteration == 8) {
			//move up a level in LOD
			stack.pop();
			iterationStack.pop();
			childrenIndexStack.pop();
			curDepth++;
			curPos -= ChildrenPos[curChildIndex] * (1 << (curDepth - 1));
			continue;
		}

		if (currentIteration == 0 && curNode->leaf) { //check if leaf if so we split the node
			curNode->children = new OctreeNode[8];
			for (uint16_t i = 0; i < 8; i++)
			{
				curNode->children[i].leaf = true;
				curNode->children[i].tcSet = false;

			}
			curNode->leaf = false;

			//std::cout << "Split Node at: " << curPos.x << " " << curPos.y << " " << curPos.z << std::endl;
			//std::cout << "Is leaf : " << curNode->leaf << std::endl;
		}

		//increment
		iterationStack.pop();
		iterationStack.push(currentIteration + 1);
		//push next children onto stack
		stack.push(&(curNode->children[currentIteration]));
		iterationStack.push(0);
		childrenIndexStack.push(currentIteration);
		//move down
		curDepth--;
		curPos += ChildrenPos[currentIteration] * (1 << (curDepth));
		
	}
}
void Octree::drawInstanced(ShaderProgram& shader, glm::mat4& model, unsigned int count, bool setMat) {

}

void Octree::clearNode(OctreeNode* node)
{
	if (!(node->leaf)) {
		for (size_t i = 0; i < 8; i++)
		{
			clearNode(&(node->children[i]));
		}
		delete[] node->children;
		node->leaf = true;
	}
	if (node->tcSet) {
		node->tc->clear();
		delete node->tc;
		node->tcSet = false;
	}
}

void Octree::clearChildren(OctreeNode* node) {
	if (!(node->leaf)) {
		for (size_t i = 0; i < 8; i++)
		{
			clearNode(&(node->children[i]));
		}
		node->leaf = true;
	}
}
