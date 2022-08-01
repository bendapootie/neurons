#include "pch.h"
#include "NeuronBall.h"

#include "NeuronGame.h"
#include "NeuronPlayer.h"
#include "Util/Math.h"

float k_defaultBallDensity = 1.0f;

NeuronBall::NeuronBall()
{
	m_shape.m_radius = 2.0f;
	m_shape.ComputeMassAndInertia(k_defaultBallDensity);
}

NeuronBall::NeuronBall(const Vector2 pos)
{
	m_shape.m_pos = pos;
	m_shape.m_radius = 2.0f;
	m_shape.ComputeMassAndInertia(k_defaultBallDensity);
}

bool NeuronBall::CollideWithField(const NeuronGame& game, const FieldCollisionStyle style)
{
	// Collide ball against bounds of the field
	const Vector2 minBound(GetRadius(), GetRadius());
	const Vector2 maxBound(game.GetFieldLength() - GetRadius(), game.GetFieldWidth() - GetRadius());

	const Vector2 oldPos = m_shape.m_pos;
	m_shape.m_pos.x = Math::Clamp(m_shape.m_pos.x, minBound.x, maxBound.x);
	m_shape.m_pos.y = Math::Clamp(m_shape.m_pos.y, minBound.y, maxBound.y);

	// Handle bouncing off walls if requested
	if (style == FieldCollisionStyle::PushAndBounce)
	{
		// If pos.x changed, there was a collision and x-velocity needs to be flipped
		if (oldPos.x != m_shape.m_pos.x)
		{
			m_shape.m_velocity.x = -m_shape.m_velocity.x;
		}
		// Same thing for y-pos and velocity
		if (oldPos.y != m_shape.m_pos.y)
		{
			m_shape.m_velocity.y = -m_shape.m_velocity.y;
		}
	}

	// If position changed, there was a collision
	return m_shape.m_pos != oldPos;
}
