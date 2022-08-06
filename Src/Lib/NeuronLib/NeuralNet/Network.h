#pragma once

#include <vector>

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

	void RandomizeWeights();

	void RandomizeBias();

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
		for (auto& neuron : neurons)
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
		// Static vectors are a nice optimization for single-threaded, but won't work multi-threaded
		// TODO: Make these thread local if this optimization is needed for multi-threading
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

	void Mutate();

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
		// The inverse of the Sigmoid function is known as the "Logit" function
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

	void AddRandomLevel();

private:
	std::vector<NetworkLevel> levels;
	int numInputs;
	MutationSettings mutationSettings;
};
