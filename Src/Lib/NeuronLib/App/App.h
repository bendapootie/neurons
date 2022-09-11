#pragma once

#include <SFML/Graphics.hpp>
#include "Util/RefCount.h"

class AiPlayerTrainer;
class NeuronGame;

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

	// Draws and processes the debug ImGui menu
	void UpdateDebugMenu();

	void DebugMenuHelper_UpdateDisplay();
	void DebugMenuHelper_UpdatePlayGame();
	void DebugMenuHelper_UpdateTraining();
	// Checks keyboard and controller input to see if debug menu display should be toggled
	void CheckForDebugMenuToggle();

private:
	sf::RenderWindow m_window;

	// Instance of AiPlayerTrainer for running game simulations and training NeuralNetPlayerController
	AiPlayerTrainer* m_aiPlayerTrainer = nullptr;

	// TEMP: Game instance for testing
	NeuronGame* m_testGame = nullptr;

	bool m_renderThisFrame = true;

	// TODO: Move UI state into separate class
	// RefCount object to track whether user input is allowed or not
	RefCount m_userInputBlocker;
	bool m_menuOpen = false;
	bool m_uiMenuToggleEscDownLastFrame = false;
	bool m_uiMenuToggleF10DownLastFrame = false;
	bool m_uiMenuToggleClawDownLastFrame = false;
};
