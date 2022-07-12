#pragma once

#include <random>

class Random
{
public:
	static int NextInt()
	{
		std::uniform_int_distribution<int> distribution(std::numeric_limits<int>::min(), std::numeric_limits<int>::max());
		return distribution(s_generator);
	}
	static int NextInt(int minInclusive, int maxExclusive)
	{
		std::uniform_int_distribution<int> distribution(minInclusive, maxExclusive - 1);
		return distribution(s_generator);
	}
	static float NextFloat()
	{
		return s_uniformRealDistribution(s_generator);
	}
	static float NextFloat(float minInclusive, float maxExclusive)
	{
		return minInclusive + (maxExclusive * s_uniformRealDistribution(s_generator));
	}
	static float NextGaussian()
	{
		return s_gaussianDistribution(s_generator);
	}
private:
	static std::default_random_engine s_generator;
	static std::normal_distribution<float> s_gaussianDistribution;
	static std::uniform_real_distribution<float> s_uniformRealDistribution;
};
