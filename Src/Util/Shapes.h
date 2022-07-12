#pragma once

#include "Vector.h"

class CollisionResponse;
class Circle;
class Rectangle;
namespace sf { class RenderWindow; }

class ShapeBase
{
public:
	Vector2 m_pos = Vector2::Zero;
	Vector2 m_velocity = Vector2::Zero;
	float m_facing = 0.0f;
	float m_radialVelocity = 0.0f;
	float m_mass = 1.0f;
};

class Circle : public ShapeBase
{
public:
	CollisionResponse Collide(const Circle& other) const;
	CollisionResponse Collide(const Rectangle& other) const;
	void Draw(sf::RenderWindow& window) const;

public:
	float m_radius = 0.0f;
};

// A facing angle of 0 radians points down the x-axis,
// so length is along the x-axis and width is along the y-axis
class Rectangle : public ShapeBase
{
public:
	CollisionResponse Collide(const Circle& other) const;
	CollisionResponse Collide(const Rectangle& other) const;

public:
	float m_halfLength = 1.0f;	// x-axis
	float m_halfWidth = 1.0f;	// y-axis
};

class CollisionResponse
{
public:
	void ApplyResponse(Circle& c0, Circle& c1) const;

public:
	const ShapeBase* m_shape0 = nullptr;
	const ShapeBase* m_shape1 = nullptr;
	// Distance needed to move m_shape1 so it no longer collides with m_shape0
	// Note: Because of floating point imprecision, moving m_shape0 by this distance may still result in a calculated collision
	Vector2 m_penetrationVector = Vector2::Zero;
	// Point of contact on surface of m_shape0
	Vector2 m_collisionPoint = Vector2::Zero;
	Vector2 m_collisionNormal = Vector2::UnitX;
	bool m_collided = false;
};
