#include "pch.h"
#include "AiPlayerTrainer.h"

#include "AiControllerData.h"
#include "NeuronBall/NeuronGame.h"
#include "NeuronBall/Controllers/NeuralNetPlayerController.h"

constexpr int k_pointsForWin = 3;
constexpr int k_pointsForDraw = 1;
constexpr int k_pointsForLoss = 0;


AiPlayerTrainer::AiPlayerTrainer(const Config& config) :
	m_config(config)
{
	for (int i = 0; i < m_config.m_numControllers; i++)
	{
		m_controllers.push_back(new AiControllerData());
	}

	// TODO: Use multiple games once execution is multi-threaded
	m_games.push_back(new NeuronGame());
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
	NeuronGame* game = m_games[0];

	switch (game->GetGameState())
	{
	case GameState::GameOver:
	{
		// TODO: Keep track of which AiControllerData was used for each game
		int p0Score = game->GetPlayerScore(0);
		int p1Score = game->GetPlayerScore(1);
		if (p0Score == p1Score)
		{
			m_controllers[0]->m_points += k_pointsForDraw;
			m_controllers[1]->m_points += k_pointsForDraw;
		}
		else if (p0Score > p1Score)
		{
			m_controllers[0]->m_points += k_pointsForWin;
			m_controllers[1]->m_points += k_pointsForLoss;
		}
		else
		{
			m_controllers[0]->m_points += k_pointsForLoss;
			m_controllers[1]->m_points += k_pointsForWin;
		}
	}
	// intentionally fall through...
	case GameState::PreGame:
		// Record the score and start the next game
		game->ResetGame(m_config.m_gameDuration);

		m_controllers[0]->m_controller->Randomize();
		m_controllers[1]->m_controller->Randomize();

		game->SetPlayerController(0, m_controllers[0]->m_controller);
		game->SetPlayerController(1, m_controllers[1]->m_controller);
		break;
	}

	game->Update();
}
