#include "pch.h"
#include "Random.h"

#include <chrono>
#include <limits>

//std::default_random_engine Random::s_generator((int)std::chrono::system_clock::now().time_since_epoch().count());
std::normal_distribution<float> Random::s_gaussianDistribution(0.0f, 1.0f);	// (mean, standard deviation)
std::uniform_real_distribution<float> Random::s_uniformRealDistribution;

void Random::Seed()
{
	m_generator.seed((int)std::chrono::system_clock::now().time_since_epoch().count());
}

void Random::Seed(int seed)
{
	m_generator.seed(seed);
}
