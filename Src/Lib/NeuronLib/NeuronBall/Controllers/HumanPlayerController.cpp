#include "pch.h"
#include "HumanPlayerController.h"

#include "NeuronBall/NeuronPlayerInput.h"
#include <SFML/Window/Joystick.hpp>
#include <SFML/Window/Keyboard.hpp>
#include "Util/Math.h"

HumanPlayerController::HumanPlayerController(const int joystickIndex) :
	m_joystickIndex(joystickIndex)
{
}

void HumanPlayerController::GetInputFromGameState(NeuronPlayerInput& outPlayerInput, const NeuronGame& game)
{
	// Sample Input
	if (sf::Joystick::isConnected(m_joystickIndex))
	{
		float x = Math::Clamp(sf::Joystick::getAxisPosition(m_joystickIndex, sf::Joystick::X) * 0.01f, -1.0f, 1.0f);
		outPlayerInput.m_steering = (x * x) * ((x >= 0.0f) ? 1.0f : -1.0f);
		outPlayerInput.m_speed = -sf::Joystick::getAxisPosition(m_joystickIndex, sf::Joystick::Z) * 0.01f;
		outPlayerInput.m_boost = sf::Joystick::isButtonPressed(m_joystickIndex, 0);
	}

	if (m_joystickIndex == 0)
	{
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
		{
			outPlayerInput.m_speed = 1.0f;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
		{
			outPlayerInput.m_speed = -1.0f;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
		{
			outPlayerInput.m_steering = -1.0f;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
		{
			outPlayerInput.m_steering = 1.0f;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
		{
			outPlayerInput.m_boost = 1.0f;
		}
	}
	else if (m_joystickIndex == 1)
	{
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
		{
			outPlayerInput.m_speed = 1.0f;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
		{
			outPlayerInput.m_speed = -1.0f;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
		{
			outPlayerInput.m_steering = -1.0f;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
		{
			outPlayerInput.m_steering = 1.0f;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::RControl))
		{
			outPlayerInput.m_boost = 1.0f;
		}
	}

}
