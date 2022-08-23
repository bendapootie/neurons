#pragma once

#include "Util/Random.h"
#include <vector>

class AiControllerData;
class GameSeason;
class NeuronGame;

class AiPlayerTrainer
{
public:
	class Config
	{
	public:
		// How many AI controllers to create and train
		int m_numControllers = 16;
		// How many seasons to run. Each season has controllers play two games.
		int m_numGameSeasons = 1;
		float m_gameDuration = 60.0f;

		float m_percentToKeep = 0.2f;
		//float m_mutationRate = 0.01f;

		int m_numGenerations = 10;
		int m_saveEveryNGenerations = 100;
		const char* m_saveFile = nullptr;
	};

	AiPlayerTrainer(const Config& config);
	~AiPlayerTrainer();

	void Update();

	int GetNumGames() const { return static_cast<int>(1); }
	const NeuronGame* GetGame(const int index) { return m_currentGame; }

	// TODO: Should this be public???
	// TODO: The entire managment of saving and loading AIControllers should be rethought and refactored
	void ReadControllersFromFile(const char* outputFileName);
	AiControllerData* GetAiController(int index);
	AiControllerData* GetBestAiController();


private:
	void PrepareNextGeneration();
	void WriteControllersToFile(const char* outputFileName) const;

private:
	const Config m_config;
	Random m_rand;

	// Actively running game
	// TODO: Make this a list of games and run them concurrently on different threads
	NeuronGame* m_currentGame = nullptr;

	// Current AI controllers being trained
	std::vector<AiControllerData*> m_controllers;

	GameSeason* m_season = nullptr;
	int m_currentGameInSeason = 0;

	int m_generation = 0;
};
