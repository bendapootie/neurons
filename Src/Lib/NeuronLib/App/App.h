// 
// Main.cpp
//
// This is the entry point for the application. This file is a simple, thin
// wrapper for creating the App object and handling command line parameters
//

#include <SFML/Graphics.hpp>

// TODO: These are just included for testing. Remove them when not needed anymore.
#include "Util/Array.h"
class AiPlayerTrainer;
class NeuronGame;
class Shape;

class App
{
public:
	App() {}
	~App();

	void Initialize();
	int Run();

private:
	void InitializeGame();
	void UpdateGame();
	void DrawGame();

private:
	sf::RenderWindow m_window;

	// Instance of AiPlayerTrainer for running game simulations and training NeuralNetPlayerController
	AiPlayerTrainer* m_aiPlayerTrainer = nullptr;

	// TEMP: Game instance for testing
	NeuronGame* m_testGame = nullptr;
};
