#include<Terrain/TerrainChunk.h>

void TerrainChunk::setMat(unsigned int texture) {
	mat.AmbientColor = glm::vec3(1);
	mat.DiffuseColor = glm::vec3(1);
	mat.SpecularColor = glm::vec3(1);
	mat.SpecularExponent = 3;
	mat.AmbientTexture = texture;
	mat.DiffuseTexture = texture;
	mat.SpecularTexture = texture;
	mat.AlphaTexture = texture;
}

void TerrainChunk::draw(ShaderProgram& shader, glm::mat4& model, bool setMat) {
	shader.use();
	shader.setMat4("model", model);
	shader.setVec3("AmbientColor", mat.AmbientColor);
	shader.setVec3("DiffuseColor", mat.DiffuseColor);
	shader.setVec3("SpecularColor", mat.SpecularColor);
	shader.setFloat("SpecularExponent", mat.SpecularExponent);
	shader.setFloat("OpticalDensity", mat.OpticalDensity);
	shader.setFloat("Dissolve", mat.Dissolve);
	shader.setFloat("Illumination", mat.Illumination);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mat.AmbientTexture);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, mat.DiffuseTexture);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, mat.SpecularTexture);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, mat.SpecularHightlightTexture);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, mat.AlphaTexture);
	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, mat.BumpTexture);

	meshRenderer.draw();
}
void TerrainChunk::drawInstanced(ShaderProgram& shader, glm::mat4& model, unsigned int count, bool setMat) {
	shader.use();
	shader.setMat4("model", model);
	shader.setVec3("AmbientColor", mat.AmbientColor);
	shader.setVec3("DiffuseColor", mat.DiffuseColor);
	shader.setVec3("SpecularColor", mat.SpecularColor);
	shader.setFloat("SpecularExponent", mat.SpecularExponent);
	shader.setFloat("OpticalDensity", mat.OpticalDensity);
	shader.setFloat("Dissolve", mat.Dissolve);
	shader.setFloat("Illumination", mat.Illumination);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mat.AmbientTexture);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, mat.DiffuseTexture);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, mat.SpecularTexture);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, mat.SpecularHightlightTexture);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, mat.AlphaTexture);
	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, mat.BumpTexture);

	meshRenderer.drawInstanced(count);
}

void TerrainChunk::generateChunk(NoiseGenerator ng, unsigned int size, float freq, float scale, int seed) {
	unsigned int noiseSize = size + 2;
	std::vector<float> noiseOutput(noiseSize * noiseSize * noiseSize);
	ng.generateNoise(noiseOutput.data(), noiseSize, pos / scale, freq, scale, seed);
	mesh = dc::generateMesh(noiseOutput, noiseSize);
	
}
void TerrainChunk::fillBuffers() {
	meshRenderer.clearBuffers();
	if (mesh.indices.size() > 0) 
		meshRenderer.fillBuffers(mesh.vertices, mesh.indices);
}

void TerrainChunk::clear()
{
	meshRenderer.clearBuffers();
}
