#include "pch.h"
#include "NeuronGame.h"

#include <algorithm>
#include "Util/Constants.h"

constexpr float k_defaultGameDuration = 60.0f;
constexpr float k_playerWidthOffsetPercent = 0.04f;

constexpr float k_maxTurnRadiansPerSecond = DegToRad(270.0f);
constexpr float k_turningDeadZone = 0.1f;
constexpr float k_maxForwardSpeed = 30.0f;
constexpr float k_maxBoostedSpeed = k_maxForwardSpeed * 1.75f;
constexpr float k_maxReverseSpeed = k_maxForwardSpeed * 0.75f;
// TODO: Consider separating acceleration into forward, reverse, and braking accelerations
constexpr float k_maxAcceleration = 100.0f;
constexpr float k_throttleDeadZone = 0.1f;
// Run at 60hz
constexpr float k_timePerTick = 1.0f / 60.0f;

NeuronGame::NeuronGame()
{
	ResetField();
	m_score[0] = 0;
	m_score[1] = 0;
	m_timeRemaining = k_defaultGameDuration;
}


void NeuronGame::Update(const NeuronPlayerInput& p0, const NeuronPlayerInput& p1)
{
	ApplyInputToPlayer(m_player0, p0);
	ApplyInputToPlayer(m_player1, p1);
	UpdateBall();
	ProcessCollisions();
	CheckForGoal();
	m_timeRemaining -= k_timePerTick;
}

void NeuronGame::ResetField()
{
	m_player0.SetPos(Vector2(m_fieldLength * 0.1f, m_fieldWidth * (0.5f - k_playerWidthOffsetPercent)));
	m_player0.SetFacing(DegToRad(0.0f));
	m_player0.SetVelocity(Vector2::Zero);

	m_player1.SetPos(Vector2(m_fieldLength * 0.9f, m_fieldWidth * (0.5f + k_playerWidthOffsetPercent)));
	m_player1.SetFacing(DegToRad(180.0f));
	m_player1.SetVelocity(Vector2::Zero);

	m_ball.m_shape.SetPos(Vector2(m_fieldLength * 0.5f, m_fieldWidth * 0.5f));
	m_ball.m_shape.SetVelocity(Vector2::Zero);
}

// static
void NeuronGame::ApplyInputToPlayer(NeuronPlayer& outPlayer, const NeuronPlayerInput& input)
{
	// Determine target speed
	const float clampedThrottle = (Math::Abs(input.m_speed) <= k_throttleDeadZone) ? 0.0f : Math::Clamp(input.m_speed, -1.0f, 1.0f);
	const float targetSpeed = (input.m_boost >= 0.5f) ? k_maxBoostedSpeed : (clampedThrottle * ((clampedThrottle >= 0.0f) ? k_maxForwardSpeed : k_maxReverseSpeed));
	const Vector2 oldForward = outPlayer.GetForward();
	const Vector2 targetVelocity = oldForward * targetSpeed;
	const Vector2 desiredDeltaVelocity = targetVelocity - outPlayer.GetVelocity();
	const float desiredDeltaVelocitySquared = desiredDeltaVelocity.GetLengthSquared();
	constexpr float k_maxAccelerationPerTick = k_maxAcceleration * k_timePerTick;
	constexpr float k_maxAccelerationPerTickSquared = k_maxAccelerationPerTick * k_maxAccelerationPerTick;
	if (desiredDeltaVelocitySquared <= k_maxAccelerationPerTickSquared)
	{
		// Acceleration is low enough to set directly
		outPlayer.SetVelocity(targetVelocity);
	}
	else
	{
		// Player wants to accelerate faster than allowed. Apply the maximum allowed.
		const Vector2 directionOfAcceleration = desiredDeltaVelocity.GetNormalized();
		outPlayer.SetVelocity(outPlayer.GetVelocity() + directionOfAcceleration * k_maxAccelerationPerTickSquared);
	}

	outPlayer.SetPos(outPlayer.GetPos() + outPlayer.GetVelocity() * k_timePerTick);

	const bool isMovingForward = oldForward.Dot(outPlayer.GetVelocity()) >= 0.0f;

	// Turn rate is limited by last update's velocity
	const float clampedSteering = (Math::Abs(input.m_steering) <= k_turningDeadZone) ? 0.0f : Math::Clamp(input.m_steering, -1.0f, 1.0f);
	const float currentSpeed = outPlayer.GetVelocity().GetLength();
	const float speedPercentOfMax = currentSpeed / k_maxForwardSpeed;
	// turnThrottleScalar is based on the square of the speed percent to give it a nicer ramp
	const float turnThrottleScalar = Math::Clamp(1.0f - ((1.0f - speedPercentOfMax) * (1.0f - speedPercentOfMax)), 0.0f, 1.0f);
	const float deltaAngle = clampedSteering * k_maxTurnRadiansPerSecond * k_timePerTick * turnThrottleScalar * (isMovingForward ? 1.0f : -1.0f);
	const float newFacing = outPlayer.GetFacing() + deltaAngle;
	// Always keep facing in the range of [0..2pi]
	outPlayer.SetFacing(remainderf(newFacing, k_2pi));
}

void NeuronGame::UpdateBall()
{
	// Update ball position based on velocity and velocity based on friction
	m_ball.m_shape.m_pos += m_ball.m_shape.GetVelocity() * k_timePerTick;
	m_ball.m_shape.SetVelocity(m_ball.m_shape.GetVelocity() * (1.0f - (m_ball.GetRollingFriction() * k_timePerTick)));

	// Collide ball against bounds of the field, bouncing off walls if hit
	m_ball.CollideWithField(*this, FieldCollisionStyle::PushAndBounce);
}

void NeuronGame::ProcessCollisions()
{
	// Objects - Players, ball, field, goals (do goals later)
	// Iterative solver
	// 1. Check player vs field (move player)
	// 2. Check ball vs player (move ball)
	// 3. Check ball vs field (move ball)
	// 4. Check player vs ball (move player)
	// 5. Check player vs player (move both)

	// TODO: Alternate order players are processed each frame to maintain fairness? Would that matter?
	// 1. Check player vs field (move player)
	for (int i = 0; i < GetNumPlayers(); i++)
	{
		GetPlayer(i).CollideWithField(*this);
	}

	// 2. Check ball vs player
	for (int i = 0; i < GetNumPlayers(); i++)
	{
		Shape* ballShape = &m_ball.m_shape;
		Shape* playerShape = GetPlayer(i).m_shape;
		CollisionResponse response = ballShape->Collide(*playerShape);
		if (response.m_collided)
		{
			response.ApplyResponse(*ballShape, *playerShape);
		}
	}

	// 3. Check ball vs field (move ball)
	m_ball.CollideWithField(*this, FieldCollisionStyle::PushOnly);

	// 5. Check player vs player (move both)
	{
		Shape* p0Shape = GetPlayer(0).m_shape;
		Shape* p1Shape = GetPlayer(1).m_shape;
		CollisionResponse response = p0Shape->Collide(*p1Shape);
		if (response.m_collided)
		{
			response.ApplyResponse(*p0Shape, *p1Shape);
		}
	}
}

void NeuronGame::CheckForGoal()
{
	const float k_epsilon = Math::FloatSmallNumber;
	const float radius = m_ball.GetRadius();
	const Vector2 pos = m_ball.m_shape.GetPos();
	const float minGoalWidth = (GetFieldWidth() - GetGoalWidth()) * 0.5f;
	const float maxGoalWidth = (GetFieldWidth() + GetGoalWidth()) * 0.5f;

	if ((pos.y >= minGoalWidth) && (pos.y <= maxGoalWidth))
	{
		// Check for goals
		if (pos.x <= radius + k_epsilon)
		{
			// GOAL!!!!!
			ScoreForPlayerIndex(1);
		}

		if (pos.x >= (GetFieldLength() - (radius + k_epsilon)))
		{
			// GOAL!!!!!
			ScoreForPlayerIndex(0);
		}
	}
}

void NeuronGame::ScoreForPlayerIndex(const int playerIndex)
{
	m_score[playerIndex]++;
	ResetField();
}
