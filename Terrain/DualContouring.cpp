#include "DualContouring.h"

using namespace dc;

Mesh dc::generateMesh(std::vector<float>& density, unsigned int gridSize)
{
	Mesh m;
	std::unordered_map<glm::uvec3, unsigned int> cellVertexIndexMap;
	std::vector<Vertex> vertices;
	std::vector<unsigned int> vertrexIndices;
	int index = -1;
	for (unsigned int i = 0; i < gridSize-1; i++)
	{
		for (unsigned int j = 0; j < gridSize-1; j++)
		{
			for (unsigned int k = 0; k < gridSize-1; k++)
			{
				index++;
				glm::uvec3 curCellPos = glm::uvec3(i, j, k);

				//gather edges with sign change
				std::vector<int> signedEdges;
				for (int x = 0; x < 12; x++) {
					glm::uvec3 cornerPosX = cornerPos[edges[x][0]];
					glm::uvec3 cornerPosY = cornerPos[edges[x][1]];
					float densityX = density[i + cornerPosX.x + (j + cornerPosX.y) * gridSize + (k + cornerPosX.z) * gridSize * gridSize];
					float densityY = density[i + cornerPosY.x + (j + cornerPosY.y) * gridSize + (k + cornerPosY.z) * gridSize * gridSize];
					int sign = ((int)signbit(densityX)) - ((int)signbit(densityY)); // if x is neg and y pos => 1, x pos and y neg -> -1
					if (sign == 0) continue;//no sign change on this edge
					//add edge index to list, if the index in edge list is positive then the first corner defined in the edges array has negative density, if negative entry then the second corner defined in the edges array has negative density 
					//the sign of the index defines the orientation of a surface
					signedEdges.push_back(sign * (x+1)); // we shift the index by +1 so no 0 index exist, if there was as 0 index the sign bit idea would not work 
				}

				if (signedEdges.size() == 0) {
					continue; // if there are no signed edges there is no surface crossing the current cell => no vertex needed and no surfaces
				}

				//create vertex
				glm::vec3 vertPos = glm::vec3(i + 0.5f, j + 0.5f, k + 0.5f); // place vertex in the middle of the cell

				//calculate vertex position by taking average of signed edges middle points
				vertPos = glm::vec3(0.f);
				for (int x = 0; x < signedEdges.size(); x++)
				{
					bool flipped = signedEdges[x] < 0;
					int edgeIndex = abs(signedEdges[x])-1;
					glm::vec3 startpos = cornerPos[edges[edgeIndex][0]];
					glm::vec3 dir = glm::vec3(cornerPos[edges[edgeIndex][1]]) - startpos;
					vertPos += startpos + dir * 0.5f ;
				}
				vertPos /= signedEdges.size();
				vertPos += +glm::vec3(curCellPos);

				glm::vec3 norm(0.f); //calculate the norm by sampling density at corners
				for (int x = 0; x < 8; x++) {
					glm::uvec3 cPos = cornerPos[x];
					norm += (glm::vec3(cPos) - glm::vec3(0.5f, 0.5f, 0.5f)) * density[i + cPos.x + (j + cPos.y) * gridSize + (k + cPos.z) * gridSize * gridSize];

				}
				//save vertex index in map
				unsigned int curVertIndex = vertices.size();
				cellVertexIndexMap.insert(std::pair<glm::uvec3, unsigned int>(curCellPos, curVertIndex)); //add vertex to our map so neighbouring cells can lookup the vertex index
				//add vertex to our list
				Vertex newVertex;
				newVertex.pos = vertPos;
				newVertex.norm = glm::normalize(norm);
				newVertex.uv = glm::vec2(i,j);
				vertices.push_back(newVertex);


				//Create Surfaces
				//we iterate over the signed edges and create a surface with neighbouring vertices
				//we only create surfaces for signed edges that contain corner 0 (edges with index <= 3), other surfaces contained in this cell are created by neighbouring cells
				for (int x = 0; x < std::min(3, (int)signedEdges.size()); x++) { // we only need to check the first tree entries in the signed edges list because there is a max of 3 edges containing 0 and edges are ordered
					//get current edge data
					bool flipped = signedEdges[x] < 0;
					int edgeIndex = abs(signedEdges[x]);
					if (edgeIndex > 3) break; //check if corner 0 is contained in edge, we can break instead of continuing because indices are ordered

					//check current edge is on border of array, if so dont generate surface

					if (curCellPos.x == 0 && edgeIndex != 1) continue;
					if (curCellPos.y == 0 && edgeIndex != 2) continue;
					if (curCellPos.z == 0 && edgeIndex != 3) continue;
					//get the positions of the neighbouring cells relative to current cell
					glm::vec3 leftCellPos(0);
					leftCellPos[edgeIndex % 3] = -1;
					glm::vec3 rightCellPos(0);
					rightCellPos[(edgeIndex+1) % 3] = -1;
					glm::vec3 backCellPos = leftCellPos + rightCellPos;
					if (flipped) {
						glm::vec3 temp = rightCellPos;
						rightCellPos = leftCellPos;
						leftCellPos = temp;
					}
					//adjust neighbouring cell positions to local space
					leftCellPos += curCellPos;
					rightCellPos += curCellPos;
					backCellPos += curCellPos;

					//get indices of neighbours vertices
					unsigned int leftVertexIndex = cellVertexIndexMap.at(leftCellPos);
					unsigned int rightVertexIndex = cellVertexIndexMap.at(rightCellPos);
					unsigned int backVertexIndex = cellVertexIndexMap.at(backCellPos);
					
					//create triangles
					vertrexIndices.push_back(leftVertexIndex);
					vertrexIndices.push_back(rightVertexIndex);
					vertrexIndices.push_back(curVertIndex);

					vertrexIndices.push_back(leftVertexIndex);
					vertrexIndices.push_back(backVertexIndex);
					vertrexIndices.push_back(rightVertexIndex);
				}

			}
		}
	}
	m.vertices = vertices;
	m.indices = vertrexIndices;
	return m;
}
