#pragma once

#include "Vector.h"

class CollisionResponse;
class Circle;
class Rectangle;
namespace sf { class RenderWindow; }

class Shape
{
public:
	virtual float ComputeMass(const float density) const = 0;
	virtual CollisionResponse Collide(const Shape& other) const = 0;
	virtual CollisionResponse Collide(const Circle& other) const = 0;
	virtual CollisionResponse Collide(const Rectangle& other) const = 0;

	virtual void Draw(sf::RenderWindow& window) const = 0;

public:
	Vector2 m_pos = Vector2::Zero;
	Vector2 m_velocity = Vector2::Zero;
	float m_facing = 0.0f;
	float m_radialVelocity = 0.0f;
	float m_mass = 1.0f;
};

class Circle : public Shape
{
public:
	virtual float ComputeMass(const float density) const override;

	virtual CollisionResponse Collide(const Shape& other) const override;
	virtual CollisionResponse Collide(const Circle& other) const override;
	virtual CollisionResponse Collide(const Rectangle& other) const override;

	virtual void Draw(sf::RenderWindow& window) const override;

public:
	float m_radius = 0.0f;
};

// A facing angle of 0 radians points down the x-axis,
// so length is along the x-axis and width is along the y-axis
class Rectangle : public Shape
{
public:
	virtual float ComputeMass(const float density) const override;

	virtual CollisionResponse Collide(const Shape& other) const override;
	virtual CollisionResponse Collide(const Circle& other) const override;
	virtual CollisionResponse Collide(const Rectangle& other) const override;

	virtual void Draw(sf::RenderWindow& window) const override;

public:
	float m_halfLength = 1.0f;	// x-axis
	float m_halfWidth = 1.0f;	// y-axis
};

class CollisionResponse
{
public:
	void ApplyResponse(Shape& s0, Shape& s1) const;

public:
	const Shape* m_shape0 = nullptr;
	const Shape* m_shape1 = nullptr;
	// Distance needed to move m_shape1 so it no longer collides with m_shape0
	// Note: Because of floating point imprecision, moving m_shape0 by this distance may still result in a calculated collision
	Vector2 m_penetrationVector = Vector2::Zero;
	// Point of contact on surface of m_shape0
	Vector2 m_collisionPoint = Vector2::Zero;
	Vector2 m_collisionNormal = Vector2::UnitX;
	bool m_collided = false;
};
