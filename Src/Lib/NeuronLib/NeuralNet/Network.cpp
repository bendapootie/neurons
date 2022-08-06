#include "pch.h"
#include "Network.h"

#include "Util/Random.h"

void Neuron::RandomizeWeights()
{
	for (auto& weight : weights)
	{
		weight = Random::NextGaussian();
	}
}

void Neuron::RandomizeBias()
{
	bias = Random::NextGaussian();
}

void Network::Mutate()
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

void Network::AddRandomLevel()
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
