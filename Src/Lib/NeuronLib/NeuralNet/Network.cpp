#include "pch.h"
#include "Network.h"

#include "Util/Math.h"
#include "Util/Random.h"

//=============================================================================
// Neuron
//
// ISerializable interface
void Neuron::Serialize(BinaryBuffer& stream) const
{
	SerializeInt(stream, static_cast<int>(weights.size()));
	for (const float& weight : weights)
	{
		SerializeFloat(stream, weight);
	}
	SerializeFloat(stream, bias);

	static_assert(sizeof(m_activationFunction) == sizeof(int));
	SerializeInt(stream, static_cast<int>(m_activationFunction));
}

void Neuron::Deserialize(BinaryBuffer& stream)
{
	int size;
	DeserializeInt(stream, size);
	weights.resize(size);
	for (float& weight : weights)
	{
		DeserializeFloat(stream, weight);
	}
	DeserializeFloat(stream, bias);

	static_assert(sizeof(m_activationFunction) == sizeof(int));
	int tempInt;
	DeserializeInt(stream, tempInt);
	m_activationFunction = static_cast<ActivationFunction>(tempInt);
}

void Neuron::RandomizeWeights(Random& rand)
{
	for (auto& weight : weights)
	{
		weight = rand.NextGaussian();
	}

	// Reset the activation function in case it was Identity
	m_activationFunction = ActivationFunction::Default;
}

void Neuron::RandomizeSingleWeight(int weightIndex, Random& rand)
{
	weights[weightIndex] = rand.NextGaussian();

	// Reset the activation function in case it was Identity
	m_activationFunction = ActivationFunction::Default;
}

void Neuron::RandomizeBias(Random& rand)
{
	bias = rand.NextGaussian();

	// Reset the activation function in case it was Identity
	m_activationFunction = ActivationFunction::Default;
}

//=============================================================================
// NetworkLevel
//
// ISerializable interface
void NetworkLevel::Serialize(BinaryBuffer& stream) const
{
	SerializeInt(stream, static_cast<int>(neurons.size()));
	for (const auto& neuron : neurons)
	{
		neuron.Serialize(stream);
	}
}

void NetworkLevel::Deserialize(BinaryBuffer& stream)
{
	int size;
	DeserializeInt(stream, size);
	neurons.resize(size);
	for (auto& neuron : neurons)
	{
		neuron.Deserialize(stream);
	}
}

void NetworkLevel::Print() const
{
	for (const auto& n : neurons)
	{
		printf("[");
		n.Print();
		printf("]");
	}
}

void NetworkLevel::MakeIdentity()
{
	for (size_t n = 0; n < neurons.size(); n++)
	{
		Neuron& neuron = neurons[n];
		for (size_t w = 0; w < neuron.weights.size(); w++)
		{
			neuron.weights[w] = (n == w) ? 1.0f : 0.0f;
		}
		neuron.bias = 0.0f;
		neuron.m_activationFunction = ActivationFunction::Identity;
	}
}

void NetworkLevel::Randomize(Random& rand)
{
	for (auto& neuron : neurons)
	{
		neuron.RandomizeAll(rand);
	}
}


//=============================================================================

void Network::Serialize(BinaryBuffer& stream) const
{
	{
		SerializeInt(stream, static_cast<int>(m_levels.size()));
		for (const auto& level : m_levels)
		{
			level.Serialize(stream);
		}
	}
	SerializeInt(stream, m_numInputs);
	SerializeSimpleObject(stream, m_mutationSettings);
}

void Network::Deserialize(BinaryBuffer& stream)
{
	{
		int size;
		DeserializeInt(stream, size);
		m_levels.resize(size);
		for (auto& level : m_levels)
		{
			level.Deserialize(stream);
		}
	}
	DeserializeInt(stream, m_numInputs);
	DeserializeSimpleObject(stream, m_mutationSettings);
}


void Network::InitializeFromParents(Random& rand, const Network* parent0, const Network* parent1)
{
	if (parent0 == nullptr && parent1 == nullptr)
	{
		// Both parents are null, so randomize the network
		Randomize(rand);
	}
	else if (parent0 != nullptr && parent1 != nullptr)
	{
		// Both parents are valid; Merge them together
		// TODO: Figure out a better way to merge neural networks that maintains functionality
		*this = *parent0;

		const int maxLevel = Math::Min(int(m_levels.size()), int(parent1->m_levels.size()));
		for (int i = 0; i < maxLevel; i++)
		{
			NetworkLevel& level = m_levels[i];
			const NetworkLevel& p1Level = parent1->m_levels[i];

			const int targetNumWeights = (i > 0) ? static_cast<int>(m_levels[i - 1].neurons.size()) : 0;
			const int maxNeuron = Math::Min(
				static_cast<int>(level.neurons.size()),
				static_cast<int>(p1Level.neurons.size())
			);
			for (int n = 0; n < maxNeuron; n++)
			{
				// 50/50 chance of copying neuron from parent1
				// Note: If networks aren't the exact same shape, there's a chance the new neuron weights my be out of bounds
				if (rand.NextInt(0, 2) == 0)
				{
					level.neurons[n] = p1Level.neurons[n];
					const int originalNumWeights = static_cast<int>(level.neurons[n].weights.size());
					if (originalNumWeights != targetNumWeights)
					{
						level.neurons[n].weights.resize(targetNumWeights);
						for (int w = originalNumWeights; w < targetNumWeights; w++)
						{
							level.neurons[n].RandomizeSingleWeight(w, rand);
						}
					}
				}
			}
		}
	}
	else
	{
		// There is only one parent. Copy it.
		*this = (parent0 != nullptr) ? *parent0 : *parent1;
	}

	// Mutations always happen regardless of how many parents are used
	Mutate(rand);
}

void Network::Mutate(Random& rand)
{
	// Chance per network of adding a level
	if (rand.NextFloat() < m_mutationSettings.addLevel)
	{
		// Since the new level is an Identity, it doesn't change the behavior of the network
		int newIndex = rand.NextInt(1, (int)m_levels.size());
		AddIdentityLevel(newIndex);
	}

	// Chance per level of adding a neuron
	// Don't add neurons to input or output layers
	for (int i = 1; i < (int)m_levels.size() - 1; i++)
	{
		if (rand.NextFloat() < m_mutationSettings.addNeuron)
		{
			// Add a new neuron to the target level
			auto& level = m_levels[i];
			level.AddNeuron(rand);
			// Give each neuron on the next level an additional weight to account for the new neuron
			for (auto& neuron : m_levels[i + 1].neurons)
			{
				neuron.weights.push_back(rand.NextGaussian());
			}
		}
		if (rand.NextFloat() < m_mutationSettings.deleteNeuron)
		{
			// Remove a neuron from the target level
			auto& level = m_levels[i];
			if (level.neurons.size() == 1)
			{
				// Remove the last neuron from a level by removing the entire level
				m_levels.erase(m_levels.begin() + i);
				//Fix up the weights for the next level
				const int targetNumWeights = (int)m_levels[i - 1].neurons.size();
				for (auto& neuron : m_levels[i].neurons)
				{
					neuron.weights.resize(targetNumWeights);
					neuron.RandomizeAll(rand);
				}
				i--;
			}
			else
			{
				const int neuronIndexToDelete = rand.NextInt(0, (int)level.neurons.size());
				level.neurons.erase(level.neurons.begin() + neuronIndexToDelete);
				// Remove the appropriate weight from each neuron in the next level
				for (auto& neuron : m_levels[i + 1].neurons)
				{
					neuron.weights.erase(neuron.weights.begin() + neuronIndexToDelete);
				}
			}
		}
	}

	//deleteNeuron = 0.1f;		// Chance per level of deleting a neuron

	// Chance per neuron of modifying all its weights
	// Chance per neuron of modifying bias
	for (int i = 1; i < m_levels.size(); i++)
	{
		for (auto& neuron : m_levels[i].neurons)
		{
			if (rand.NextFloat() < m_mutationSettings.modifyWeights)
			{
				neuron.RandomizeWeights(rand);
			}
			else
			{
				// TODO: This way of determining how many and which weights to change isn't
				//       statistically the same as checking each one.
				int numWeightsToChange = static_cast<int>(neuron.weights.size() * m_mutationSettings.modifyWeight);
				for (int w = 0; w < numWeightsToChange; w++)
				{
					int weightIndex = rand.NextInt(static_cast<int>(neuron.weights.size()));
					neuron.RandomizeSingleWeight(weightIndex, rand);
				}
			}

			if (rand.NextFloat() < m_mutationSettings.modifyBias)
			{
				neuron.RandomizeBias(rand);
			}
		}
	}
}

void Network::AddIdentityLevel(int levelIndex)
{
	if ((levelIndex <= 0) || (levelIndex >= static_cast<int>(m_levels.size())))
	{
		// TODO: Handle failue here better. Return false?
		_ASSERT(false);	// Index out of range!
		return;
	}

	const int neuronsinPreviousLevel = static_cast<int>(m_levels[levelIndex - 1].neurons.size());
	NetworkLevel newLevel(neuronsinPreviousLevel, neuronsinPreviousLevel);

	newLevel.MakeIdentity();

	m_levels.insert(m_levels.begin() + levelIndex, newLevel);
}
