#pragma once

#include "NeuronBall/NeuronPlayerController.h"

class Network;

class NeuralNetPlayerController : public NeuronPlayerController
{
public:
	NeuralNetPlayerController(const int playerIndex);
	~NeuralNetPlayerController();

	virtual void GetInputFromGameState(NeuronPlayerInput& outPlayerInput, const NeuronGame& game) override;

private:
	const int m_playerIndex = 0;
	Network* m_neuralNetwork = nullptr;
};
