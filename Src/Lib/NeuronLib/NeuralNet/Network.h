#pragma once

#include "Util/Serializable.h"
#include <vector>

class Random;

enum class NetworkRange
{
	ZeroToOne,
	NegativeOneToOne
};

constexpr NetworkRange k_networkRange = NetworkRange::ZeroToOne;

class MutationSettings
{
public:
	bool operator == (const MutationSettings& rhs) const
	{
		return (memcmp(this, &rhs, sizeof(*this)) == 0);
	}
public:
	float addLevel = 0.05f;			// Chance per network of adding a level
	float addNeuron = 0.1f;			// Chance per level of adding a neuron
	float deleteNeuron = 0.1f;		// Chance per level of deleting a neuron
	// TODO: Come up with a better algorithm for determining which and how many individual weights to change
	float modifyWeight = 0.1f;		// Percentage of weights to modify
	float modifyWeights = 0.01f;	// Chance per neuron of modifying all its weights
	float modifyBias = 0.05f;		// Chance per neuron of modifying bias
};

//=============================================================================

class Neuron : public ISerializable
{
public:
	Neuron() {}
	Neuron(const int numWeights)
	{
		weights.resize(numWeights);
	}

	// ISerializable interface
	virtual void Serialize(BinaryBuffer& stream) const override;
	virtual void Deserialize(BinaryBuffer& stream) override;

	// Primarily used for validating unit tests
	bool operator == (const Neuron& rhs) const
	{
		return
			(bias == rhs.bias) &&
			(weights == rhs.weights);
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

	void RandomizeWeights(Random& rand);
	void RandomizeSingleWeight(int weightIndex, Random& rand);

	void RandomizeBias(Random& rand);

	void RandomizeAll(Random& rand)
	{
		RandomizeWeights(rand);
		RandomizeBias(rand);
	}

public:
	std::vector<float> weights;
	float bias = 0.0f;
};

//=============================================================================

class NetworkLevel : public ISerializable
{
public:
	NetworkLevel() = default;
	NetworkLevel(const int numNeurons, const int numWeights)
	{
		neurons.resize(numNeurons, Neuron(numWeights));
	}

	// ISerializable interface
	virtual void Serialize(BinaryBuffer& stream) const override;
	virtual void Deserialize(BinaryBuffer& stream) override;

	// Primarily used for validating unit tests
	bool operator == (const NetworkLevel& rhs) const
	{
		return neurons == rhs.neurons;
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

	void AddNeuron(Random& rand)
	{
		neurons.emplace_back((int)neurons[0].weights.size());
		neurons[neurons.size() - 1].RandomizeAll(rand);
	}

	void Randomize(Random& rand)
	{
		for (auto& neuron : neurons)
		{
			neuron.RandomizeAll(rand);
		}
	}

public:
	std::vector<Neuron> neurons;
};

//=============================================================================

class Network : public ISerializable
{
public:
	Network() {}
	Network(const std::vector<int>& neuronsPerLevel)
	{
		m_levels.clear();
		m_numInputs = neuronsPerLevel[0];

		for (int i = 0; i < neuronsPerLevel.size(); i++)
		{
			const int numNeurons = neuronsPerLevel[i];
			// Number of weights for this level is the same as the number of neurons in the next level
			const int numWeights = (i > 0) ? neuronsPerLevel[i - 1] : 0;
			m_levels.emplace_back(numNeurons, numWeights);
		}
	}

	// ISerializable interface
	virtual void Serialize(BinaryBuffer& stream) const override;
	virtual void Deserialize(BinaryBuffer& stream) override;

	// Primarily used for validating unit tests
	bool operator == (const Network& rhs) const
	{
		return
			(m_levels == rhs.m_levels) &&
			(m_numInputs == rhs.m_numInputs) &&
			(m_mutationSettings == rhs.m_mutationSettings);
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

		for (int levelIndex = 1; levelIndex < m_levels.size(); levelIndex++)
		{
			const auto& level = m_levels[levelIndex];
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
		for (int i = 0; i < m_levels.size(); i++)
		{
			m_levels[i].Print();
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

	void Randomize(Random& rand)
	{
		for (auto& level : m_levels)
		{
			level.Randomize(rand);
		}
	}

	// Rebuild this network by merging the two provided parents
	void InitializeFromParents(const Network& parent0, const Network& parent1, Random& rand);

	void Mutate(Random& rand);

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

	void AddRandomLevel(Random& rand);

private:
	std::vector<NetworkLevel> m_levels;
	int m_numInputs;
	MutationSettings m_mutationSettings;
};
