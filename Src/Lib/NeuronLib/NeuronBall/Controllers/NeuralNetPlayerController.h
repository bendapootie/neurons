#pragma once

#include "NeuronBall/NeuronPlayerController.h"

class Network;

class NeuralNetPlayerController : public NeuronPlayerController
{
public:
	NeuralNetPlayerController();
	~NeuralNetPlayerController();

	virtual void GetInputFromGameState(NeuronPlayerInput& outPlayerInput, const NeuronGame& game, const int playerIndex) override;

	// Rewrites m_neuralNetwork by combining parents
	void Breed(const NeuralNetPlayerController& parent0, const NeuralNetPlayerController& parent1);
	// Rewrites m_neuralNetwork randomly
	void Randomize();

private:
	Network* m_neuralNetwork = nullptr;
};
