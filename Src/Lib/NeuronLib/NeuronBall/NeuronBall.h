#pragma  once

#include "Util/Vector.h"
#include "Util/Shapes.h"

class NeuronGame;
class NeuronPlayer;

class NeuronBall
{
public:
	NeuronBall();
	NeuronBall(const Vector2 pos);

	bool CollideWithPlayer(const NeuronPlayer& player);
	bool CollideWithField(const NeuronGame& game);

	static constexpr float GetRadius() { return 2.0f; }
	// Roughly what percentage of velocity is lost per second while rolling
	static constexpr float GetRollingFriction() { return 0.2f; }

public:
	Circle m_shape;
};
