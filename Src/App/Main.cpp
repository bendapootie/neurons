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
		window.create(sf::VideoMode(800, 600), "Neurons");
		window.setVerticalSyncEnabled(true);
	}

	int Run()
	{
		int returnCode = 0;
		while (window.isOpen())
		{
			sf::Event event;
			while (window.pollEvent(event))
			{
				if (event.type == sf::Event::Closed)
				{
					window.close();
				}
			}

			if (window.isOpen())
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
	}

	void Draw()
	{
		sf::View view(sf::Vector2f(40.0f, 50.0f), sf::Vector2f(200.0f, 150.0f));
		window.setView(view);
		window.clear(sf::Color(0, 0, 32, 255));
		
		static NeuronGame game;
		NeuronGameDisplay gameDisplay(game);
		gameDisplay.Draw(window);

		window.display();
	}

private:
	sf::RenderWindow window;
};

int WinMain(const int argc, const char** argv)
{
	App app;
	app.Initialize();
	int returnCode = app.Run();
	return returnCode;
}
