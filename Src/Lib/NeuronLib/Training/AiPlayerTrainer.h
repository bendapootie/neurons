#pragma once

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
	};

	AiPlayerTrainer(const Config& config);
	~AiPlayerTrainer();

	void Update();

	int GetNumGames() const { return static_cast<int>(1); }
	const NeuronGame* GetGame(const int index) { return m_currentGame; }

public:
	void PrepareNextGeneration();

private:
	const Config m_config;

	// Actively running game
	// TODO: Make this a list of games and run them concurrently on different threads
	NeuronGame* m_currentGame = nullptr;

	// Current AI controllers being trained
	std::vector<AiControllerData*> m_controllers;

	GameSeason* m_season = nullptr;
	int m_currentGameInSeason = 0;

	int m_generation = 0;
};
