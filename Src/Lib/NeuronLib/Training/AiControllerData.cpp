#include "pch.h"
#include "AiControllerData.h"

#include "NeuronBall/Controllers/NeuralNetPlayerController.h"


AiControllerData::AiControllerData()
{

}

AiControllerData::~AiControllerData()
{
	if (m_controller != nullptr)
	{
		delete m_controller;
	}
}
