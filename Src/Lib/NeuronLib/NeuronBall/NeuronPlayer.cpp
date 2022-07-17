#include "pch.h"
#include "NeuronPlayer.h"

#include "NeuronGame.h"

// TODO: Make this less hacky! There's tons of edge cases where things break horribly
static Vector2 ConvexHullCollisionTest(const Array<Vector2, 4>& testPoints, const Array<Vector2, 4>& staticShape)
{
	// Check each point in "testPoints" to see if any are within the shape defined by "staticShape"
	Array<Vector2, 4> fixDistances;

	bool anyCollided = false;
	for (int j = 0; j < testPoints.Count(); j++)
	{
		const Vector2 testPoint = testPoints[j];
		bool allInside = true;
		Vector2 shortestFixDistance = Vector2(Math::FloatMax, Math::FloatMax);
		for (int i = 0; i < staticShape.Count(); i++)
		{
			const Vector2 p0 = staticShape[i];
			const Vector2 p1 = staticShape[(i + 1) % staticShape.Count()];
			const Vector2 edgeVector = (p1 - p0).GetSafeNormalized();
			const Vector2 edgeNormal = Vector2(edgeVector.y, -edgeVector.x);

			const Vector2 vectorToTestPoint = testPoint - p0;
			const float dot = edgeNormal.Dot(vectorToTestPoint);
			if (dot >= 0.0f)
			{
				allInside = false;
				break;
			}
			else
			{
				const Vector2 fixDistance = edgeNormal * dot;
				if (fixDistance.GetLengthSquared() < shortestFixDistance.GetLengthSquared())
				{
					shortestFixDistance = fixDistance;
				}
			}
		}

		anyCollided |= allInside;
		fixDistances[j] = allInside ? shortestFixDistance : Vector2::Zero;
	}

	// Return the largest fix distance. If there were no collisions, it will be ::Zero
	Vector2 fixDistance = Vector2::Zero;
	if (anyCollided)
	{
		for (const Vector2 dist : fixDistances)
		{
			if (dist.GetLengthSquared() > fixDistance.GetLengthSquared())
			{
				fixDistance = dist;
			}
		}
	}
	return fixDistance;
}

Array<Vector2, 4> NeuronPlayer::GetCornerPoints() const
{
	Array<Vector2, 4> corners;

	const Vector2 halfForward = GetForward() * 0.5f;
	const Vector2 halfRight = Vector2(halfForward.y, -halfForward.x);

	const Vector2 halfLengthVector = halfForward * GetPlayerLength();
	const Vector2 halfWidthVector = halfRight * GetPlayerWidth();

	// Compute all the corner points in counter-clockwise order
	corners[0] = Vector2(m_pos + halfLengthVector + halfWidthVector);
	corners[1] = Vector2(m_pos + halfLengthVector - halfWidthVector);
	corners[2] = Vector2(m_pos - halfLengthVector - halfWidthVector);
	corners[3] = Vector2(m_pos - halfLengthVector + halfWidthVector);

	return corners;
}

bool NeuronPlayer::CollideWithField(const NeuronGame& game)
{
	// Perf: Early-out if player is clearly not close to the walls
	const float fieldWidth = game.GetFieldWidth();
	const float fieldLength = game.GetFieldLength();

	Array<Vector2, 4> corners = GetCornerPoints();

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

	const Vector2 ballToPlayer = m_pos - ball.m_shape.m_pos;
	float distance;
	const Vector2 ballToPlayerNormal = ballToPlayer.GetSafeNormalized(distance);
	const float minDistanceAllowed = ball.GetRadius() + GetPlayerWidth();
	if (distance < minDistanceAllowed)
	{
		// Car collided with ball and needs to move away
		m_pos = ball.m_shape.m_pos + (ballToPlayerNormal * minDistanceAllowed);
		wasCollision = true;
	}
	// TODO: Should velocities change here?

	return wasCollision;
}

bool NeuronPlayer::CollidePlayers(NeuronPlayer& player0, NeuronPlayer& player1)
{
	// Do a quick distance check vs player bounding circles to try and early-out
	const float minCollisionDistance = player0.GetPlayerRadius() + player1.GetPlayerRadius();
	const float distanceSqr = player0.m_pos.GetDistanceSquared(player1.m_pos);
	if (distanceSqr >= Math::Sqr(minCollisionDistance))
	{
		return false;
	}

	const Array<Vector2, 4> corners0 = player0.GetCornerPoints();
	const Array<Vector2, 4> corners1 = player1.GetCornerPoints();

	const Vector2 fixDistance0 = ConvexHullCollisionTest(corners0, corners1);
	const Vector2 fixDistance1 = ConvexHullCollisionTest(corners1, corners0);

	if (fixDistance0.GetLengthSquared() > fixDistance1.GetLengthSquared())
	{
		player0.m_pos -= fixDistance0;
	}
	else
	{
		player1.m_pos -= fixDistance1;
	}

	return false;
}
