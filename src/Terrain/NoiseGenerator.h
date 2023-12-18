#pragma once

#include<vector>
#include <FastNoise/FastNoise.h>
#include <FastNoise/Metadata.h>
#include <FastNoise/Generators/Modifiers.h>
#include <FastNoise/Generators/BasicGenerators.h>
#include <glm/glm.hpp>


class NoiseGenerator {
	FastNoise::SmartNode <FastNoise::DomainScale> fnGenerator;
	FastNoise::SmartNode <FastNoise::FractalFBm> fnFractal;
public:
	NoiseGenerator() {
		fnGenerator = FastNoise::New<FastNoise::DomainScale>();
		auto fnScale2 = FastNoise::New<FastNoise::DomainScale>();
		auto fnTerrain = FastNoise::NewFromEncodedNodeTree("GQAcAAEZAAUAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAwADNzEw+ARQAEwAAAIA/DQAFAAAAAAAAQAgAAAAAAD8AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");
		

		
		auto fnSimplex = FastNoise::New<FastNoise::Simplex>();
		fnFractal = FastNoise::New<FastNoise::FractalFBm>();
		fnFractal->SetSource(fnSimplex);
		fnFractal->SetOctaveCount(5);

		auto fnLevel = FastNoise::New<FastNoise::PositionOutput>();
		fnLevel->Set<FastNoise::Dim::Y>(1);

		auto fnSphere = FastNoise::New<FastNoise::DistanceToPoint>();
		fnSphere->SetDistanceFunction(FastNoise::DistanceFunction::Euclidean);
		auto fnAdd2 = FastNoise::New < FastNoise::Add>();
		fnAdd2->SetLHS(fnSphere);
		fnAdd2->SetRHS(-2.f);

		

		auto fnAdd = FastNoise::New<FastNoise::Add>();
		fnAdd->SetLHS(fnFractal);
		fnAdd->SetRHS(fnLevel);
		
		fnScale2->SetScale(0.01f);
		fnScale2->SetSource(fnAdd);
		fnGenerator->SetSource(fnScale2);
		
	}
	void setGain(float gain) { fnFractal->SetGain(gain); };
	void setLacunarity(float lacunarity) { fnFractal->SetLacunarity(lacunarity); };
	void setScale(float scale) { fnGenerator->SetScale(scale);  };
	void generateNoise(float* noiseOutput, int terrainSize, glm::vec3 noisePos = glm::vec3(0), float freq = 0.2, float scale = 1.f, int seed = rand());
	float getDensity(glm::vec3 pos);
};