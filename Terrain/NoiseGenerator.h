#pragma once

#include<vector>
#include <FastNoise/FastNoise.h>
#include <glm/glm.hpp>


class NoiseGenerator {
		FastNoise::SmartNode<> fnGenerator;
public:
	void generateNoise(FastNoise::SmartNode<> fnGenerator, float* noiseOutput, int terrainSize, glm::vec3 noisePos = glm::vec3(0), int seed = rand());
	void generateSphericalNoise(FastNoise::SmartNode<> fnGenerator, float* noiseOutput, int terrainSize, float sphereRadiusSquared, float sphereIntensity, glm::vec3 noisePos = glm::vec3(0), glm::vec3 sphereCenter = glm::vec3(0), int seed = rand());
	void generateSphere(float* noiseOutput, int terrainSize, float sphereRadiusSquared, float sphereIntensity, glm::vec3 noisePos = glm::vec3(0), glm::vec3 sphereCenter = glm::vec3(0));
};