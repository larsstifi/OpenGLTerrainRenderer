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

}
void Octree::drawInstanced(ShaderProgram& shader, glm::mat4& model, unsigned int count, bool setMat) {

}
std::vector<OctreeNode*> Octree::getActiveNodes(glm::vec3 playerPos, float LOD_Falloff) {
	std::stack<OctreeNode*> stack; //keep track of Nodes and how many children we already iterated over
	std::stack<unsigned int> iterationStack; //keep track of Nodes and how many children we already iterated over
	std::stack<uint8_t> childrenIndexStack; //keep track of which children this node is in its parent 
	std::vector<OctreeNode*> activeNodes;
	stack.push(&root);
	iterationStack.push(0);
	childrenIndexStack.push(0);
	unsigned int curDepth = TreeDepth;
	glm::ivec3 curPos = glm::ivec3(0);
	while (!stack.empty()) {
		//get current node values
	    uint8_t curChildIndex = childrenIndexStack.top();
		OctreeNode* curNode = stack.top();
		unsigned int currentIteration = iterationStack.top();

		//calculate target LOD for this chunk
		glm::vec3 d = glm::vec3(curPos) - playerPos;
		unsigned int targetDepth = glm::dot(d, d) * LOD_Falloff;

		//check if last node or below target depth, if so add this node and move up in tree          only check if this is the first iteration
		bool isActive = (currentIteration == 0 && curDepth <= targetDepth) || curDepth == 0 ;
		if (isActive) {
			//add active node
			activeNodes.push_back(curNode);
			std::cout << "added node Depth: "<< curDepth <<" Position: " << curPos.x << " " << curPos.y << " " << curPos.z << std::endl;
		}
		if (isActive || currentIteration == 8) {
			//move up a level in LOD
			stack.pop();
			iterationStack.pop();
			childrenIndexStack.pop();
			curDepth++;
			curPos -= ChildrenPos[curChildIndex] * (2 << curDepth);
			continue;
		}
		
		if (currentIteration == 0 && curNode->leaf) { //check if leaf if so we spit the node
			OctreeNode children[8];
			curNode->leaf = false;
			curNode->children = children;
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
		curPos += ChildrenPos[currentIteration] * (2 << (curDepth-1));
	}
	return activeNodes;
}