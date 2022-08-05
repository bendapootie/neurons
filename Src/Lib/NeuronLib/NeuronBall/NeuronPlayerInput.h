#pragma once

class NeuronPlayerInput
{
public:
	// Values outside this range will get clamped
	// -1.0 <= turn full left; 0.0 = straight; 1.0 >= turn full right
	float m_steering = 0.0f;
	// -1.0 <= full reverse; 0.0 = stop; 1.0 >= full forward
	float m_speed = 0.0f;
	// Value above 0.5 tries to activate boost
	float m_boost = 0.0f;
};
