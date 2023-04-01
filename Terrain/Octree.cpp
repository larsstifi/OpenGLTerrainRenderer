#include<Terrain/Octree.h>

static glm::ivec3 ChildrenPos[8]
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

int executeGeneration(TerrainChunk* tc, NoiseGenerator ng, int size, float freq, float scale, int seed) 
{
	tc->generateChunk(ng, size, freq, scale, seed);
	return 0;
}

Octree::~Octree()
{
	clearNode(&root);
}

void Octree::Update() 
{
	//TODO:: refactor
	activeNodes = std::vector<OctreeNode*>();
	//first we fill the buffers of the terrainchunks created in the last loop
	int size = terrainGenerationThreads.size();
	if (size > MAX_CHUNKS_READ_PER_FRAME) size = MAX_CHUNKS_READ_PER_FRAME;
	for (size_t i = 0; i < size; i++)
	{
		std::pair<std::thread*, OctreeNode*> pair = terrainGenerationThreads.front(); terrainGenerationThreads.pop_front();
		std::thread* curThread = pair.first;
		OctreeNode* curNode = pair.second;
		curThread->join();
		delete curThread;
		if (curNode && curNode->tc) {
			curNode->tc->fillBuffers();
			curNode->tcSet = true;
			clearChildren(curNode);
		}

	}

	glm::vec3 octreeCornerPos = octreePos - glm::vec3((float)(1 << (TreeDepth - 1)));
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
		glm::vec3 d = ((glm::vec3(curPos) + glm::vec3(halfsize)) * (float)chunkSize + octreeCornerPos) * TerrainScale - playerPos;
		int targetDepth = (sqrtf(glm::dot(d, d)) - halfsize * 2.f * TerrainScale * chunkSize) * LOD_Falloff;
		if (targetDepth < 0) targetDepth = 0;

		//check if last node or below target depth, if so add this node and move up in tree          only check if this is the first iteration
		bool isActive = (currentIteration == 0 && (int)curDepth <= targetDepth) || curDepth == 0;
		if (isActive && !(curNode->tcSet)) {
			if (!(curNode->isGenerating) && terrainGenerationThreads.size() < MAX_CHUNKS_IN_GENERATION && CreatedChunksAmt < MAX_CHUNKS_GEN_PER_FRAME) {
				TerrainChunk* newTC;
				newTC = new TerrainChunk();
				newTC->pos = (glm::vec3(curPos * chunkSize) + octreeCornerPos);
				curNode->tc = newTC;
				curNode->isGenerating = true;
				CreatedChunksAmt++;
				std::thread* newThread = new std::thread(executeGeneration, newTC, ng, chunkSize, NoiseFrequency, (float)(1 << curDepth), 0);
				terrainGenerationThreads.push_back(std::pair<std::thread*, OctreeNode*>(newThread, curNode));
			}

			//move up a level in LOD
			childNotCreated = true;
			stack.pop();
			iterationStack.pop();
			childrenIndexStack.pop();
			curDepth++;
			curPos -= ChildrenPos[curChildIndex] * (1 << (curDepth - 1));
			continue;
		}
		if (isActive || (childNotCreated && curNode->tcSet)) {
			//active node
			childNotCreated = false;
			activeNodes.push_back(curNode);
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
				curNode->children[i].depth = curNode->depth - 1;

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

void Octree::draw(ShaderProgram& shader, glm::mat4& model, bool setMat)
{
	for (OctreeNode* curNode : activeNodes) {
		glm::mat4 modelMat(1.f);
		modelMat = glm::scale(modelMat, glm::vec3(TerrainScale));
		modelMat = glm::translate(modelMat, (curNode->tc->pos));
		modelMat = glm::scale(modelMat, glm::vec3(1 << curNode->depth));

		modelMat = model * modelMat;
		curNode->tc->draw(shader, modelMat, setMat);
	}

}
void Octree::drawInstanced(ShaderProgram& shader, glm::mat4& model, unsigned int count, bool setMat)
{
	for (OctreeNode* curNode : activeNodes) {
		glm::mat4 modelMat(1.f);
		modelMat = glm::scale(modelMat, glm::vec3(TerrainScale));
		modelMat = glm::translate(modelMat, (curNode->tc->pos));
		modelMat = glm::scale(modelMat, glm::vec3(1 << curNode->depth));

		modelMat = model * modelMat;
		curNode->tc->drawInstanced(shader, modelMat, count, setMat);
	}
}
void Octree::drawImgui()
{
	ImGui::Text("Octree Settings");
	if (ImGui::SliderInt("Chunk Size", &chunkSize, 3, 64))resetOctree();
	if (ImGui::DragFloat3("Octree Position", glm::value_ptr(octreePos), 1.f)) resetOctree();
	if (ImGui::SliderInt("Octree Depth", (int*)&TreeDepth, 0, 8)) resetOctree();
	if (ImGui::SliderFloat("Noise Frequency", &NoiseFrequency, 0.001, 2)) resetOctree();
	if (ImGui::SliderFloat("Noise Scale", &NoiseScale, 0.001, 2)) {ng.setScale(NoiseScale); resetOctree(); }
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

void Octree::clearChildren(OctreeNode* node) 
{
	if (!(node->leaf)) {
		for (size_t i = 0; i < 8; i++)
		{
			clearNode(&(node->children[i]));
		}
		node->leaf = true;
	}
}

void Octree::resetOctree()
{
	//TODO: fix this so it doesnt cause error
	uint32_t size = terrainGenerationThreads.size();
	for (size_t i = 0; i < size; i++)
	{
		std::pair<std::thread*, OctreeNode*> pair = terrainGenerationThreads.front(); terrainGenerationThreads.pop_front();
		std::thread* curThread = pair.first;
		curThread->join();
		delete curThread;
	}
	clearNode(&root);
	root = OctreeNode();
	root.depth = TreeDepth;
}
