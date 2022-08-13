#include <SFML/Graphics.hpp>

// TODO: These are just included for testing. Remove them when not needed anymore.
#include "Util/Array.h"
class Shape;

class PhysicsTest
{
public:
	PhysicsTest() {}
	~PhysicsTest();

	void Initialize();
	int Run();

private:
	void InitializeGame();
	void UpdateGame();
	void DrawGame();
	void UpdatePhysicsTest();
	void DrawPhysicsTest();


private:
	sf::RenderWindow m_window;

	// Physics test members
	bool m_initialized = false;
	Array<Shape*, 64> m_shapes;

	float m_totalLinearEnergy = 0.0f;
	float m_totalRotationalEnergy = 0.0f;
};
