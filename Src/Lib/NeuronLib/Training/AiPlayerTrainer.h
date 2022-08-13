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
		int m_numControllers = 0;
		int m_numGamesPerController = 0;
	};

	AiPlayerTrainer(const Config& config);
	~AiPlayerTrainer();

private:
	const Config m_config;
	std::vector<NeuronGame*> m_games;
	std::vector<AiControllerData*> m_controllers;
};
