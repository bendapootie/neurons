#pragma once

#include "NeuronBall/NeuronPlayerController.h"
#include "InputProvider.h"

class HumanPlayerController : public NeuronPlayerController
{
public:
	// Note: InputProvider is passed by value because it's so small and there's only one implementation.
	//       If either of those things change, it may need to be constructed differently
	static_assert(sizeof(InputProvider) <= 16);
	HumanPlayerController(InputProvider inputProvider);

	virtual void GetInputFromGameState(NeuronPlayerInput& outPlayerInput, const NeuronGame& game, const int playerIndex) override;

private:
	InputProvider m_inputProvider;
};
