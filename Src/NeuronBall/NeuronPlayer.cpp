#include "NeuronPlayer.h"

#include "NeuronGame.h"

bool NeuronPlayer::CollideWithField(const NeuronGame& game)
{
	// Perf: Early-out if player is clearly not close to the walls
	const float fieldWidth = game.GetFieldWidth();
	const float fieldLength = game.GetFieldLength();

	const Vector2 halfForward = ComputeForward() * 0.5f;
	const Vector2 halfRight = Vector2(halfForward.y, -halfForward.x);

	const Vector2 halfLengthVector = halfForward * GetPlayerLength();
	const Vector2 halfWidthVector = halfRight * GetPlayerWidth();

	// Figure out where all the corners are and check if any of them are out of bounds
	Vector2 corners[4];
	corners[0] = Vector2(m_pos + halfLengthVector + halfWidthVector);
	corners[1] = Vector2(m_pos + halfLengthVector - halfWidthVector);
	corners[2] = Vector2(m_pos - halfLengthVector + halfWidthVector);
	corners[3] = Vector2(m_pos - halfLengthVector - halfWidthVector);

	Vector2 pushDistance = Vector2::Zero;

	// Check bounds of each corner
	for (int i = 0; i < 4; i++)
	{
		if (corners[i].x < 0)
		{
			pushDistance.x = Math::Max(pushDistance.x, -corners[i].x);
		}
		if (corners[i].x > fieldWidth)
		{
			pushDistance.x = Math::Min(pushDistance.x, fieldWidth - corners[i].x);
		}
		if (corners[i].y < 0)
		{
			pushDistance.y = Math::Max(pushDistance.y, -corners[i].y);
		}
		if (corners[i].y > fieldWidth)
		{
			pushDistance.y = Math::Min(pushDistance.y, fieldLength - corners[i].y);
		}
	}

	// Move the player back by how much it was over the line
	m_pos += pushDistance;

	// If pushDistance isn't zero, there was a collision
	return pushDistance != Vector2::Zero;
}

bool NeuronPlayer::CollideWithBall(const NeuronBall& ball)
{
	// TODO: Treat player shape as a rectangle, not a circle
	// TODO: Fix this and NeuronBall::CollideWithPlayer (merge the two functions?)
	bool wasCollision = false;

	const Vector2 ballToPlayer = m_pos - ball.m_pos;
	float distance;
	const Vector2 ballToPlayerNormal = ballToPlayer.GetSafeNormalized(distance);
	const float minDistanceAllowed = ball.GetRadius() + GetPlayerWidth();
	if (distance < minDistanceAllowed)
	{
		// Car collided with ball and needs to move away
		m_pos = ball.m_pos + (ballToPlayerNormal * minDistanceAllowed);
		wasCollision = true;
	}
	// TODO: Should velocities change here?

	return wasCollision;
}
