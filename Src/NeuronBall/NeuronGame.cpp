#include "NeuronGame.h"
#include <algorithm>
#include "Util/Constants.h"

constexpr float k_maxTurnRadiansPerSecond = DegToRad(270.0f);
constexpr float k_turningDeadZone = 0.1f;
constexpr float k_maxForwardSpeed = 30.0f;
constexpr float k_maxReverseSpeed = k_maxForwardSpeed * 0.75f;
constexpr float k_throttleDeadZone = 0.1f;
// Run at 60hz
constexpr float k_timePerTick = 1.0f / 60.0f;

void NeuronGame::Update(const NeuronPlayerInput& p0, const NeuronPlayerInput& p1)
{
	ApplyInputToPlayer(m_player0, p0);
	ApplyInputToPlayer(m_player1, p1);
}

// static
void NeuronGame::ApplyInputToPlayer(NeuronPlayer& outPlayer, const NeuronPlayerInput& input)
{
	const float clampedSteering = (fabsf(input.m_steering) <= k_turningDeadZone) ? 0.0f : std::clamp(input.m_steering, -1.0f, 1.0f);
	const float deltaAngle = (clampedSteering * k_maxTurnRadiansPerSecond) * k_timePerTick;
	float newFacing = outPlayer.m_facingRadians + deltaAngle;
	newFacing = remainderf(newFacing, k_2pi);
	outPlayer.m_facingRadians = newFacing;

	const float clampedThrottle = (fabsf(input.m_speed) <= k_throttleDeadZone) ? 0.0f : std::clamp(input.m_speed, -1.0f, 1.0f);
	const float targetSpeed = clampedThrottle * ((clampedThrottle >= 0.0f) ? k_maxForwardSpeed : k_maxReverseSpeed);
	const float vx = cosf(newFacing) * targetSpeed;
	const float vy = sinf(newFacing) * targetSpeed;
	outPlayer.m_vx = vx;
	outPlayer.m_vy = vy;

	outPlayer.m_x += vx * k_timePerTick;
	outPlayer.m_y += vy * k_timePerTick;
}
