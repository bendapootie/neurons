#include "pch.h"
#include "App.h"

#include <SFML/Graphics.hpp>

// TODO: These are just included for testing. Remove them when not needed anymore.
#include "NeuronBall/Controllers/HumanPlayerController.h"
#include "NeuronBall/Controllers/NeuralNetPlayerController.h"
#include "NeuronBall/NeuronGame.h"
#include "NeuronBall/NeuronGameDisplay.h"
#include "Training/AiPlayerTrainer.h"
#include <chrono>

constexpr bool k_vsyncEnabled = true;
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
}

int App::Run()
{
	int returnCode = 0;
	while (m_window.isOpen())
	{
		sf::Event event;
		while (m_window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
			{
				m_window.close();
			}
		}

		if (m_window.isOpen())
		{
			if (m_testGame == nullptr)
			{
				InitializeGame();
			}
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
	m_testGame = new NeuronGame();
	m_testGame->SetPlayerController(0, new NeuralNetPlayerController(0));
	m_testGame->SetPlayerController(1, new NeuralNetPlayerController(1));
}

void App::UpdateGame()
{
	// Update game
	m_testGame->Update();
}

void App::DrawGame()
{
	const float length = m_testGame->GetFieldLength();
	const float width = m_testGame->GetFieldWidth();
	const float viewSize = length * 2.0f;
	sf::View view(sf::Vector2f(length * 0.5f, width * 0.5f), sf::Vector2f(viewSize, viewSize / k_aspectRatio));
	m_window.setView(view);
	m_window.clear(sf::Color(0, 0, 32, 255));

	NeuronGameDisplay gameDisplay(*m_testGame);
	gameDisplay.Draw(m_window);

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
		m_window.display();
	}
}
