#include "pch.h"
#include "Network.h"

#include "Util/Math.h"
#include "Util/Random.h"

void Neuron::RandomizeWeights()
{
	for (auto& weight : weights)
	{
		weight = Random::NextGaussian();
	}
}

void Neuron::RandomizeSingleWeight(int weightIndex)
{
	weights[weightIndex] = Random::NextGaussian();
}

void Neuron::RandomizeBias()
{
	bias = Random::NextGaussian();
}

void Network::InitializeFromParents(const Network& parent0, const Network& parent1)
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
			if (Random::NextInt(0, 2) == 0)
			{
				level.neurons[n] = p1Level.neurons[n];
				const int originalNumWeights = static_cast<int>(level.neurons[n].weights.size());
				if (originalNumWeights != targetNumWeights)
				{
					level.neurons[n].weights.resize(targetNumWeights);
					for (int w = originalNumWeights; w < targetNumWeights; w++)
					{
						level.neurons[n].RandomizeSingleWeight(w);
					}
				}
			}
		}
	}

	Mutate();
}

void Network::Mutate()
{
	// Chance per network of adding a level
	if (Random::NextFloat() < m_mutationSettings.addLevel)
	{
		AddRandomLevel();
	}

	// Chance per level of adding a neuron
	// Don't add neurons to input or output layers
	for (int i = 1; i < (int)m_levels.size() - 1; i++)
	{
		if (Random::NextFloat() < m_mutationSettings.addNeuron)
		{
			// Add a new neuron to the target level
			auto& level = m_levels[i];
			level.AddNeuron();
			// Give each neuron on the next level an additional weight to account for the new neuron
			for (auto& neuron : m_levels[i + 1].neurons)
			{
				neuron.weights.push_back(Random::NextGaussian());
			}
		}
		if (Random::NextFloat() < m_mutationSettings.deleteNeuron)
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
					neuron.RandomizeAll();
				}
				i--;
			}
			else
			{
				const int neuronIndexToDelete = Random::NextInt(0, (int)level.neurons.size());
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
			if (Random::NextFloat() < m_mutationSettings.modifyWeights)
			{
				neuron.RandomizeWeights();
			}
			if (Random::NextFloat() < m_mutationSettings.modifyBias)
			{
				neuron.RandomizeBias();
			}
		}
	}
}

void Network::AddRandomLevel()
{
	int newIndex = Random::NextInt(1, (int)m_levels.size());

	const int neuronsinPreviousLevel = (int)m_levels[newIndex - 1].neurons.size();
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

	m_levels.insert(m_levels.begin() + newIndex, newLevel);
}
