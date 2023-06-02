#include<Terrain/TerrainChunk.h>


void TerrainChunk::draw(ShaderProgram& shader, glm::mat4& model, bool setMat) {
	shader.use();
	shader.setMat4("model", model);

	meshRenderer.draw();
}
void TerrainChunk::drawInstanced(ShaderProgram& shader, glm::mat4& model, unsigned int count, bool setMat) {
	shader.use();
	shader.setMat4("model", model);

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
