// 
// Main.cpp
//
// This is the entry point for the application. This file is a simple, thin
// wrapper for creating the App object and handling command line parameters
//

#include <SFML/Graphics.hpp>

// TODO: These are just included for testing. Remove them when not needed anymore.
#include "NeuronBall/NeuronGame.h"
#include "Util/Shapes.h"

class App
{
public:
	App() {}
	~App() {}

	void Initialize();
	int Run();

private:
	void UpdateGame();
	void DrawGame();
	void UpdatePhysicsTest();
	void DrawPhysicsTest();


private:
	sf::RenderWindow m_window;
	NeuronGame m_testGame;

	// Physics test members
	bool m_initialized = false;
	Array<Shape*, 64> m_shapes;
};
