#include "pch.h"
#include "NeuralNetPlayerController.h"

#include "NeuronBall/NeuronPlayerInput.h"
#include "NeuronBall/NeuronGame.h"
#include "NeuronBall/NeuronBall.h"
#include "Util/Array.h"

// TODO: Move to separate file?
class GameStateForNeuralNetInput
{
public:
	GameStateForNeuralNetInput(const NeuronGame& game, const int playerIndex) :
		m_game(game),
		m_playerIndex(playerIndex),
		m_nextInputToWrite(0)
	{
		SampleGameState();
	}

private:
	Vector2 PosRelativeToPlayer(Vector2 pos)
	{
		if (m_playerIndex == 0)
		{
			return pos;
		}

		// Equation...
		// c = center of the field
		// p = position to mirror around the center
		// p' = mirrored position
		// p' = c - (p - c) = 2c - p
		// In this case, c is (halfLength, halfWidth), so 2c is (length, width)
		const Vector2 fieldSize(m_game.GetFieldLength(), m_game.GetFieldWidth());
		return fieldSize - pos;
	}

	Vector2 VectorRelativeToPlayer(Vector2 vector)
	{
		if (m_playerIndex == 0)
		{
			return vector;
		}
		// Rotate 180 degrees
		return -vector;
	}

	void WriteWorldPosition(const Vector2 pos)
	{
		const Vector2 relativePos = PosRelativeToPlayer(pos);
		m_neuralNetInputs[m_nextInputToWrite++] = relativePos.x;
		m_neuralNetInputs[m_nextInputToWrite++] = relativePos.y;
	}

	void WriteRelativeVector(const Vector2 vector)
	{
		const Vector2 relativeVector = VectorRelativeToPlayer(vector);
		m_neuralNetInputs[m_nextInputToWrite++] = relativeVector.x;
		m_neuralNetInputs[m_nextInputToWrite++] = relativeVector.y;
	}

	void WriteFloat(const float value)
	{
		m_neuralNetInputs[m_nextInputToWrite++] = value;
	}

	void SampleGameState()
	{
		m_nextInputToWrite = 0;

		// TODO: Translate all game state to relate to input playerIndex

		// Input values...
		// - Player0 (Pos(x,y), Velocity(x,y), Forward(x,y), Boost)
		// - Player1 (Pos(x,y), Velocity(x,y), Forward(x,y))
		// - Ball (Pos(x,y), Velocity(x,y))
		// Optional
		// - Scores (mine, theirs)
		// - Time Remaining (sec)

		for (int p = 0; p < m_game.GetNumPlayers(); p++)
		{
			const NeuronPlayer& player = m_game.GetPlayer(p);

			WriteWorldPosition(player.GetPos());
			WriteRelativeVector(player.GetVelocity());
			WriteRelativeVector(player.GetForward());
			WriteFloat(player.GetBoostRemaining());
		}
		_ASSERT(m_nextInputToWrite == 14); // There should be 7 state variables per player

		{
			const NeuronBall& ball = m_game.GetBall();

			WriteWorldPosition(ball.m_shape.GetPos());
			WriteRelativeVector(ball.m_shape.GetVelocity());
		}
		_ASSERT(m_nextInputToWrite == 18); // Ball adds 4 more

		for (int p = 0; p < m_game.GetNumPlayers(); p++)
		{
			WriteFloat(static_cast<float>(m_game.GetPlayerScore(p)));
		}
		_ASSERT(m_nextInputToWrite == 20); // Scores add two

		WriteFloat(m_game.GetTimeRemaining());
		_ASSERT(m_nextInputToWrite == 21); // There are a total of 21 state variables that describe the the game
		_ASSERT(m_nextInputToWrite == k_numGameStateInputs); // Make sure these match
	}

private:
	static constexpr int k_numGameStateInputs = 21;

	const NeuronGame& m_game;
	const int m_playerIndex;
	int m_nextInputToWrite;
	Array<float, k_numGameStateInputs> m_neuralNetInputs;
};



//=============================================================================


NeuralNetPlayerController::NeuralNetPlayerController(const int playerIndex) :
	m_playerIndex(playerIndex)
{
}

void NeuralNetPlayerController::GetInputFromGameState(NeuronPlayerInput& outPlayerInput, const NeuronGame& game)
{
	GameStateForNeuralNetInput networkInput(game, m_playerIndex);
}

