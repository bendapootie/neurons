#include "pch.h"
#include "NeuronBall.h"

#include "NeuronGame.h"
#include "NeuronPlayer.h"

bool NeuronBall::CollideWithPlayer(const NeuronPlayer& player)
{
	bool wasCollision = false;

	const Vector2 playerToBall = m_pos - player.m_pos;
	float distance;
	const Vector2 playerToBallNormal = playerToBall.GetSafeNormalized(distance);
	// TODO: Compute player radius correctly
	// TODO: Treat player shape as a rectangle, not a circle
	// TODO: Fix this and NeuronPlayer::CollideWithBall (merge the two functions?)
	const float minDistanceAllowed = GetRadius() + player.GetPlayerWidth();
	if (distance < minDistanceAllowed)
	{
		// Ball collided with car and needs to move away
		m_pos = player.m_pos + (playerToBallNormal * minDistanceAllowed);

		// TODO: This isn't the right way to apply velocity to the ball
		const float velocityDot = playerToBallNormal.Dot(player.m_velocity.GetSafeNormalized());
		if (velocityDot > 0.0f)
		{
			m_velocity = player.m_velocity * velocityDot;
		}
		wasCollision = true;
	}

	return wasCollision;
}

bool NeuronBall::CollideWithField(const NeuronGame& game)
{
	// Collide ball against bounds of the field
	const Vector2 minBound(GetRadius(), GetRadius());
	const Vector2 maxBound(game.GetFieldWidth() - GetRadius(), game.GetFieldLength() - GetRadius());

	const Vector2 oldPos = m_pos;
	// Note: Only position is updated, not velocity. Bouncing off the walls is handled elsewhere
//	if ((m_pos.x < minBound.x) || (m_pos.x > maxBound.x))
	{
		m_pos.x = Math::Clamp(m_pos.x, minBound.x, maxBound.x);
	}
//	if ((m_pos.y < minBound.y) || (m_pos.y > maxBound.y))
	{
		m_pos.y = Math::Clamp(m_pos.y, minBound.y, maxBound.y);
	}

	// If position changed, there was a collision
	return m_pos != oldPos;
}
