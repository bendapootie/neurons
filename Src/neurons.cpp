#include <stdio.h>
#include <vector>
#include <random>
#include <chrono>
#include <limits>

enum class NetworkRange
{
	ZeroToOne,
	NegativeOneToOne
};

constexpr NetworkRange k_networkRange = NetworkRange::ZeroToOne;

class MutationSettings
{
public:
	float addLevel = 0.05f;			// Chance per network of adding a level
	float addNeuron = 0.1f;			// Chance per level of adding a neuron
	float deleteNeuron = 0.1f;		// Chance per level of deleting a neuron
	float modifyWeights = 0.05f;	// Chance per neuron of modifying all its weights
	float modifyBias = 0.05f;		// Chance per neuron of modifying bias
};

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
	static float NextGaussian()
	{
		return s_gaussianDistribution(s_generator);
	}
private:
	static std::default_random_engine s_generator;
	static std::normal_distribution<float> s_gaussianDistribution;
	static std::uniform_real_distribution<float> s_uniformRealDistribution;
};

std::default_random_engine Random::s_generator((int)std::chrono::system_clock::now().time_since_epoch().count());
std::normal_distribution<float> Random::s_gaussianDistribution(0.0f, 1.0f);	// (mean, standard deviation)
std::uniform_real_distribution<float> Random::s_uniformRealDistribution;

class Neuron
{
public:
	Neuron() {}
	Neuron(const int numWeights)
	{
		weights.resize(numWeights);
	}

	void Print() const
	{
		printf("(");
		for (int i = 0; i < weights.size(); i++)
		{
			printf("%s%0.2f", ((i == 0) ? "" : ","), weights[i]);
		}
		printf(")");
		if (!weights.empty())
		{
			printf("%s%0.2f", ((bias < 0.0f) ? "" : "+"), bias);
		}
	}

	void RandomizeWeights()
	{
		for (auto& weight : weights)
		{
			weight = Random::NextGaussian();
		}
	}

	void RandomizeBias()
	{
		bias = Random::NextGaussian();
	}

	void RandomizeAll()
	{
		RandomizeWeights();
		RandomizeBias();
	}

public:
	std::vector<float> weights;
	float bias = 0.0f;
};

class NetworkLevel
{
public:
	NetworkLevel(const int numNeurons, const int numWeights)
	{
		neurons.resize(numNeurons, Neuron(numWeights));
	}

	void Print() const
	{
		for (const auto& n : neurons)
		{
			printf("[");
			n.Print();
			printf("]");
		}
	}

	void AddNeuron()
	{
		neurons.emplace_back((int)neurons[0].weights.size());
		neurons[neurons.size() - 1].RandomizeAll();
	}

	void Randomize()
	{
		for (auto &neuron : neurons)
		{
			neuron.RandomizeAll();
		}
	}

public:
	std::vector<Neuron> neurons;
};

class Network
{
public:
	Network() {}
	Network(const std::vector<int>& neuronsPerLevel)
	{
		levels.clear();
		numInputs = neuronsPerLevel[0];

		int numWeights = 0;
		for (int i = 0; i < neuronsPerLevel.size(); i++)
		{
			const int numNeurons = neuronsPerLevel[i];
			levels.emplace_back(numNeurons, numWeights);
			// Number of weights for the next level is same as the number of neurons in this level
			numWeights = numNeurons;
		}
	}

	std::vector<float> Evaluate(const std::vector<float>& inputs) const
	{
		// assert inputs.size() == numInputs
		static std::vector<float> scratch1;
		static std::vector<float> scratch2;
		std::vector<float>* l1 = &scratch1;
		std::vector<float>* l2 = &scratch2;

		scratch1 = inputs;
		scratch2.clear();
		
		for (int levelIndex = 1; levelIndex < levels.size(); levelIndex++)
		{
			const auto& level = levels[levelIndex];
			// ensure output is big enough
			l2->resize(level.neurons.size());
			for (int i = 0; i < level.neurons.size(); i++)
			{
				float val = level.neurons[i].bias;
				// assert l1-size() == weights.size()
				for (int j = 0; j < l1->size(); j++)
				{
					val += (*l1)[j] * level.neurons[i].weights[j];
				}
				(*l2)[i] = Activation(val);
			}
			// swap buffers
			std::vector<float>* temp = l1;
			l1 = l2;
			l2 = temp;
		}
		
		return *l1;
	}
	
	void Print()
	{
		for (int i = 0; i < levels.size(); i++)
		{
			levels[i].Print();
			printf("\n");
		}
	}

	// Note: Only works correctly for networks with two inputs
	void PrintResults()
	{
		for (float y = 0.0f; y <= 1.0f; y += 0.125f)
		{
			for (float x = 0.0f; x <= 1.0f; x += 0.125f)
			{
				auto result = Evaluate({ x, y });
				for (auto v : result)
				{
					printf("%0.2f, ", v);
				}
			}
			printf("\n");
		}
	}

	void Randomize()
	{
		for (auto& level : levels)
		{
			level.Randomize();
		}
	}

	void Mutate()
	{
		// Chance per network of adding a level
		if (Random::NextFloat() < mutationSettings.addLevel)
		{
			AddRandomLevel();
		}

		// Chance per level of adding a neuron
		// Don't add neurons to input or output layers
		for (int i = 1; i < (int)levels.size() - 1; i++)
		{
			if (Random::NextFloat() < mutationSettings.addNeuron)
			{
				// Add a new neuron to the target level
				auto& level = levels[i];
				level.AddNeuron();
				// Give each neuron on the next level an additional weight to account for the new neuron
				for (auto& neuron : levels[i + 1].neurons)
				{
					neuron.weights.push_back(Random::NextGaussian());
				}
			}
			if (Random::NextFloat() < mutationSettings.deleteNeuron)
			{
				// Remove a neuron from the target level
				auto& level = levels[i];
				if (level.neurons.size() == 1)
				{
					// Remove the last neuron from a level by removing the entire level
					levels.erase(levels.begin() + i);
					//Fix up the weights for the next level
					const int targetNumWeights = (int)levels[i - 1].neurons.size();
					for (auto& neuron : levels[i].neurons)
					{
						neuron.weights.resize(targetNumWeights);
						neuron.RandomizeAll();
					}
					i--;
				}
				else
				{
					const int neuronIndexToDelete = Random::NextInt(0, (int)level.neurons.size());
					level.neurons.erase(level.neurons.begin() + neuronIndexToDelete);
					// Remove the appropriate weight from each neuron in the next level
					for (auto& neuron : levels[i + 1].neurons)
					{
						neuron.weights.erase(neuron.weights.begin() + neuronIndexToDelete);
					}
				}
			}
		}

		//deleteNeuron = 0.1f;		// Chance per level of deleting a neuron

		// Chance per neuron of modifying all its weights
		// Chance per neuron of modifying bias
		for (int i = 1; i < levels.size(); i++)
		{
			for (auto& neuron : levels[i].neurons)
			{
				if (Random::NextFloat() < mutationSettings.modifyWeights)
				{
					neuron.RandomizeWeights();
				}
				if (Random::NextFloat() < mutationSettings.modifyBias)
				{
					neuron.RandomizeBias();
				}
			}
		}
	}
	
	static float Sigmoid(const float x)
	{
		return 1.0f / (1.0f + expf(-x));
	}
	static float SigmoidDx(const float x)
	{
		return Sigmoid(x) * (1.0f - Sigmoid(x));
	}
	static float SigmoidInverse(const float x)
	{
		// Ths inverse of the Sigmoid function is known as the "Logit" function
		return logf(x) - logf(1.0f - x);
	}

private:
	static float Activation(const float x)
	{
		// TODO: Is there a better activation function is use?
		//       Maybe the activation function could be a parameter of the network
		//       and susceptible to mutation?
		//return Sigmoid(x);
		return tanhf(x);
	}

	void AddRandomLevel()
	{
		int newIndex = Random::NextInt(1, (int)levels.size());

		const int neuronsinPreviousLevel = (int)levels[newIndex - 1].neurons.size();
		NetworkLevel newLevel(neuronsinPreviousLevel, neuronsinPreviousLevel);

		constexpr bool completelyRandomizeNewLevel = true;
		if constexpr (completelyRandomizeNewLevel)
		{
			newLevel.Randomize();
		}
		else
		{
			for (int i = 0; i < newLevel.neurons.size(); i++)
			{
				newLevel.neurons[i].weights[i] = 1.0f;
			}
		}

		levels.insert(levels.begin() + newIndex, newLevel);
	}
	
private:
	std::vector<NetworkLevel> levels;
	int numInputs;
	MutationSettings mutationSettings;
};

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

int main(int argc, char *argv[])
{
	TrainForCircle(100, 0.15f, 0.05f);
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
}
