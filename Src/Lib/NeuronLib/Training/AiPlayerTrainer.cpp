#include "pch.h"
#include "AiPlayerTrainer.h"

#include "AiControllerData.h"
#include "NeuronBall/NeuronGame.h"


AiPlayerTrainer::AiPlayerTrainer(const Config& config) :
	m_config(config)
{

}

AiPlayerTrainer::~AiPlayerTrainer()
{
	for (auto game : m_games)
	{
		if (game != nullptr)
		{
			delete game;
		}
	}
	m_games.clear();

	for (auto controller : m_controllers)
	{
		if (controller != nullptr)
		{
			delete controller;
		}
	}
	m_controllers.clear();
}

void AiPlayerTrainer::Update()
{
}