#pragma once

#include<vector>
#include <FastNoise/FastNoise.h>
#include <FastNoise/Metadata.h>
#include <FastNoise/Generators/Modifiers.h>
#include <glm/glm.hpp>


class NoiseGenerator {
	FastNoise::SmartNode <> fnGenerator;
public:
	NoiseGenerator() {
		//fnGenerator = FastNoise::NewFromEncodedNodeTree("EQACAAAAAAAgQBAAAAAAQBkAEwDD9Sg/DQAEAAAAAAAgQAkAAGZmJj8AAAAAPwEEAAAAAAAAAEBAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAM3MTD4AMzMzPwAAAAA/");
		
		auto fnSimplex = FastNoise::New<FastNoise::Simplex>();
		auto fnFractal = FastNoise::New<FastNoise::FractalFBm>();
		fnFractal->SetSource(fnSimplex);
		fnFractal->SetOctaveCount(5);
		auto fnAdd = FastNoise::New<FastNoise::Add>();
		fnAdd->SetLHS(fnFractal);
		auto fnPointDistance = FastNoise::New<FastNoise::DistanceToPoint>();
		fnAdd->SetRHS(fnPointDistance);
		fnGenerator = fnFractal;
	}
	void generateNoise(float* noiseOutput, int terrainSize, glm::vec3 noisePos = glm::vec3(0), int seed = rand());
	void generateSphericalNoise(float* noiseOutput, int terrainSize, float sphereRadiusSquared, float sphereIntensity, glm::vec3 noisePos = glm::vec3(0), glm::vec3 sphereCenter = glm::vec3(0), int seed = rand());
	void generateSphere(float* noiseOutput, int terrainSize, float sphereRadiusSquared, float sphereIntensity, glm::vec3 noisePos = glm::vec3(0), glm::vec3 sphereCenter = glm::vec3(0));
};