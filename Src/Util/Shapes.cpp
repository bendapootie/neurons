#include "Shapes.h"

#include "crtdbg.h"	// for _ASSERT
#include <SFML/Graphics.hpp>

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

CollisionResponse Circle::Collide(const Rectangle& other) const
{
	return CollisionResponse();
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

CollisionResponse Rectangle::Collide(const Circle& other) const
{
	return CollisionResponse();
}

CollisionResponse Rectangle::Collide(const Rectangle& other) const
{
	return CollisionResponse();
}


void CollisionResponse::ApplyResponse(Circle& c0, Circle& c1) const
{
	_ASSERT(&c0 == m_shape0);
	_ASSERT(&c1 == m_shape1);

	// 1. Resolve penetration
	c1.m_pos += m_penetrationVector;

	constexpr bool k_ignoreMass = false;
	if constexpr (k_ignoreMass)
	{
		// 2. Use c0 as frame of reference for velocities
		const Vector2 c1RelativeVel = c1.m_velocity - c0.m_velocity;

		// 3. Project relative velocity onto normal and tangent
		const Vector2 collisionTangent = Vector2(m_collisionNormal.y, -m_collisionNormal.x);
		const Vector2 c1RelNorm = m_collisionNormal * c1RelativeVel.Dot(m_collisionNormal);
		const Vector2 c1RelTan = collisionTangent * c1RelativeVel.Dot(collisionTangent);

		// 4. Return to world frame of reference
		Vector2 c1Norm = c1RelNorm + c0.m_velocity;
		Vector2 c1Tan = c1RelTan + c0.m_velocity;

		// 5. Set new velocities on shapes
		c0.m_velocity = c1Norm;
		c1.m_velocity = c1Tan;
	}
	else
	{
		// Handle mass
		// Equations from https://en.wikipedia.org/wiki/Elastic_collision - Two-dimensional collision with two moving objects
		const Vector2 v0 = c0.m_velocity;
		const Vector2 v1 = c1.m_velocity;
		const float m0 = c0.m_mass;
		const float m1 = c1.m_mass;
		const Vector2 x0 = c0.m_pos;
		const Vector2 x1 = c1.m_pos;
		const Vector2 v0out = v0 - 
			(x0 - x1) * 
			((2 * m1) / (m0 + m1)) *
			((v0 - v1).Dot(x0 - x1) / (x0 - x1).GetLengthSquared());
		const Vector2 v1out = v1 - 
			(x1 - x0) * 
			((2 * m0) / (m0 + m1)) *
			((v1 - v0).Dot(x1 - x0) / (x1 - x0).GetLengthSquared());

		c0.m_velocity = v0out;
		c1.m_velocity = v1out;
	}
}
