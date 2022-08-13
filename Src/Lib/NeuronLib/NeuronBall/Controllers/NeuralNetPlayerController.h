#pragma once

#include "NeuronBall/NeuronPlayerController.h"

class Network;

class NeuralNetPlayerController : public NeuronPlayerController
{
public:
	NeuralNetPlayerController();
	~NeuralNetPlayerController();

	virtual void GetInputFromGameState(NeuronPlayerInput& outPlayerInput, const NeuronGame& game, const int playerIndex) override;

	void Randomize();

private:
	Network* m_neuralNetwork = nullptr;
};
