#pragma  once
#include "NeuronBall.h"
#include "NeuronPlayer.h"
#include "Util/Constants.h"
#include "Util/Vector.h"

class NeuronPlayerInput;
class NeuronPlayer;
class NeuronPlayerController;

constexpr int k_numPlayers = 2;

// Instance of a simple 1v1 soccer-like game
// Player = 1.4m long
// Field 100m x 80m
// One corner of the field is at (0, 0) the other is at (width, length)
class NeuronGame
{
public:
	NeuronGame();

	void SetPlayerController(int playerIndex, NeuronPlayerController* playerController);

	void Update();
	bool IsGameOver() const;

	float GetFieldWidth() const { return m_fieldWidth; }
	float GetFieldLength() const { return m_fieldLength; }
	float GetGoalWidth() const { return m_fieldWidth * 0.25f; }
	static constexpr int GetNumPlayers() { return k_numPlayers; }
	const NeuronPlayer& GetPlayer(const int index) const { return m_players[index]; }
	NeuronPlayer& GetPlayer(const int index) { return m_players[index]; }
	const NeuronBall& GetBall() const { return m_ball; }
	int GetPlayerScore(const int playerIndex) const { return m_scores[playerIndex]; }
	float GetTimeRemaining() const { return m_timeRemaining; }

private:
	// Called after a goal to reset player and ball positions
	void ResetField();

	static void ApplyInputToPlayer(NeuronPlayer& outPlayer, const NeuronPlayerInput& input);
	void UpdateBall();
	void ProcessCollisions();
	void CheckForGoal();
	void ScoreForPlayerIndex(const int playerIndex);

private:
	// Length is along x-axis
	const float m_fieldLength = 100.0f;
	// Width is along y-axis
	const float m_fieldWidth = 80.0f;
	Array<NeuronPlayer, k_numPlayers> m_players;
	NeuronBall m_ball;
	Array<int, k_numPlayers> m_scores;
	float m_timeRemaining;

	Array<NeuronPlayerController*, k_numPlayers> m_playerControllers;
};
