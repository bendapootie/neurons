#include <stdio.h>
#include <vector>
#include "Util/Random.h"
#include "NeuralNet/Network.h"

void TrainForCircle(const int maxPopulation, const float topKeepPercent = 0.2f, const float randomKeepPercent = 0.1f)
{
	float bestScore = 0.0f;
	int iterations = 0;

	std::vector<Network> workingSet1;
	std::vector<Network> workingSet2;

	// 1. Create initial population
	std::vector<Network>* population = &workingSet1;
	std::vector<Network>* nextGeneration = &workingSet2;
	for (int i = 0; i < maxPopulation; i++)
	{
		population->emplace_back(Network({ 2, 1 }));
		(*population)[population->size() - 1].Randomize();
	}

	std::vector<std::pair<int, float>> indexToScore;
	while (bestScore < 80.0f)
	{
		// 2. Score each network
		indexToScore.clear();
		for (auto& entry : *population)
		{
			float computedScore = 0.0f;
			for (float y = 0.0f; y <= 1.0f; y += 0.125f)
			{
				for (float x = 0.0f; x <= 1.0f; x += 0.125f)
				{
					const float groundTruth = sqrt(((x - 0.5f) * (x - 0.5f) + (y - 0.5f) * (y - 0.5f))) / 1.7677f;
					//const float groundTruth = std::max(0.4f + x * 0.2f, 0.4f + y * 0.2f);
					auto result = entry.Evaluate({ x, y });
					computedScore += 1.0f - abs(groundTruth - result[0]);	// Exact match = 1.0
				}
			}
			indexToScore.emplace_back((int)indexToScore.size(), computedScore);
		}
		iterations++;

		// 3. Keep the top X% and another random Y%
		// Sort by score
		std::sort(indexToScore.begin(), indexToScore.end(),
			[](const auto& a, const auto& b)
			{
				return a.second > b.second;
			});
		if (indexToScore[0].second > bestScore)
		{
			bestScore = indexToScore[0].second;
			printf("Best score = %0.2f; Iterations = %d\n", bestScore, iterations);
			//(*population)[indexToScore[0].first].Print();
			(*population)[indexToScore[0].first].PrintResults();
		}
		int maxIndexToKeep = (int)(indexToScore.size() * topKeepPercent);
		int numRandomToKeep = (int)(indexToScore.size() * randomKeepPercent);
		while (numRandomToKeep > 0)
		{
			const int randomIndexToKeep = Random::NextInt(maxIndexToKeep, (int)indexToScore.size());
			indexToScore[maxIndexToKeep].swap(indexToScore[randomIndexToKeep]);
			numRandomToKeep--;
			maxIndexToKeep++;
		}
		indexToScore.resize(maxIndexToKeep);

		// 4. Make children to refill population
		nextGeneration->clear();
		for (int i = 0; i < population->size(); i++)
		{
			const int parentIndex = indexToScore[i % indexToScore.size()].first;
			nextGeneration->push_back((*population)[parentIndex]);

			// First copy of network is unchanged. All other descendants are mutated.
			if (i >= indexToScore.size())
			{
				(*nextGeneration)[nextGeneration->size() - 1].Mutate();
			}
		}
		std::swap(population, nextGeneration);

		// 5. Repeat...
	}
}

int oldmain(int argc, char *argv[])
{

//	TrainForCircle(100, 0.15f, 0.05f);
/*
	Network test({ 2, 1 });
	test.Randomize();

	auto result = test.Evaluate({0.2f, 0.9f});

	for (int i = 0; i < 20; i++)
	{
		printf("\n");
		test.Print();
		for (float y = 0.0f; y <= 1.0f; y += 0.125f)
		{
			for (float x = 0.0f; x <= 1.0f; x += 0.125f)
			{
				auto result = test.Evaluate({ x, y });
				for (auto v : result)
				{
					printf("%0.2f, ", v);
				}
			}
			printf("\n");
		}
		test.Mutate();
	}
	*/
	return 0;
}
