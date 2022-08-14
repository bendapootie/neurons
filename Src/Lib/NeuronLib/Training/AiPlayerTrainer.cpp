#include "pch.h"
#include "AiPlayerTrainer.h"

#include <algorithm>
#include "AiControllerData.h"
#include "NeuronBall/NeuronGame.h"
#include "NeuronBall/Controllers/NeuralNetPlayerController.h"
#include "Util/Random.h"
#include <windows.h> // for OutputDebugString

constexpr int k_pointsForWin = 3;
constexpr int k_pointsForDraw = 1;
constexpr int k_pointsForLoss = 0;

class GameStats
{
public:
	GameStats(const int index0, const int index1) :
		m_controllerIndex0(index0),
		m_controllerIndex1(index1)
	{
	}

public:
	int m_controllerIndex0 = -1;
	int m_controllerIndex1 = -1;
};


class GameSeason
{
public:
	GameSeason(int numControllers, int numSeasons)
	{
		for (int season = 0; season < numSeasons; season++)
		{
			for (int controllerIndex = 0; controllerIndex < numControllers; controllerIndex++)
			{
				const int controllerIndex0 = controllerIndex;
				const int controllerIndex1 = (controllerIndex + season + 1) % numControllers;
				m_gameStats.push_back(GameStats(controllerIndex0, controllerIndex1));
			}
		}
	}

public:
	std::vector<GameStats> m_gameStats;
};



AiPlayerTrainer::AiPlayerTrainer(const Config& config) :
	m_config(config)
{
	for (int i = 0; i < m_config.m_numControllers; i++)
	{
		AiControllerData* aiControllerData = new AiControllerData();
		aiControllerData->m_controller->Randomize();
		m_controllers.push_back(aiControllerData);
	}

	// TODO: Use multiple games once execution is multi-threaded
	m_currentGame = new NeuronGame();

	m_season = new GameSeason(config.m_numControllers, config.m_numGameSeasons);
}

AiPlayerTrainer::~AiPlayerTrainer()
{
	if (m_currentGame != nullptr)
	{
		delete m_currentGame;
	}

	for (auto controller : m_controllers)
	{
		if (controller != nullptr)
		{
			delete controller;
		}
	}
	m_controllers.clear();

	_ASSERT(m_season != nullptr);
	delete m_season;
}

void AiPlayerTrainer::Update()
{
	NeuronGame* game = m_currentGame;

	if (game->GetGameState() == GameState::PreGame)
	{
		const GameStats& stats = m_season->m_gameStats[m_currentGameInSeason];
		
		game->SetPlayerController(0, m_controllers[stats.m_controllerIndex0]->m_controller);
		game->SetPlayerController(1, m_controllers[stats.m_controllerIndex1]->m_controller);
	}

	game->Update();

	if (game->IsGameOver())
	{
		// Record the game's score and reset the game

		const GameStats& stats = m_season->m_gameStats[m_currentGameInSeason];
		AiControllerData* p0 = m_controllers[stats.m_controllerIndex0];
		AiControllerData* p1 = m_controllers[stats.m_controllerIndex1];

		// TODO: Keep track of which AiControllerData was used for each game
		int p0Score = game->GetPlayerScore(0);
		int p1Score = game->GetPlayerScore(1);
		if (p0Score == p1Score)
		{
			p0->m_points += k_pointsForDraw;
			p1->m_points += k_pointsForDraw;
		}
		else if (p0Score > p1Score)
		{
			p0->m_points += k_pointsForWin;
			p1->m_points += k_pointsForLoss;
		}
		else
		{
			p0->m_points += k_pointsForLoss;
			p1->m_points += k_pointsForWin;
		}
		game->ResetGame(m_config.m_gameDuration);

		// Shift to next game in season
		m_currentGameInSeason++;

		// Check if all games have been run
		if (m_currentGameInSeason >= m_season->m_gameStats.size())
		{
			PrepareNextGeneration();
		}
	}
}

void AiPlayerTrainer::PrepareNextGeneration()
{
	// Sort controllers based on score.
	sort(begin(m_controllers),
		end(m_controllers),
		[](AiControllerData* a, AiControllerData* b) {return a->m_points > b->m_points; });

	// Output stats
	char msg[64];
	sprintf_s(msg, "Generation %d complete =====================================\n", m_generation);
	OutputDebugStringA(msg);
	sprintf_s(msg, "%d games played in %d seasons\n", static_cast<int>(m_season->m_gameStats.size()), m_config.m_numGameSeasons);
	OutputDebugStringA(msg);

	for (int i = 0; i < m_controllers.size(); i++)
	{
		const AiControllerData* aiData = m_controllers[i];
		sprintf_s(msg, "Controller %2d = %3d points\n", i, aiData->m_points);
		OutputDebugStringA(msg);
	}

	// Replace "dead" controllers with new ones for the next generation
	const int numControllersToKeep = static_cast<int>(m_controllers.size() * m_config.m_percentToKeep);
	for (int i = numControllersToKeep; i < m_controllers.size(); i++)
	{
		m_controllers[i]->m_controller->Randomize();
	}

	// Randomize seeding of controllers
	for (int i = 0; i < m_controllers.size() - 1; i++)
	{
		const int swapIndex = Random::NextInt(i + 1, static_cast<int>(m_controllers.size()));
		std::swap(m_controllers[i], m_controllers[swapIndex]);
	}

	// Reset points for next generation
	for (auto controller : m_controllers)
	{
		controller->m_points = 0;
	}

	m_generation++;
	m_currentGameInSeason = 0;
}
