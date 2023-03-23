#pragma once

#include<vector>
#include <FastNoise/FastNoise.h>
#include <FastNoise/Metadata.h>
#include <FastNoise/Generators/Modifiers.h>
#include <glm/glm.hpp>


class NoiseGenerator {
	FastNoise::SmartNode <FastNoise::DomainScale> fnScale;
	FastNoise::SmartNode <> fnGenerator;
public:
	NoiseGenerator() {
		
		auto fnTerrain = FastNoise::NewFromEncodedNodeTree("EQACAAAAAAAgQBAAAAAAQBkAEwDD9Sg/DQAEAAAAAAAgQAkAAGZmJj8AAAAAPwEEAAAAAAAAAEBAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAM3MTD4AMzMzPwAAAAA/");
		auto fnSimplex = FastNoise::New<FastNoise::Simplex>();
		auto fnFractal = FastNoise::New<FastNoise::FractalFBm>();
		fnFractal->SetSource(fnSimplex);
		fnFractal->SetOctaveCount(5);
		fnScale = FastNoise::New<FastNoise::DomainScale>();
		fnScale->SetSource(fnSimplex);
		fnScale->SetScale(0.1f);
		auto fnAdd = FastNoise::New<FastNoise::Add>();
		fnAdd->SetLHS(fnFractal);
		auto fnPositionOutput = FastNoise::New<FastNoise::PositionOutput>();
		fnPositionOutput->Set<FastNoise::Dim::Y>(3.f, 0);
		fnAdd->SetRHS(fnPositionOutput);
		

		fnScale->SetSource(fnAdd);
		fnGenerator = fnScale;
		
	}
	void generateNoise(float* noiseOutput, int terrainSize, glm::vec3 noisePos = glm::vec3(0), float freq = 0.2, float scale = 1.f, int seed = rand());
	void generateSphericalNoise(float* noiseOutput, int terrainSize, float sphereRadiusSquared, float sphereIntensity, glm::vec3 noisePos = glm::vec3(0), glm::vec3 sphereCenter = glm::vec3(0), int seed = rand());
	void generateSphere(float* noiseOutput, int terrainSize, float sphereRadiusSquared, float sphereIntensity, glm::vec3 noisePos = glm::vec3(0), glm::vec3 sphereCenter = glm::vec3(0));
};