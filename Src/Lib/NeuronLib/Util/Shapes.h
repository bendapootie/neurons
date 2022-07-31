#pragma once

#include "Vector.h"
#include "Array.h"

class CollisionResponse;
class Circle;
class Rectangle;
namespace sf { class RenderWindow; }
namespace sf { class Color; }

class Shape
{
public:
	// Computes and sets mass and inertia given the provided density
	// Since these values are dependent on the exact dimensions of the shape,
	// this function should only be called after the shape's form has been set.
	virtual void ComputeMassAndInertia(const float density) = 0;

	virtual CollisionResponse Collide(const Shape& other) const = 0;
	virtual CollisionResponse Collide(const Circle& other) const = 0;
	virtual CollisionResponse Collide(const Rectangle& other) const = 0;

	virtual void Draw(sf::RenderWindow& window) const = 0;
	virtual void Draw(
		sf::RenderWindow& window,
		const sf::Color& fillColor,
		const sf::Color& outlineColor,
		const float outlineThickness
	) const = 0;

	Vector2 GetPos() const
	{
		return m_pos;
	}
	Vector2& GetPos()
	{
		return m_pos;
	}

	Vector2 GetVelocity() const
	{
		return m_velocity;
	}
	void SetVelocity(const Vector2 vel)
	{
		m_velocity = vel;
	}

	float GetFacing() const
	{
		return m_facing;
	}

	Vector2 GetForward() const
	{
		return Vector2(Math::Cos(m_facing), Math::Sin(m_facing));
	}

	// Returns the linear velocity of the shape at a given world position.
	// If angular velocity is zero, this will be the same as m_velocity.
	// If it's not zero, the instantaneous linear velocity contributed by rotation will be accounted for.
	Vector2 GetVelocityAtWorldPos(const Vector2& testPos) const
	{
		return m_velocity + (testPos - m_pos) * m_angularVelocity;
	}

	// Calculates and returns linear kinetic energy (KE = 0.5 * mass * velocity^2)
	// Units are kg*m^2/s^2
	float GetLinearKineticEnergy() const
	{
		return 0.5f * m_mass * m_velocity.GetLengthSquared();
	}

	// Rotational kinetic energy (RE = 0.5 * inertia * angularVelocity^2)
	// Units are kg*m^2/s^2
	float GetRotationalKineticEnergy() const
	{
		return 0.5f * m_inertia * Math::Sqr(m_angularVelocity);
	}

	float GetTotalKineticEnergy() const
	{
		return GetLinearKineticEnergy() + GetRotationalKineticEnergy();
	}

public:
	// m
	Vector2 m_pos = Vector2::Zero;
	// m/s
	Vector2 m_velocity = Vector2::Zero;
	// kg
	float m_mass = 0.0f;

	// radians
	float m_facing = 0.0f;
	// radians/s
	float m_angularVelocity = 0.0f;
	// kg*m^2
	float m_inertia = 0.0f;
};

class Circle : public Shape
{
public:
	virtual void ComputeMassAndInertia(const float density) override;

	virtual CollisionResponse Collide(const Shape& other) const override;
	virtual CollisionResponse Collide(const Circle& other) const override;
	virtual CollisionResponse Collide(const Rectangle& other) const override;

	virtual void Draw(sf::RenderWindow& window) const override;
	virtual void Draw(
		sf::RenderWindow& window,
		const sf::Color& fillColor,
		const sf::Color& outlineColor,
		const float outlineThickness
	) const override;

public:
	float m_radius = 0.0f;
};

// A facing angle of 0 radians points down the x-axis,
// so length is along the x-axis and width is along the y-axis
class Rectangle : public Shape
{
public:
	virtual void ComputeMassAndInertia(const float density) override;

	virtual CollisionResponse Collide(const Shape& other) const override;
	virtual CollisionResponse Collide(const Circle& other) const override;
	virtual CollisionResponse Collide(const Rectangle& other) const override;

	virtual void Draw(sf::RenderWindow& window) const override;
	virtual void Draw(
		sf::RenderWindow& window,
		const sf::Color& fillColor,
		const sf::Color& outlineColor,
		const float outlineThickness
	) const override;

protected:
	Array<Vector2, 4> GetCornerPoints() const;

public:
	float m_halfLength = 0.0f;	// x-axis
	float m_halfWidth = 0.0f;	// y-axis
};

class CollisionResponse
{
public:
	CollisionResponse() {}
	CollisionResponse(const Shape* s0, const Shape* s1) :
		m_shape0(s0),
		m_shape1(s1)
	{
	}

	void ApplyResponse(Shape& s0, Shape& s1) const;

public:
	const Shape* m_shape0 = nullptr;
	const Shape* m_shape1 = nullptr;
	// Distance needed to move m_shape1 so it no longer collides with m_shape0
	// Note: Because of floating point imprecision, moving m_shape1 by this distance may still result in a calculated collision
	Vector2 m_penetrationVector = Vector2::Zero;
	// Point of contact on surface of m_shape0
	Vector2 m_collisionPoint = Vector2::Zero;
	Vector2 m_collisionNormal = Vector2::UnitX;
	bool m_collided = false;
};
