#include<Terrain/NoiseGenerator.h>

void NoiseGenerator::generateNoise(float* noiseOutput, int terrainSize, glm::vec3 noisePos, float freq, float scale, int seed) {
	fnGenerator->GenUniformGrid3D(noiseOutput, lroundf(noisePos.x), lroundf(noisePos.y), lroundf(noisePos.z), terrainSize, terrainSize, terrainSize, scale, seed);

	
}

float NoiseGenerator::getDensity(glm::vec3 pos)
{
	return fnGenerator->GenSingle3D(pos.x, pos.y, pos.z, 0);
}

