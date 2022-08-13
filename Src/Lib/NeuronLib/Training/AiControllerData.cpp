#include "pch.h"
#include "AiControllerData.h"

#include "NeuronBall/Controllers/NeuralNetPlayerController.h"


AiControllerData::AiControllerData()
{
	m_controller = new NeuralNetPlayerController();
}

AiControllerData::~AiControllerData()
{
	if (m_controller != nullptr)
	{
		delete m_controller;
	}
}
