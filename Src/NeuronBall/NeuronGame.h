
class NeuronPlayer
{
public:
	NeuronPlayer() {}
	NeuronPlayer(const float x, const float y, const float facing) :
		m_x(x),
		m_y(y),
		m_facingRadians(facing)
	{}

	float GetRotationDegrees() const { return m_facingRadians * (180.0f / 3.1415926535f); }

public:
	float m_x = 0.0f;
	float m_y = 0.0f;
	float m_facingRadians = 0.0f;
	float m_vx = 0.0f;
	float m_vy = 0.0f;
};

class NeuronBall
{
public:
	NeuronBall() {}
	NeuronBall(const float x, const float y) :
		m_x(x),
		m_y(y)
	{}
	float GetRadius() const { return 2.0f; }

public:
	float m_x = 0.0f;
	float m_y = 0.0f;
	float m_vx = 0.0f;
	float m_vy = 0.0f;
};

// Instance of a simple 1v1 soccer-like game
// Player = 1.4m long
// Field 100m x 80m
// One corner of the field is at (0, 0) the other is at (width, length)
class NeuronGame
{
public:
	NeuronGame() :
		m_player1(m_fieldWidth * 0.5f, m_fieldLength * 0.1f, 3.14159f * 0.0f),
		m_player2(m_fieldWidth * 0.5f, m_fieldLength * 0.9f, 3.14159f * 1.0f),
		m_ball(m_fieldWidth * 0.5f, m_fieldLength * 0.5f)
	{}

	float GetFieldWidth() const { return m_fieldWidth; }
	float GetFieldLength() const { return m_fieldLength; }
	float GetGoalWidth() const { return m_fieldLength * 0.25f; }
	int GetNumPlayers() const { return 2; }
	const NeuronPlayer& GetPlayer(const int index) const { return (index == 0) ? m_player1 : m_player2; }
	const NeuronBall& GetBall() const { return m_ball; }
private:
	float m_fieldWidth = 80.0f;
	float m_fieldLength = 100.0f;
	NeuronPlayer m_player1;
	NeuronPlayer m_player2;
	NeuronBall m_ball;
};
