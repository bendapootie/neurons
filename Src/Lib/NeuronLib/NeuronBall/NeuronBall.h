#pragma  once

#include "Util/Vector.h"
#include "Util/Shapes.h"

class NeuronGame;
class NeuronPlayer;

enum class FieldCollisionStyle
{
	PushOnly,
	PushAndBounce
};

class NeuronBall
{
public:
	NeuronBall();
	NeuronBall(const Vector2 pos);

	bool CollideWithField(const NeuronGame& game, const FieldCollisionStyle style);

	static constexpr float GetRadius() { return 2.0f; }
	// Roughly what percentage of velocity is lost per second while rolling
	static constexpr float GetRollingFriction() { return 0.2f; }

public:
	Circle m_shape;
};
