#pragma  once
#include "Util/Vector.h"

class NeuronGame;
class NeuronPlayer;

class NeuronBall
{
public:
	NeuronBall() {}
	NeuronBall(const Vector2 pos) :
		m_pos(pos)
	{}

	bool CollideWithPlayer(const NeuronPlayer& player);
	bool CollideWithField(const NeuronGame& game);

	static constexpr float GetRadius() { return 2.0f; }
	// Roughly what percentage of velocity is lost per second while rolling
	static constexpr float GetRollingFriction() { return 0.2f; }

public:
	Vector2 m_pos = Vector2::Zero;
	Vector2 m_velocity = Vector2::Zero;
};
