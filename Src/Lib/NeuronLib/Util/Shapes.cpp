#include "pch.h"
#include "Shapes.h"

#include "crtdbg.h"	// for _ASSERT

// TODO: Move SFML-related code into another class (ie. shape drawer)
#include <SFML/Graphics.hpp>

void Circle::ComputeMassAndInertia(const float density)
{
	m_mass = density * k_pi * Math::Sqr(m_radius);

	// Reference for inertia calculations of common shapes...
	// https://en.wikipedia.org/wiki/List_of_moments_of_inertia
	m_inertia = 0.5f * m_mass * Math::Sqr(m_radius);
}


CollisionResponse Circle::Collide(const Shape& other) const
{
	return other.Collide(*this);
}

CollisionResponse Circle::Collide(const Circle& other) const
{
	CollisionResponse response(this, &other);

	float dist;
	const Vector2 toOtherNormal = (other.m_pos - m_pos).GetSafeNormalized(dist);
	const float penetrationDistance = (m_radius + other.m_radius) - dist;
	if (penetrationDistance > 0.0f)
	{
		response.m_penetrationVector = toOtherNormal * penetrationDistance;
		response.m_collisionPoint = m_pos + (toOtherNormal * m_radius);
		response.m_collisionNormal = toOtherNormal;
		response.m_collided = true;
	}

	return response;
}

CollisionResponse Circle::Collide(const Rectangle& rect) const
{
	CollisionResponse response(this, &rect);

	Vector2 collisionPoint = Vector2::Zero;
	Vector2 collisionNormal = Vector2::UnitX;

	// Transform circle into rectangle's space so collision detection is done centered and axis-aligned
	const Vector2 relCirclePos = (m_pos - rect.m_pos).RotateAroundOrigin(-rect.m_facing);

	// Determine whether circle should be tested against width, length, or the corner and compute penetration vector
	Vector2 absRelPenetrationVector = Vector2::Zero;
	// Do all computations in positive quadrant to avoid duplicating checks
	const Vector2 absTransformedCirclePos(Math::Abs(relCirclePos.x), Math::Abs(relCirclePos.y));
	// Note: Facing of 0 is facing right, going down the x-axis
	// TODO: If circle is fully within rectangle, the wrong axis may be chosen to calculate penetration vector
	// https://github.com/bendapootie/neurons/issues/4
	if (absTransformedCirclePos.x < rect.m_halfLength)
	{
		// Only need to check collision with top of box
		const float yAxisPenetration = rect.m_halfWidth - (absTransformedCirclePos.y - m_radius);
		// Negative penetration means there was no collision, so penetration is clamped to zero
		absRelPenetrationVector.y = -Math::Max(yAxisPenetration, 0.0f);

		collisionPoint = absTransformedCirclePos - Vector2(0.0f, m_radius);
		collisionNormal = -Vector2::UnitY;
	}
	else if (absTransformedCirclePos.y < rect.m_halfWidth)
	{
		// Only need to check collision with side of box
		const float xAxisPenetration = rect.m_halfLength - (absTransformedCirclePos.x - m_radius);
		// Negative penetration means there was no collision, so penetration is clamped to zero
		absRelPenetrationVector.x = -Math::Max(xAxisPenetration, 0.0f);

		collisionPoint = absTransformedCirclePos - Vector2(m_radius, 0.0f);
		collisionNormal = -Vector2::UnitX;
	}
	else
	{
		// Circle is closest to a corner point
		const Vector2 absRelCircleToCorner = Vector2(rect.m_halfLength, rect.m_halfWidth) - absTransformedCirclePos;
		float distanceFromCornerToCenterOfCircle;
		const Vector2 circleToCornerNormal = absRelCircleToCorner.GetSafeNormalized(distanceFromCornerToCenterOfCircle);
		const float penetrationDepth = m_radius - distanceFromCornerToCenterOfCircle;
		if (penetrationDepth > 0.0f)
		{
			absRelPenetrationVector = circleToCornerNormal * penetrationDepth;
		}

		collisionPoint = absTransformedCirclePos + (circleToCornerNormal * m_radius);
		collisionNormal = circleToCornerNormal;
	}

	// Non-zero penetration vector means there was a collision
	if (absRelPenetrationVector != Vector2::Zero)
	{
		// Transform results into correct quadrant
		const Vector2 signCorrection(Math::Sign(relCirclePos.x), Math::Sign(relCirclePos.y));

		response.m_penetrationVector = (absRelPenetrationVector * signCorrection);
		response.m_penetrationVector = response.m_penetrationVector.RotateAroundOrigin(rect.m_facing);

		response.m_collisionPoint = (collisionPoint * signCorrection);
		response.m_collisionPoint = response.m_collisionPoint.RotateAroundOrigin(rect.m_facing);
		response.m_collisionPoint += rect.m_pos;

		response.m_collisionNormal = (collisionNormal * signCorrection).RotateAroundOrigin(rect.m_facing);
		response.m_collided = true;
	}

	return response;
}

void Circle::Draw(sf::RenderWindow& window) const
{
	const float k_radiusThicknessScalar = 0.05f;

	// Draw ball
	const int r = Math::Clamp(static_cast<int>(Math::Cos(m_facing) * 256.0f), 0, 255);
	const int g = Math::Clamp(static_cast<int>(Math::Cos(m_facing + (k_2pi / 3.0f)) * 256.0f), 0, 255);
	const int b = Math::Clamp(static_cast<int>(Math::Cos(m_facing - (k_2pi / 3.0f)) * 256.0f), 0, 255);
	sf::Color fillColor(r, g, b);

	sf::CircleShape ball;
	ball.setRadius(m_radius);
	ball.setRotation(Math::RadToDeg(m_facing));
	ball.setOrigin(m_radius, m_radius);
	ball.setFillColor(fillColor);
	//ball.setOutlineColor(ballOutlineColor);
	//ball.setOutlineThickness(ballOutlineThickness);
	ball.setPosition(m_pos.x, m_pos.y);
	window.draw(ball);

	sf::RectangleShape radius;
	radius.setSize(sf::Vector2f(m_radius, m_radius * k_radiusThicknessScalar));
	radius.setRotation(Math::RadToDeg(m_facing));
	radius.setOrigin(0.0f, 0.0f);
	radius.setFillColor(sf::Color::White);
	radius.setPosition(m_pos.x, m_pos.y);
	window.draw(radius);
}

void Rectangle::ComputeMassAndInertia(const float density)
{
	m_mass = density * m_halfLength * m_halfWidth * 4.0f;

	// Reference for inertia calculations of common shapes...
	// https://en.wikipedia.org/wiki/List_of_moments_of_inertia
	m_inertia = (1.0f / 12.0f) * m_mass * (Math::Sqr(2.0f * m_halfLength) + Math::Sqr(2.0f * m_halfWidth));
}

CollisionResponse Rectangle::Collide(const Shape& other) const
{
	return other.Collide(*this);
}

CollisionResponse Rectangle::Collide(const Circle& other) const
{
	return other.Collide(*this);
}

CollisionResponse Rectangle::Collide(const Rectangle& other) const
{
	// TODO: 1. Early-out if bounding circles are far enough apart

	Array<CollisionResponse, 2> responses;

	// 2. Check CollisionResponse from transforming "other" into this's space
	// 3. Check CollisionResponse from transforming "this" into others's space
	for (int loop = 0; loop < 2; loop++)
	{
		// TODO: This is a lot of dirty syntax to allow the rest of the function use nice syntax
		const Rectangle* r0ptr;
		const Rectangle* r1ptr;
		CollisionResponse* responsePtr;
		if (loop == 0)
		{
			r0ptr = this;
			r1ptr = &other;
			responsePtr = &responses[0];
		}
		else
		{
			r0ptr = &other;
			r1ptr = this;
			responsePtr = &responses[1];
		}

		// Get references to parameters
		const Rectangle& r0 = *r0ptr;
		const Rectangle& r1 = *r1ptr;

		CollisionResponse& response = *responsePtr;
		response.m_shape0 = &r0;
		response.m_shape1 = &r1;
		// Initialize with a very large number so following code can easily find the smallest penetration vector
		response.m_penetrationVector = Vector2(Math::FloatMax, Math::FloatMax);
		// This response will only be used if there is a collision
		response.m_collided = true;

		// Transform r1's position and facing into r0's space
		const Vector2 relPos = (r1.m_pos - r0.m_pos).RotateAroundOrigin(-r0.m_facing);
		const float relFacing = r1.m_facing - r0.m_facing;

		// Get the corner points of r1 relative to r0
		const Vector2 relForward = Vector2(Math::Cos(relFacing), Math::Sin(relFacing));
		const Vector2 relRight(relForward.y, -relForward.x);
		const Vector2 relHalfLength = relForward * r1.m_halfLength;
		const Vector2 relHalfWidth = relRight * r1.m_halfWidth;
		Array<Vector2, 4> relPoints;
		relPoints[0] = Vector2(relPos + relHalfLength + relHalfWidth);
		relPoints[1] = Vector2(relPos + relHalfLength - relHalfWidth);
		relPoints[2] = Vector2(relPos - relHalfLength - relHalfWidth);
		relPoints[3] = Vector2(relPos - relHalfLength + relHalfWidth);

		// Find min and max x/y values for relative points projected onto x/y axis
		float minX = relPoints[0].x;
		float maxX = relPoints[0].x;
		float minY = relPoints[0].y;
		float maxY = relPoints[0].y;
		for (int i = 1; i < relPoints.Count(); i++)
		{
			minX = Math::Min(minX, relPoints[i].x);
			maxX = Math::Max(maxX, relPoints[i].x);
			minY = Math::Min(minY, relPoints[i].y);
			maxY = Math::Max(maxY, relPoints[i].y);
		}

		// Check if projections overlap. If they don't, early out with no collision.
		// If they do overlap, determine CollisionResponse parameters (eg. penetration, normal, etc)
		if ((minX >= r0.m_halfLength) || (maxX <= -r0.m_halfLength))
		{
			// No collision!
			return CollisionResponse(&r0, &r1);
		}
		else
		{
			// Choose the direction with the smaller penetration depth
			const float pMinX = r0.m_halfLength - minX;
			const float pMaxX = -r0.m_halfLength - maxX;
			if (Math::Abs(pMinX) <= Math::Abs(pMaxX))
			{
				// Track response with smallest penetration depth
				if (Math::Sqr(pMinX) < response.m_penetrationVector.GetLengthSquared())
				{
					response.m_penetrationVector = Vector2(pMinX, 0.0f);
					response.m_collisionNormal = Vector2::UnitX;
					// TODO: Figure out collision point
					response.m_collisionPoint = Vector2::Zero;
				}
			}
			else
			{
				// Track response with smallest penetration depth
				if (Math::Sqr(pMaxX) < response.m_penetrationVector.GetLengthSquared())
				{
					response.m_penetrationVector = Vector2(pMaxX, 0.0f);
					response.m_collisionNormal = -Vector2::UnitX;
					// TODO: Figure out collision point
					response.m_collisionPoint = Vector2::Zero;
				}
			}
		}

		// Do the same checks on the y-axis.
		// TODO: Should these code chunks be combined since the logic is essentially the same?
		if ((minY >= r0.m_halfWidth) || (maxY <= -r0.m_halfWidth))
		{
			// No collision!
			return CollisionResponse(&r0, &r1);
		}
		else
		{
			// Choose the direction with the smaller penetration depth
			const float pMinY = r0.m_halfWidth - minY;
			const float pMaxY = -r0.m_halfWidth - maxY;
			if (Math::Abs(pMinY) <= Math::Abs(pMaxY))
			{
				// Track response with smallest penetration depth
				if (Math::Sqr(pMinY) < response.m_penetrationVector.GetLengthSquared())
				{
					response.m_penetrationVector = Vector2(0.0f, pMinY);
					response.m_collisionNormal = Vector2::UnitY;
					// TODO: Figure out collision point
					response.m_collisionPoint = Vector2::Zero;
				}
			}
			else
			{
				// Track response with smallest penetration depth
				if (Math::Sqr(pMaxY) < response.m_penetrationVector.GetLengthSquared())
				{
					response.m_penetrationVector = Vector2(0.0f, pMaxY);
					response.m_collisionNormal = -Vector2::UnitY;
					// TODO: Figure out collision point
					response.m_collisionPoint = Vector2::Zero;
				}
			}
		}
	}

	// 4. Compare collision responses and return the one with shorter penetration
	// Note: Above loop would have returned early if there was not a collision
	CollisionResponse* response =
		(responses[0].m_penetrationVector.GetLengthSquared() <= responses[1].m_penetrationVector.GetLengthSquared()) ?
		&responses[0] :
		&responses[1];

	// 5. Transform response back into world-space. At this point, the vectors are all still in shape0's space
	response->m_penetrationVector = response->m_penetrationVector.RotateAroundOrigin(response->m_shape0->m_facing);

	response->m_collisionPoint = response->m_collisionPoint.RotateAroundOrigin(response->m_shape0->m_facing);
	response->m_collisionPoint += response->m_shape0->m_pos;

	response->m_collisionNormal = response->m_collisionNormal.RotateAroundOrigin(response->m_shape0->m_facing);

	_ASSERT(response->m_collided == true);
	return *response;
}

void Rectangle::Draw(sf::RenderWindow& window) const
{
	// Draw rectangle
	const int r = Math::Clamp(static_cast<int>(Math::Cos(m_facing) * 256.0f), 0, 255);
	const int g = Math::Clamp(static_cast<int>(Math::Cos(m_facing + (k_2pi / 3.0f)) * 256.0f), 0, 255);
	const int b = Math::Clamp(static_cast<int>(Math::Cos(m_facing - (k_2pi / 3.0f)) * 256.0f), 0, 255);
	sf::Color fillColor(r, g, b);

	sf::RectangleShape rect;
	rect.setSize(sf::Vector2f(2.0f * m_halfLength, 2.0f * m_halfWidth));
	rect.setRotation(Math::RadToDeg(m_facing));
	rect.setOrigin(m_halfLength, m_halfWidth);
	rect.setFillColor(fillColor);
	//rect.setOutlineColor(ballOutlineColor);
	//rect.setOutlineThickness(ballOutlineThickness);
	rect.setPosition(m_pos.x, m_pos.y);
	window.draw(rect);
}

void CollisionResponse::ApplyResponse(Shape& shape0, Shape& shape1) const
{
	// Confirm the passed in shapes are the same ones in the collision response
	// TODO: Reevaluate if passing in these parameters is more hassle than it's worth
	_ASSERT((&shape0 == m_shape0) || (&shape1 == m_shape0));
	_ASSERT((&shape0 == m_shape1) || (&shape1 == m_shape1));

	// HACKY HACK HACK!!!
	Shape& s0 = const_cast<Shape&>(*m_shape0);
	Shape& s1 = const_cast<Shape&>(*m_shape1);

	// 1. Resolve penetration
	s1.m_pos += m_penetrationVector;

	const bool k_separateAxes = false;
	if constexpr (k_separateAxes)
	{
		// Calculate velocity at collision point for each shape
		// TODO: Sanity check that m_collisionPoint is on (or very close to) the surface of each shape?
		const Vector2 v0atC = s0.GetVelocityAtWorldPos(m_collisionPoint);
		const Vector2 v1atC = s1.GetVelocityAtWorldPos(m_collisionPoint);

		// Make all calculations as though s0 is moving and s1 is stationary
		const Vector2 v0RelAtC = v0atC - v1atC;

		// Split velocity into normal and tangent to handle collision and friction separately
		const Vector2 collisionTangent(m_collisionNormal.y, -m_collisionNormal.x);
		const Vector2 vRelNorm = m_collisionNormal * v0RelAtC.Dot(m_collisionNormal);
		const Vector2 vRelTan = collisionTangent * v0RelAtC.Dot(collisionTangent);

		// Compute velocities for an elastic collision in the direction of the normal
		// If s0 is at rest, the following can be used to calculate result velocities
		// v0' = v0 * (m0 - m1) / (m0 + m1)
		// v1' = (v0 * 2 * m0) / (m0 + m1)
		const Vector2 v0RelOut = v0RelAtC * ((s0.m_mass - s1.m_mass) / (s0.m_mass + s1.m_mass));
		const Vector2 v1RelOut = v0RelAtC * ((2.0f * s0.m_mass) / (s0.m_mass + s1.m_mass));

	//	Start here to figure out rotation
		// The best example equations I've found so far...
		// https://www.euclideanspace.com/physics/dynamics/collision/twod/index.htm
	}
	else
	{
		// Only resolve collision if relative velocities are pointed at each other
		if (m_collisionNormal.Dot(s1.m_velocity - s0.m_velocity) < 0.0f)
		{
			// Handle circles with different masses
			// Equations from https://en.wikipedia.org/wiki/Elastic_collision - Two-dimensional collision with two moving objects
			const Vector2 v0 = s0.m_velocity;
			const Vector2 v1 = s1.m_velocity;
			const float m0 = s0.m_mass;
			const float m1 = s1.m_mass;
			const Vector2 x0 = s0.m_pos;
			const Vector2 x1 = s1.m_pos;
			const Vector2 v0out = v0 -
				(x0 - x1) *
				((2 * m1) / (m0 + m1)) *
				((v0 - v1).Dot(x0 - x1) / (x0 - x1).GetLengthSquared());
			const Vector2 v1out = v1 -
				(x1 - x0) *
				((2 * m0) / (m0 + m1)) *
				((v1 - v0).Dot(x1 - x0) / (x1 - x0).GetLengthSquared());

			// TODO: Add restitution here?
			s0.m_velocity = v0out;
			s1.m_velocity = v1out;
		}
	}
}
