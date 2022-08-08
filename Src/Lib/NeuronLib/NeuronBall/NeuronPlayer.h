#pragma once
#include "Util/Array.h"
#include "Util/Vector.h"


class NeuronBall;
class NeuronGame;
class Shape;

class NeuronPlayer
{
public:
	NeuronPlayer() : NeuronPlayer(Vector2::Zero, 0.0f) {}
	NeuronPlayer(const Vector2 pos, const float facing);

	Vector2 GetPos() const;
	void SetPos(const Vector2 pos);
	Vector2 GetVelocity() const;
	void SetVelocity(const Vector2 vel);
	// Returns facing in radians
	float GetFacing() const;
	// Set facing in radians
	void SetFacing(const float facing);
	Vector2 GetForward() const;
	float GetBoostRemaining() const { return m_boost; }
	Array<Vector2, 4> GetCornerPoints() const;

	bool CollideWithField(const NeuronGame& game);

	static constexpr float GetPlayerWidth() { return 4.0f; }
	static constexpr float GetPlayerLength() { return GetPlayerWidth() * 1.8f; }
	static constexpr float GetPlayerHalfWidth() { return GetPlayerWidth() * 0.5f; }
	static constexpr float GetPlayerHalfLength() { return GetPlayerLength() * 0.5f; }
	static float GetPlayerRadius() { return Math::Sqrt(Math::Sqr(GetPlayerHalfWidth()) + Math::Sqr(GetPlayerHalfLength())); }

public:
	Shape* m_shape = nullptr;
	// Available boost
	float m_boost = 0.0f;
};
