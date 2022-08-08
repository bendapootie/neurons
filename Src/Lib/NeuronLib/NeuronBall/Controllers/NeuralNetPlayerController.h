#pragma once

#include "NeuronBall/NeuronPlayerController.h"

class NeuralNetPlayerController : public NeuronPlayerController
{
public:
	NeuralNetPlayerController(const int playerIndex);

	virtual void GetInputFromGameState(NeuronPlayerInput& outPlayerInput, const NeuronGame& game) override;

private:
	const int m_playerIndex = 0;
};
