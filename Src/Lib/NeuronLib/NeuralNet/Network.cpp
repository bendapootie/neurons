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
}

void Neuron::RandomizeWeights(Random& rand)
{
	for (auto& weight : weights)
	{
		weight = rand.NextGaussian();
	}
}

void Neuron::RandomizeSingleWeight(int weightIndex, Random& rand)
{
	weights[weightIndex] = rand.NextGaussian();
}

void Neuron::RandomizeBias(Random& rand)
{
	bias = rand.NextGaussian();
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


void Network::InitializeFromParents(const Network& parent0, const Network& parent1, Random& rand)
{
	*this = parent0;

	const int maxLevel = Math::Min(int(m_levels.size()), int(parent1.m_levels.size()));
	for (int i = 0; i < maxLevel; i++)
	{
		NetworkLevel& level = m_levels[i];
		const NetworkLevel& p1Level = parent1.m_levels[i];
		
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

	Mutate(rand);
}

void Network::Mutate(Random& rand)
{
	// Chance per network of adding a level
	if (rand.NextFloat() < m_mutationSettings.addLevel)
	{
		AddRandomLevel(rand);
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

void Network::AddRandomLevel(Random& rand)
{
	int newIndex = rand.NextInt(1, (int)m_levels.size());

	const int neuronsinPreviousLevel = (int)m_levels[newIndex - 1].neurons.size();
	NetworkLevel newLevel(neuronsinPreviousLevel, neuronsinPreviousLevel);

	constexpr bool completelyRandomizeNewLevel = true;
	if constexpr (completelyRandomizeNewLevel)
	{
		newLevel.Randomize(rand);
	}
	else
	{
		for (int i = 0; i < newLevel.neurons.size(); i++)
		{
			newLevel.neurons[i].weights[i] = 1.0f;
		}
	}

	m_levels.insert(m_levels.begin() + newIndex, newLevel);
}
