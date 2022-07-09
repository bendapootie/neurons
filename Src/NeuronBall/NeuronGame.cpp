#include "NeuronGame.h"
#include <algorithm>
#include "Util/Constants.h"

constexpr float k_maxTurnRadiansPerSecond = DegToRad(270.0f);
constexpr float k_turningDeadZone = 0.1f;
constexpr float k_maxForwardSpeed = 30.0f;
constexpr float k_maxReverseSpeed = k_maxForwardSpeed * 0.75f;
// TODO: Consider separating acceleration into forward, reverse, and braking accelerations
constexpr float k_maxAcceleration = 100.0f;
constexpr float k_throttleDeadZone = 0.1f;
// Run at 60hz
constexpr float k_timePerTick = 1.0f / 60.0f;

void NeuronGame::Update(const NeuronPlayerInput& p0, const NeuronPlayerInput& p1)
{
	ApplyInputToPlayer(m_player0, p0);
	ApplyInputToPlayer(m_player1, p1);
	UpdateBall();
	ProcessCollisions();
}

// static
void NeuronGame::ApplyInputToPlayer(NeuronPlayer& outPlayer, const NeuronPlayerInput& input)
{
	// Determine target speed
	const float clampedThrottle = (Math::Abs(input.m_speed) <= k_throttleDeadZone) ? 0.0f : Math::Clamp(input.m_speed, -1.0f, 1.0f);
	const float targetSpeed = clampedThrottle * ((clampedThrottle >= 0.0f) ? k_maxForwardSpeed : k_maxReverseSpeed);
	const Vector2 oldForward = outPlayer.ComputeForward();
	const Vector2 targetVelocity = oldForward * targetSpeed;
	const Vector2 desiredDeltaVelocity = targetVelocity - outPlayer.m_velocity;
	const float desiredDeltaVelocitySquared = desiredDeltaVelocity.GetLengthSquared();
	constexpr float k_maxAccelerationPerTick = k_maxAcceleration * k_timePerTick;
	constexpr float k_maxAccelerationPerTickSquared = k_maxAccelerationPerTick * k_maxAccelerationPerTick;
	if (desiredDeltaVelocitySquared <= k_maxAccelerationPerTickSquared)
	{
		// Acceleration is low enough to set directly
		outPlayer.m_velocity = targetVelocity;
	}
	else
	{
		// Player wants to accelerate faster than allowed. Apply the maximum allowed.
		const Vector2 directionOfAcceleration = desiredDeltaVelocity.GetNormalized();
		outPlayer.m_velocity += directionOfAcceleration * k_maxAccelerationPerTickSquared;
	}

	outPlayer.m_pos += outPlayer.m_velocity * k_timePerTick;

	const bool isMovingForward = oldForward.Dot(outPlayer.m_velocity) >= 0.0f;

	// Turn rate is limited by last update's velocity
	const float clampedSteering = (Math::Abs(input.m_steering) <= k_turningDeadZone) ? 0.0f : Math::Clamp(input.m_steering, -1.0f, 1.0f);
	const float currentSpeed = outPlayer.m_velocity.GetLength();
	const float speedPercentOfMax = currentSpeed / k_maxForwardSpeed;
	// turnThrottleScalar is based on the square of the speed percent to give it a nicer ramp
	const float turnThrottleScalar = Math::Clamp(1.0f - ((1.0f - speedPercentOfMax) * (1.0f - speedPercentOfMax)), 0.0f, 1.0f);
	const float deltaAngle = clampedSteering * k_maxTurnRadiansPerSecond * k_timePerTick * turnThrottleScalar * (isMovingForward ? 1.0f : -1.0f);
	const float newFacing = outPlayer.m_facingRadians + deltaAngle;
	// Always keep facing in the range of [0..2pi]
	outPlayer.m_facingRadians = remainderf(newFacing, k_2pi);
}

void NeuronGame::UpdateBall()
{
	// Update ball position based on velocity and velocity based on friction
	m_ball.m_pos += m_ball.m_velocity * k_timePerTick;
	m_ball.m_velocity *= (1.0f - (m_ball.GetRollingFriction() * k_timePerTick));

	// Collide ball against bounds of the field
	const Vector2 minBound(m_ball.GetRadius(), m_ball.GetRadius());
	const Vector2 maxBound(GetFieldWidth() - m_ball.GetRadius(), GetFieldLength() - m_ball.GetRadius());

	if ((m_ball.m_pos.x < minBound.x) || (m_ball.m_pos.x > maxBound.x))
	{
		m_ball.m_pos.x = Math::Clamp(m_ball.m_pos.x, minBound.x, maxBound.x);
		m_ball.m_velocity.x = -m_ball.m_velocity.x;
	}
	if ((m_ball.m_pos.y < minBound.y) || (m_ball.m_pos.y > maxBound.y))
	{
		m_ball.m_pos.y = Math::Clamp(m_ball.m_pos.y, minBound.y, maxBound.y);
		m_ball.m_velocity.y = -m_ball.m_velocity.y;
	}
}

void NeuronGame::ProcessCollisions()
{
	// Objects - Players, ball, field, goals (do goals later)
	// Iterative solver
	// 1. Check player vs field (move player)
	// 2. Check ball vs player (move ball)
	// 3. Check ball vs field (move ball)
	// 4. Check player vs ball (move player)

	// 1. Check player vs field (move player)
	for (int i = 0; i < GetNumPlayers(); i++)
	{
		GetPlayer(i).CollideWithField(*this);
	}

	// Check for ball vs car collisions
	for (int i = 0; i < GetNumPlayers(); i++)
	{
		const NeuronPlayer& player = GetPlayer(i);
		const Vector2 playerToBall = m_ball.m_pos - player.m_pos;
		float distance;
		const Vector2 playerToBallNormal = playerToBall.GetSafeNormalized(distance);
		// TODO: Compute player radius correctly
		// TODO: Treat player shape as a rectangle, not a circle
		const float minDistanceAllowed = m_ball.GetRadius() + player.GetPlayerWidth();
		if (distance < minDistanceAllowed)
		{
			// Ball collided with car and needs to move away
			m_ball.m_pos = player.m_pos + (playerToBallNormal * minDistanceAllowed);

			// TODO: This isn't the right way to apply velocity to the ball
			const float velocityDot = playerToBallNormal.Dot(player.m_velocity.GetSafeNormalized());
			if (velocityDot > 0.0f)
			{
				m_ball.m_velocity = player.m_velocity * velocityDot;
			}
		}
	}
}