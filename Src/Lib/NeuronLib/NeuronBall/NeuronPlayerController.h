#pragma once

class NeuronGame;
class NeuronPlayerInput;

class NeuronPlayerController
{
public:
	virtual void GetInputFromGameState(NeuronPlayerInput& outPlayerInput, const NeuronGame& game, const int playerIndex) = 0;
};
