#pragma once

#include <random>

class Random
{
public:
	void Seed();
	void Seed(int seed);

	int NextInt()
	{
		std::uniform_int_distribution<int> distribution(std::numeric_limits<int>::min(), std::numeric_limits<int>::max());
		return distribution(m_generator);
	}
	int NextInt(int maxExclusive)
	{
		std::uniform_int_distribution<int> distribution(0, maxExclusive - 1);
		return distribution(m_generator);
	}
	int NextInt(int minInclusive, int maxExclusive)
	{
		std::uniform_int_distribution<int> distribution(minInclusive, maxExclusive - 1);
		return distribution(m_generator);
	}
	float NextFloat()
	{
		return s_uniformRealDistribution(m_generator);
	}
	float NextFloat(float minInclusive, float maxExclusive)
	{
		return minInclusive + (maxExclusive * s_uniformRealDistribution(m_generator));
	}
	float NextGaussian()
	{
		return s_gaussianDistribution(m_generator);
	}
private:
	std::default_random_engine m_generator;
	static std::normal_distribution<float> s_gaussianDistribution;
	static std::uniform_real_distribution<float> s_uniformRealDistribution;
};

// Globally shared random generator for convenience
extern Random s_rand;