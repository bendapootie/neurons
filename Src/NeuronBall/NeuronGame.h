#pragma  once
#include "NeuronBall.h"
#include "NeuronPlayer.h"
#include "Util/Constants.h"
#include "Util/Vector.h"

class NeuronPlayerInput;
class NeuronPlayer;

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
	NeuronPlayer& GetPlayer(const int index) { return (index == 0) ? m_player0 : m_player1; }
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
