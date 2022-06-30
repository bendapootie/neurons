// 
// Main.cpp
//
// This is the entry point for the application. This file is a simple, thin
// wrapper for creating the App object and handling command line parameters
//

#include <SFML/Graphics.hpp>

class App
{
public:
	App() {}
	~App() {}

	void Initialize()
	{
		window.create(sf::VideoMode(800, 600), "Neurons");
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
		sf::View view(sf::Vector2f(0.0f, 0.0f), sf::Vector2f(800.0f, 600.0f));
		window.setView(view);
		window.clear(sf::Color(0, 0, 32, 255));
		
		sf::RectangleShape rect;
		rect.setSize(sf::Vector2f(100.0f, 50.0f));
		rect.setOutlineColor(sf::Color::Red);
		rect.setOutlineThickness(5.0f);
		rect.setPosition(100.0f, 200.0f);
		window.draw(rect);

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
