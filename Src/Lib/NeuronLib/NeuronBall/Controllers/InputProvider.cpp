#include "pch.h"
#include "InputProvider.h"

#include <SFML/Window/Joystick.hpp>		// sf::Joystick
#include <SFML/Window/Keyboard.hpp>		// sf::Keyboard
#include "Util/Math.h"
#include "Util/RefCount.h"
#include <vector>

const int k_joystickBoostButtonIndex = 0;

enum class InputBinding
{
	Forward,
	Backward,
	TurnLeft,
	TurnRight,
	Boost,
	Count
};

const std::vector<sf::Keyboard::Key> k_playerZeroKeys =
{
	sf::Keyboard::W,
	sf::Keyboard::S,
	sf::Keyboard::A,
	sf::Keyboard::D,
	sf::Keyboard::Space
};

const std::vector<sf::Keyboard::Key> k_playerOneKeys =
{
	sf::Keyboard::Up,
	sf::Keyboard::Down,
	sf::Keyboard::Left,
	sf::Keyboard::Right,
	sf::Keyboard::RControl
};

// TODO: Make this the same as k_numPlayers
const std::vector<const std::vector<sf::Keyboard::Key>*> k_playerKeyboardInputMappings =
{
	&k_playerZeroKeys,
	&k_playerOneKeys
};

namespace
{
	// Helper function to select the correct array values from the vector data
	bool IsKeyPressed(int playerIndex, InputBinding input)
	{
		const sf::Keyboard::Key inputKey = (*k_playerKeyboardInputMappings[playerIndex])[static_cast<int>(input)];
		return sf::Keyboard::isKeyPressed(inputKey);
	}
}

InputProvider::InputProvider(const int joystickIndex, RefCount& inputBlocker) :
	m_joystickIndex(joystickIndex),
	m_inputBlocker(inputBlocker)
{
	// Sanity check static data
	_ASSERT(k_playerZeroKeys.size() == (int)InputBinding::Count);
	_ASSERT(k_playerOneKeys.size() == (int)InputBinding::Count);
	// TODO: Compare this to "k_numPlayers"
	_ASSERT(k_playerKeyboardInputMappings.size() == 2);

	// TODO: Do something more intelligent if the joystick isn't connected?
	sf::Joystick::isConnected(m_joystickIndex);
}

float InputProvider::GetBoost() const
{
	float boost = 0.0f;
	if (m_inputBlocker.IsFree())
	{
		if (sf::Joystick::isConnected(m_joystickIndex))
		{
			boost = sf::Joystick::isButtonPressed(m_joystickIndex, k_joystickBoostButtonIndex);
		}

		if (IsKeyPressed(m_joystickIndex, InputBinding::Boost))
		{
			boost = 1.0f;
		}
	}

	return Math::Clamp(boost, 0.0f, 1.0f);
}

float InputProvider::GetSteering() const
{
	float steering = 0.0f;
	if (m_inputBlocker.IsFree())
	{
		if (sf::Joystick::isConnected(m_joystickIndex))
		{
			float x = Math::Clamp(sf::Joystick::getAxisPosition(m_joystickIndex, sf::Joystick::X) * 0.01f, -1.0f, 1.0f);
			steering += (x * x) * ((x >= 0.0f) ? 1.0f : -1.0f);
		}

		if (IsKeyPressed(m_joystickIndex, InputBinding::TurnLeft))
		{
			steering -= 1.0f;
		}
		if (IsKeyPressed(m_joystickIndex, InputBinding::TurnRight))
		{
			steering += 1.0f;
		}
	}

	return Math::Clamp(steering, -1.0f, 1.0f);
}

float InputProvider::GetSpeed() const
{
	float speed = 0.0f;
	if (m_inputBlocker.IsFree())
	{

		if (sf::Joystick::isConnected(m_joystickIndex))
		{
			speed = -sf::Joystick::getAxisPosition(m_joystickIndex, sf::Joystick::Z) * 0.01f;
		}

		if (IsKeyPressed(m_joystickIndex, InputBinding::Forward))
		{
			speed += 1.0f;
		}
		if (IsKeyPressed(m_joystickIndex, InputBinding::Backward))
		{
			speed -= 1.0f;
		}
	}

	return Math::Clamp(speed, -1.0f, 1.0f);
}
