#pragma  once

class NeuronGame;

namespace sf
{
	class RenderWindow;
}

// Helper class to visualize a NeuronBall game
// Keeps drawing code out of core NeuronBall logic, which needs to run
// as fast as possible to simulate lots and lots of games.
class NeuronGameDisplay
{
public:
	NeuronGameDisplay(const NeuronGame& neuronGame) :
		m_neuronGame(neuronGame)
	{
	}
	
	void Draw(sf::RenderWindow& window) const ;
private:
	const NeuronGame& m_neuronGame;
};
