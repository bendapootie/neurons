#pragma  once
#include "Util/Vector.h"

class NeuronBall;
class NeuronGame;

class NeuronPlayerInput
{
public:
	// Values outside this range will get clamped
	// -1.0 <= turn full left; 0.0 = straight; 1.0 >= turn full right
	float m_steering = 0.0f;
	// -1.0 <= full reverse; 0.0 = stop; 1.0 >= full forward
	float m_speed = 0.0f;
};

class NeuronPlayer
{
public:
	NeuronPlayer() {}
	NeuronPlayer(const Vector2 pos, const float facing) :
		m_pos(pos),
		m_facingRadians(facing)
	{}

	float GetRotationDegrees() const { return RadToDeg(m_facingRadians); }
	Vector2 ComputeForward() const { return Vector2(Math::Cos(m_facingRadians), Math::Sin(m_facingRadians)); }

	bool CollideWithField(const NeuronGame& game);
	bool CollideWithBall(const NeuronBall& ball);

	static constexpr float GetPlayerWidth() { return 4.0f; }
	static constexpr float GetPlayerLength() { return GetPlayerWidth() * 1.8f; }

public:
	Vector2 m_pos = Vector2::Zero;
	Vector2 m_velocity = Vector2::Zero;
	float m_facingRadians = 0.0f;
};
