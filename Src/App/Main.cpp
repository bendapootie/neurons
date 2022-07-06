// 
// Main.cpp
//
// This is the entry point for the application. This file is a simple, thin
// wrapper for creating the App object and handling command line parameters
//

#include <SFML/Graphics.hpp>

// TODO: These are just included for testing. Remove them when not needed anymore.
#include "NeuronBall/NeuronGame.h"
#include "NeuronBall/NeuronGameDisplay.h"

class App
{
public:
	App() {}
	~App() {}

	void Initialize()
	{
		m_window.create(sf::VideoMode(800, 600), "Neurons");
		m_window.setVerticalSyncEnabled(true);
	}

	int Run()
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
				Update();
				Draw();
			}
		}
		return returnCode;
	}

private:
	void Update()
	{
		NeuronPlayerInput i0;
		NeuronPlayerInput i1;

		// Sample Input
		if (sf::Joystick::isConnected(0))
		{
			float x = std::clamp(sf::Joystick::getAxisPosition(0, sf::Joystick::X) * 0.01f, -1.0f, 1.0f);
			i0.m_steering = (x * x) *((x >= 0.0f) ? 1.0f : -1.0f);
			i0.m_speed = -sf::Joystick::getAxisPosition(0, sf::Joystick::Z) * 0.01f;
		}

		// Update game with input
		m_testGame.Update(i0, i1);
	}

	void Draw()
	{
		sf::View view(sf::Vector2f(40.0f, 50.0f), sf::Vector2f(200.0f, 150.0f));
		m_window.setView(view);
		m_window.clear(sf::Color(0, 0, 32, 255));
		
		NeuronGameDisplay gameDisplay(m_testGame);
		gameDisplay.Draw(m_window);

		m_window.display();
	}

private:
	sf::RenderWindow m_window;
	NeuronGame m_testGame;
};

int WinMain(const int argc, const char** argv)
{
	App app;
	app.Initialize();
	int returnCode = app.Run();
	return returnCode;
}
