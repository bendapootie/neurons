#include "pch.h"
#include "App.h"

#include <SFML/Graphics.hpp>

// TODO: These are just included for testing. Remove them when not needed anymore.
#include "NeuronBall/Controllers/HumanPlayerController.h"
#include "NeuronBall/Controllers/NeuralNetPlayerController.h"
#include "NeuronBall/NeuronGame.h"
#include "NeuronBall/NeuronGameDisplay.h"
#include "Training/AiControllerData.h"
#include "Training/AiPlayerTrainer.h"
#include <chrono>

enum class PlayMode
{
	TrainAiControllers,
	VsSavedAi,
	PlayerVsPlayer
};

constexpr PlayMode k_playMode = PlayMode::TrainAiControllers;
constexpr float k_gameDuration = 30.0f * 1.0f;

constexpr int k_numControllers = 1024;
constexpr int m_numGameSeasons = 4;
constexpr float k_percentControllersToKeepPerGeneration = 0.2f;
constexpr int k_saveEveryNGenerations = 100;
constexpr int k_numGenerationsToRun = 1000 * 1000;
const char* k_saveFileName = "Ai_v%d_%d_%d_1024_%dgen.bin";
const char* k_loadFileName = "Ai_v0_1_0_1024_9000gen.bin";

// Disable vsync when training AI so the simulations can run as fast as possible
constexpr bool k_vsyncEnabled = (k_playMode != PlayMode::TrainAiControllers);
constexpr float k_windowWidth = 640;	// 1280
constexpr float k_aspectRatio = 16.0f / 9.0f;

constexpr int k_startupDelay = 60 * 0;

App::~App()
{
	if (m_aiPlayerTrainer != nullptr)
	{
		delete m_aiPlayerTrainer;
	}

	if (m_testGame != nullptr)
	{
		delete m_testGame;
	}
}

void App::Initialize()
{
	m_window.create(sf::VideoMode(static_cast<int>(k_windowWidth), static_cast<int>(Math::Ceil(k_windowWidth / k_aspectRatio))), "Neurons");
	m_window.setVerticalSyncEnabled(k_vsyncEnabled);

	InitializeGame();
}

int App::Run()
{
	int returnCode = 0;
	while (m_window.isOpen())
	{
		sf::Event event;
		if (!m_skipPollEvent)
		{
			while (m_window.pollEvent(event))
			{
				if (event.type == sf::Event::Closed)
				{
					m_window.close();
				}
			}
		}

		if (m_window.isOpen())
		{
			static int s_startupDelay = k_startupDelay;
			if (s_startupDelay > 0)
			{
				s_startupDelay--;
			}
			else
			{
				UpdateGame();
			}
			DrawGame();
		}
	}
	return returnCode;
}

void App::InitializeGame()
{
	switch (k_playMode)
	{
	case PlayMode::TrainAiControllers:
	{
		AiPlayerTrainer::Config config;
		config.m_numControllers = k_numControllers;
		config.m_numGameSeasons = m_numGameSeasons;
		config.m_gameDuration = k_gameDuration;
		config.m_percentToKeep = k_percentControllersToKeepPerGeneration;
		config.m_saveEveryNGenerations = k_saveEveryNGenerations;
		config.m_numGenerations = k_numGenerationsToRun;
		config.m_saveFile = k_saveFileName;

		_ASSERT(m_aiPlayerTrainer == nullptr);
		m_aiPlayerTrainer = new AiPlayerTrainer(config);

		m_aiPlayerTrainer->ReadControllersFromFile(k_saveFileName, k_saveEveryNGenerations, k_numGenerationsToRun);
		break;
	}

	case PlayMode::VsSavedAi:
	{
		// TODO: Figure out a better way of loading AI controllers without instantiating m_aiPlayerTrainer
		AiPlayerTrainer::Config dummyConfig;
		m_aiPlayerTrainer = new AiPlayerTrainer(dummyConfig);
		m_aiPlayerTrainer->ReadControllersFromFile(k_loadFileName);

		_ASSERT(m_testGame == nullptr);
		m_testGame = new NeuronGame();
//		m_testGame->SetPlayerController(0, new HumanPlayerController(0));
		m_testGame->SetPlayerController(0, m_aiPlayerTrainer->GetAiController(1)->m_controller);
		m_testGame->SetPlayerController(1, m_aiPlayerTrainer->GetAiController(512)->m_controller);
		break;
	}
	break;

	case PlayMode::PlayerVsPlayer:
	{
		_ASSERT(m_testGame == nullptr);
		m_testGame = new NeuronGame();
		m_testGame->SetPlayerController(0, new HumanPlayerController(0));
		m_testGame->SetPlayerController(1, new HumanPlayerController(1));
		break;
	}

	default:
		break;
	}
}

void App::UpdateGame()
{
	if (k_playMode == PlayMode::TrainAiControllers)
	{
		m_aiPlayerTrainer->Update();
	}
	else
	{
		m_testGame->Update();
	}
}

void App::DrawGame()
{
	const NeuronGame* gameToDisplay = m_testGame;
	if (k_playMode == PlayMode::TrainAiControllers)
	{
		gameToDisplay = m_aiPlayerTrainer->GetGame(0);
	}

	// Only update the display if vsync is on or enough time has passed
	bool updateDisplay = k_vsyncEnabled;
	if (!k_vsyncEnabled)
	{
		auto now = std::chrono::high_resolution_clock::now();
		static auto s_lastDisplayTime = now;
		auto timeSinceUpdate = now - s_lastDisplayTime;
		auto elapsedMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(timeSinceUpdate);
		if (elapsedMilliseconds.count() >= 16)
		{
			updateDisplay = true;
			s_lastDisplayTime = now;
		}
	}
	if (updateDisplay)
	{
		const float length = gameToDisplay->GetFieldLength();
		const float width = gameToDisplay->GetFieldWidth();
		const float viewSize = length * 2.0f;
		sf::View view(sf::Vector2f(length * 0.5f, width * 0.5f), sf::Vector2f(viewSize, viewSize / k_aspectRatio));
		m_window.setView(view);
		m_window.clear(sf::Color(0, 0, 32, 255));

		NeuronGameDisplay gameDisplay(*gameToDisplay);
		gameDisplay.Draw(m_window);

		m_window.display();
	}
	// Only poll for OS events if the display was updated. It wastes CPU to do it more often.
	m_skipPollEvent = !updateDisplay;
}
