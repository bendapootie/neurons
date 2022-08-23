#include "pch.h"
#include "AiControllerData.h"

#include "NeuronBall/Controllers/NeuralNetPlayerController.h"


AiControllerData::AiControllerData(Random& rand)
{
	m_controller = new NeuralNetPlayerController(rand);
}

AiControllerData::~AiControllerData()
{
	if (m_controller != nullptr)
	{
		delete m_controller;
	}
}

void AiControllerData::Serialize(BinaryBuffer& stream) const
{
	m_controller->Serialize(stream);
	SerializeSimpleObject(stream, m_winLossRecord);
	SerializeInt(stream, m_generation);
}

void AiControllerData::Deserialize(BinaryBuffer& stream)
{
	m_controller->Deserialize(stream);
	DeserializeSimpleObject(stream, m_winLossRecord);
	DeserializeInt(stream, m_generation);
}
