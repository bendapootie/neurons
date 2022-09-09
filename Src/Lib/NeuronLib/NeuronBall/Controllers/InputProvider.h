#pragma once

class RefCount;

// Abstraction for keyboard, mouse, and controller input
class InputProvider
{
public:
	explicit InputProvider(const int joystickIndex, RefCount& inputBlocker);

	float GetBoost() const;
	float GetSteering() const;
	float GetSpeed() const;

private:
	RefCount& m_inputBlocker;
	const int m_joystickIndex = 0;
};
