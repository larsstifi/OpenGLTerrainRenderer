#include<Terrain/NoiseGenerator.h>

void NoiseGenerator::generateNoise(FastNoise::SmartNode<> fnGenerator, float* noiseOutput, int terrainSize, glm::vec3 noisePos, int seed) {
	fnGenerator->GenUniformGrid3D(noiseOutput, noisePos.x, noisePos.y, noisePos.z, noisePos.x + terrainSize, noisePos.x + terrainSize, noisePos.x + terrainSize, 0.01f, seed);
}
void NoiseGenerator::generateSphericalNoise(FastNoise::SmartNode<> fnGenerator, float* noiseOutput, int terrainSize, float sphereRadiusSquared, float sphereIntensity, glm::vec3 noisePos, glm::vec3 sphereCenter, int seed)
{
	generateNoise(fnGenerator, noiseOutput, terrainSize, noisePos, seed);
	generateSphere(noiseOutput, terrainSize, sphereRadiusSquared,sphereIntensity, noisePos, sphereCenter);
}
void NoiseGenerator::generateSphere(float* output, int terrainSize, float sphereRadiusSquared, float sphereIntensity, glm::vec3 noisePos, glm::vec3 sphereCenter) {
	int index = 0;
	for (size_t i = 0; i < terrainSize; i++)
	{
		for (size_t j = 0; j < terrainSize; j++)
		{
			for (size_t k = 0; k < terrainSize; k++)
			{
				glm::vec3 distance = sphereCenter - noisePos - glm::vec3(i,j,k);
				float distanceToSphere = glm::dot(distance, distance);
				output[index] -= (sphereRadiusSquared - distanceToSphere) * sphereIntensity;

				index++;
			}
		}
	}
}
