
class NeuronBallPlayer
{
public:
private:
	float m_x;
	float m_y;
	float m_facingDegrees;
	float m_vx;
	float m_vy;
};

class NeuronBallBall
{
public:
private:
	float m_x;
	float m_y;
	float m_vx;
	float m_vy;
};

// Instance of a simple 1v1 soccer-like game
// Player = 1.4m long
// Field 100m x 80m
// One corner of the field is at (0, 0) the other is at (width, length)
class NeuronBall
{
public:
	float GetFieldWidth() const { return m_fieldWidth; }
	float GetFieldLength() const { return m_fieldLength; }
	float GetGoalWidth() const { return m_fieldLength * 0.25f; }
private:
	float m_fieldWidth = 80.0f;
	float m_fieldLength = 100.0f;
	NeuronBallPlayer m_player1;
	NeuronBallPlayer m_player2;
	NeuronBallBall m_ball;
};
