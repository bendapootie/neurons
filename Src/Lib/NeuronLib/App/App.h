
#include <SFML/Graphics.hpp>

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

private:
	sf::RenderWindow m_window;

	// Instance of AiPlayerTrainer for running game simulations and training NeuralNetPlayerController
	AiPlayerTrainer* m_aiPlayerTrainer = nullptr;

	// TEMP: Game instance for testing
	NeuronGame* m_testGame = nullptr;
};
