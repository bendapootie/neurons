#include "Shapes.h"

#include "crtdbg.h"	// for _ASSERT
#include <SFML/Graphics.hpp>


float Circle::ComputeMass(const float density) const
{
	return density * k_pi * Math::Sqr(m_radius);
}


CollisionResponse Circle::Collide(const Shape& other) const
{
	return other.Collide(*this);
}

CollisionResponse Circle::Collide(const Circle& other) const
{
	CollisionResponse response;
	response.m_shape0 = this;
	response.m_shape1 = &other;

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
	CollisionResponse response;
	response.m_shape0 = this;
	response.m_shape1 = &rect;

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
}

float Rectangle::ComputeMass(const float density) const
{
	return density * m_halfLength * m_halfWidth * 4.0f;
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
	return CollisionResponse();
}

void Rectangle::Draw(sf::RenderWindow& window) const
{
	// Draw rectangle
	const int r = Math::Clamp(static_cast<int>(Math::Cos(m_facing) * 256.0f), 0, 255);
	const int g = Math::Clamp(static_cast<int>(Math::Cos(m_facing + (k_2pi / 3.0f)) * 256.0f), 0, 255);
	const int b = Math::Clamp(static_cast<int>(Math::Cos(m_facing - (k_2pi / 3.0f)) * 256.0f), 0, 255);
	sf::Color fillColor(r, g, b);

	sf::RectangleShape rect;
	rect.setSize(sf::Vector2(2.0f * m_halfLength, 2.0f * m_halfWidth));
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

		s0.m_velocity = v0out;
		s1.m_velocity = v1out;
	}
}
