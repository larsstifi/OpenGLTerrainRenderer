#include<Terrain/TerrainChunk.h>


void TerrainChunk::draw(unsigned int spID, glm::mat4& model) {
	ShaderProgram::setMat4(spID, "model", model);

	meshRenderer.draw();
}
void TerrainChunk::drawInstanced(unsigned int spID, glm::mat4& model, unsigned int count) {
	ShaderProgram::setMat4(spID, "model", model);

	meshRenderer.drawInstanced(count);
}

void TerrainChunk::generateChunk(NoiseGenerator ng, unsigned int size, float freq, float scale, int seed) {
	unsigned int noiseSize = size + 2;
	std::vector<float> noiseOutput(noiseSize * noiseSize * noiseSize);
	ng.generateNoise(noiseOutput.data(), noiseSize, pos / scale, freq, scale, seed);
	mesh = dc::generateMesh(noiseOutput, noiseSize);
	
}
void TerrainChunk::fillBuffers() {
	if (mesh.indices.size() > 0)
		meshRenderer.fillBuffers(mesh.vertices, mesh.indices);
}

void TerrainChunk::clear()
{
	meshRenderer.clearBuffers();
}
