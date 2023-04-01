#include<Terrain/NoiseGenerator.h>

void NoiseGenerator::generateNoise(float* noiseOutput, int terrainSize, glm::vec3 noisePos, float freq, float scale, int seed) {
	fnOffset->SetOffset<FastNoise::Dim::X>(noisePos.x);
	fnOffset->SetOffset<FastNoise::Dim::Y>(noisePos.y);
	fnOffset->SetOffset<FastNoise::Dim::Z>(noisePos.z);
	fnScale->SetScale(scale * this->scale);
	fnGenerator->GenUniformGrid3D(noiseOutput, lroundf(noisePos.x), lroundf(noisePos.y), lroundf(noisePos.z), terrainSize, terrainSize, terrainSize, freq, seed);

	
}
void NoiseGenerator::generateSphericalNoise(float* noiseOutput, int terrainSize, float sphereRadiusSquared, float sphereIntensity, glm::vec3 noisePos, glm::vec3 sphereCenter, int seed)
{
	generateNoise(noiseOutput, terrainSize, noisePos, 0.2f, 1.f, seed);
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
