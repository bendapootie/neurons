#include "pch.h"
#include "NeuronPlayer.h"

#include "NeuronGame.h"

float k_defaultPlayerDensity = 1.0f;


NeuronPlayer::NeuronPlayer(const Vector2 pos, const float facing)
{
	Rectangle* rect = new Rectangle();
	rect->m_pos = pos;
	rect->m_velocity = Vector2::Zero;
	rect->m_facing = facing;
	rect->m_angularVelocity = 0.0f;
	rect->m_halfLength = GetPlayerHalfLength();
	rect->m_halfWidth = GetPlayerHalfWidth();
	rect->ComputeMassAndInertia(k_defaultPlayerDensity);

	m_shape = rect;
}

Vector2 NeuronPlayer::GetPos() const
{
	return m_shape->m_pos;
}

void NeuronPlayer::SetPos(const Vector2 pos)
{
	m_shape->m_pos = pos;
}

Vector2 NeuronPlayer::GetVelocity() const
{
	return m_shape->m_velocity;
}

void NeuronPlayer::SetVelocity(const Vector2 vel)
{
	m_shape->m_velocity = vel;
}

float NeuronPlayer::GetFacing() const
{
	return m_shape->m_facing;
}

void NeuronPlayer::SetFacing(const float facing)
{
	m_shape->m_facing = facing;
}

Vector2 NeuronPlayer::GetForward() const
{
	return Vector2(Math::Cos(m_shape->m_facing), Math::Sin(m_shape->m_facing));
}

Array<Vector2, 4> NeuronPlayer::GetCornerPoints() const
{
	Array<Vector2, 4> corners;

	const Vector2 halfForward = GetForward() * 0.5f;
	const Vector2 halfRight = Vector2(halfForward.y, -halfForward.x);

	const Vector2 halfLengthVector = halfForward * GetPlayerLength();
	const Vector2 halfWidthVector = halfRight * GetPlayerWidth();

	// Compute all the corner points in counter-clockwise order
	corners[0] = Vector2(m_shape->m_pos + halfLengthVector + halfWidthVector);
	corners[1] = Vector2(m_shape->m_pos + halfLengthVector - halfWidthVector);
	corners[2] = Vector2(m_shape->m_pos - halfLengthVector - halfWidthVector);
	corners[3] = Vector2(m_shape->m_pos - halfLengthVector + halfWidthVector);

	return corners;
}

bool NeuronPlayer::CollideWithField(const NeuronGame& game)
{
	// Perf: Early-out if player is clearly not close to the walls
	const float fieldLength = game.GetFieldLength();
	const float fieldWidth = game.GetFieldWidth();

	Array<Vector2, 4> corners = GetCornerPoints();

	Vector2 pushDistance = Vector2::Zero;

	// Check bounds of each corner
	for (int i = 0; i < 4; i++)
	{
		if (corners[i].x < 0)
		{
			pushDistance.x = Math::Max(pushDistance.x, -corners[i].x);
		}
		if (corners[i].x > fieldLength)
		{
			pushDistance.x = Math::Min(pushDistance.x, fieldLength - corners[i].x);
		}
		if (corners[i].y < 0)
		{
			pushDistance.y = Math::Max(pushDistance.y, -corners[i].y);
		}
		if (corners[i].y > fieldWidth)
		{
			pushDistance.y = Math::Min(pushDistance.y, fieldWidth - corners[i].y);
		}
	}

	// Move the player back by how much it was over the line
	m_shape->m_pos += pushDistance;

	// If pushDistance isn't zero, there was a collision
	return pushDistance != Vector2::Zero;
}
