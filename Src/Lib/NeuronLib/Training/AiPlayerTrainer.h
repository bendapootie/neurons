#pragma once

#include <vector>

class AiControllerData;
class NeuronGame;

class AiPlayerTrainer
{
public:
	class Config
	{
	public:
		int m_numControllers = 2;
		int m_numGamesPerController = 1;
		float m_gameDuration = 60.0f;
	};

	AiPlayerTrainer(const Config& config);
	~AiPlayerTrainer();

	void Update();

	int GetNumGames() const { return static_cast<int>(m_games.size()); }
	const NeuronGame* GetGame(const int index) { return m_games[index]; }

private:
	const Config m_config;
	std::vector<NeuronGame*> m_games;
	std::vector<AiControllerData*> m_controllers;
};
