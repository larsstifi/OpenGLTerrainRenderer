#pragma once

#include<vector>
#include <FastNoise/FastNoise.h>
#include <FastNoise/Metadata.h>
#include <FastNoise/Generators/Modifiers.h>
#include <FastNoise/Generators/BasicGenerators.h>
#include <glm/glm.hpp>


class NoiseGenerator {
	FastNoise::SmartNode <FastNoise::DomainOffset> fnOffset;
	FastNoise::SmartNode <FastNoise::DomainScale> fnScale;
	FastNoise::SmartNode <> fnGenerator;
public:
	NoiseGenerator() {
		
		fnOffset = FastNoise::New<FastNoise::DomainOffset>();
		fnScale = FastNoise::New<FastNoise::DomainScale>();
		auto fnScale2 = FastNoise::New<FastNoise::DomainScale>();
		auto fnTerrain = FastNoise::NewFromEncodedNodeTree("GQAcAAEZAAUAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAwADNzEw+ARQAEwAAAIA/DQAFAAAAAAAAQAgAAAAAAD8AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");
		

		
		auto fnSimplex = FastNoise::New<FastNoise::Simplex>();
		auto fnFractal = FastNoise::New<FastNoise::FractalFBm>();
		fnFractal->SetSource(fnSimplex);
		fnFractal->SetOctaveCount(5);
		auto fnAdd = FastNoise::New<FastNoise::Add>();
		fnAdd->SetLHS(fnFractal);
		auto fnPositionOutput = FastNoise::New<FastNoise::DistanceToPoint>();
		fnPositionOutput->SetDistanceFunction(FastNoise::DistanceFunction::Euclidean);
		auto fnAdd2 = FastNoise::New < FastNoise::Add>();
		fnAdd2->SetLHS(fnPositionOutput);
		fnAdd2->SetRHS(-2.f);
		fnAdd->SetRHS(fnAdd2);
		
		fnScale2->SetScale(1.f);
		fnScale2->SetSource(fnFractal);
		fnOffset->SetSource(fnScale2);
		fnScale->SetSource(fnScale2);
		fnGenerator = fnScale;
		
	}
	void generateNoise(float* noiseOutput, int terrainSize, glm::vec3 noisePos = glm::vec3(0), float freq = 0.2, float scale = 1.f, int seed = rand());
	void generateSphericalNoise(float* noiseOutput, int terrainSize, float sphereRadiusSquared, float sphereIntensity, glm::vec3 noisePos = glm::vec3(0), glm::vec3 sphereCenter = glm::vec3(0), int seed = rand());
	void generateSphere(float* noiseOutput, int terrainSize, float sphereRadiusSquared, float sphereIntensity, glm::vec3 noisePos = glm::vec3(0), glm::vec3 sphereCenter = glm::vec3(0));
};