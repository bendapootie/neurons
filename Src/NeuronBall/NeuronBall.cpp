#include "NeuronBall.h"

#include "NeuronGame.h"
#include "NeuronPlayer.h"

void NeuronBall::CollideWithPlayer(const NeuronPlayer& player)
{
	const Vector2 playerToBall = m_pos - player.m_pos;
	float distance;
	const Vector2 playerToBallNormal = playerToBall.GetSafeNormalized(distance);
	// TODO: Compute player radius correctly
	// TODO: Treat player shape as a rectangle, not a circle
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
	}
}

void NeuronBall::CollideWithField(const NeuronGame& game)
{
	// Collide ball against bounds of the field
	const Vector2 minBound(GetRadius(), GetRadius());
	const Vector2 maxBound(game.GetFieldWidth() - GetRadius(), game.GetFieldLength() - GetRadius());

	// Note: Only position is updated, not velocity. Bouncing off the walls is handled elsewhere
	if ((m_pos.x < minBound.x) || (m_pos.x > maxBound.x))
	{
		m_pos.x = Math::Clamp(m_pos.x, minBound.x, maxBound.x);
	}
	if ((m_pos.y < minBound.y) || (m_pos.y > maxBound.y))
	{
		m_pos.y = Math::Clamp(m_pos.y, minBound.y, maxBound.y);
	}
}
