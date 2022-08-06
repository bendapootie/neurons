#pragma once

#include "NeuronBall/NeuronPlayerController.h"

class NeuralNetPlayerController : public NeuronPlayerController
{
public:
	NeuralNetPlayerController();

	virtual void GetInputFromGameState(NeuronPlayerInput& outPlayerInput, const NeuronGame& game) override;

private:
};
