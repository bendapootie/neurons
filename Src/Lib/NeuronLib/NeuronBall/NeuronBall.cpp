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

bool NeuronBall::CollideWithField(const NeuronGame& game)
{
	// Collide ball against bounds of the field
	const Vector2 minBound(GetRadius(), GetRadius());
	const Vector2 maxBound(game.GetFieldLength() - GetRadius(), game.GetFieldWidth() - GetRadius());

	const Vector2 oldPos = m_shape.m_pos;
	// Note: Only position is updated, not velocity. Bouncing off the walls is handled elsewhere
//	if ((m_pos.x < minBound.x) || (m_pos.x > maxBound.x))
	{
		m_shape.m_pos.x = Math::Clamp(m_shape.m_pos.x, minBound.x, maxBound.x);
	}
//	if ((m_pos.y < minBound.y) || (m_pos.y > maxBound.y))
	{
		m_shape.m_pos.y = Math::Clamp(m_shape.m_pos.y, minBound.y, maxBound.y);
	}

	// If position changed, there was a collision
	return m_shape.m_pos != oldPos;
}
