#pragma  once
#include "Util/Constants.h"
#include "Util/Vector.h"

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

	static constexpr float GetPlayerWidth() { return 4.0f; }
	static constexpr float GetPlayerLength() { return GetPlayerWidth() * 1.8f; }

public:
	Vector2 m_pos = Vector2::Zero;
	Vector2 m_velocity = Vector2::Zero;
	float m_facingRadians = 0.0f;
};

class NeuronBall
{
public:
	NeuronBall() {}
	NeuronBall(const Vector2 pos) :
		m_pos(pos)
	{}

	static constexpr float GetRadius() { return 2.0f; }
	// Roughly what percentage of velocity is lost per second while rolling
	static constexpr float GetRollingFriction() { return 0.2f; }

public:
	Vector2 m_pos = Vector2::Zero;
	Vector2 m_velocity = Vector2::Zero;
};

// Instance of a simple 1v1 soccer-like game
// Player = 1.4m long
// Field 100m x 80m
// One corner of the field is at (0, 0) the other is at (width, length)
class NeuronGame
{
public:
	NeuronGame() :
		m_player0(Vector2(m_fieldWidth * 0.5f, m_fieldLength * 0.1f), DegToRad( 90.0f)),
		m_player1(Vector2(m_fieldWidth * 0.5f, m_fieldLength * 0.9f), DegToRad(270.0f)),
		m_ball(Vector2(m_fieldWidth * 0.5f, m_fieldLength * 0.5f))
	{}

	void Update(const NeuronPlayerInput& p0, const NeuronPlayerInput& p1);

	float GetFieldWidth() const { return m_fieldWidth; }
	float GetFieldLength() const { return m_fieldLength; }
	float GetGoalWidth() const { return m_fieldLength * 0.25f; }
	static constexpr int GetNumPlayers() { return 2; }
	const NeuronPlayer& GetPlayer(const int index) const { return (index == 0) ? m_player0 : m_player1; }
	const NeuronBall& GetBall() const { return m_ball; }

private:
	static void ApplyInputToPlayer(NeuronPlayer& outPlayer, const NeuronPlayerInput& input);
	void UpdateBall();
	void ProcessCollisions();

private:
	const float m_fieldWidth = 80.0f;
	const float m_fieldLength = 100.0f;
	NeuronPlayer m_player0;
	NeuronPlayer m_player1;
	NeuronBall m_ball;
};
