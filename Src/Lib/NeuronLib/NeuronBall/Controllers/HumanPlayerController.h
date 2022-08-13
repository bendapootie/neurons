#pragma once

#include "NeuronBall/NeuronPlayerController.h"

class HumanPlayerController : public NeuronPlayerController
{
public:
	HumanPlayerController(const int joystickIndex);

	virtual void GetInputFromGameState(NeuronPlayerInput& outPlayerInput, const NeuronGame& game, const int playerIndex) override;

private:
	const int m_joystickIndex = 0;
};
