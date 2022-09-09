#include "pch.h"
#include "HumanPlayerController.h"

#include "NeuronBall/NeuronPlayerInput.h"
#include "Util/Math.h"

HumanPlayerController::HumanPlayerController(InputProvider inputProvider) :
	m_inputProvider(inputProvider)
{
}

void HumanPlayerController::GetInputFromGameState(NeuronPlayerInput& outPlayerInput, const NeuronGame& game, const int playerIndex)
{
	outPlayerInput.m_boost = m_inputProvider.GetBoost();
	outPlayerInput.m_speed = m_inputProvider.GetSpeed();
	outPlayerInput.m_steering = m_inputProvider.GetSteering();
}
