class NeuronBall;

namespace sf
{
	class RenderWindow;
}

// Helper class to visualize a NeuronBall game
// Keeps drawing code out of core NeuronBall logic, which needs to run
// as fast as possible to simulate lots and lots of games.
class NeuronBallDisplay
{
public:
	NeuronBallDisplay(const NeuronBall& neuronBall) :
		m_neuronBall(neuronBall)
	{
	}
	
	void Draw(sf::RenderWindow& window);
private:
	const NeuronBall& m_neuronBall;
};
