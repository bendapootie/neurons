// 
// Main.cpp
//
// This is the entry point for the application. This file is a simple, thin
// wrapper for creating the App object and handling command line parameters
//

#include <SFML/Graphics.hpp>

// TODO: These are just included for testing. Remove them when not needed anymore.
#include "NeuronBall/NeuronGame.h"
#include "NeuronBall/NeuronGameDisplay.h"
#include "Util/Random.h"
#include "Util/Shapes.h"

class App
{
public:
	App() {}
	~App() {}

	void Initialize()
	{
		m_window.create(sf::VideoMode(800, 600), "Neurons");
		m_window.setVerticalSyncEnabled(true);
	}

	int Run()
	{
		int returnCode = 0;
		while (m_window.isOpen())
		{
			sf::Event event;
			while (m_window.pollEvent(event))
			{
				if (event.type == sf::Event::Closed)
				{
					m_window.close();
				}
			}

			if (m_window.isOpen())
			{
				//UpdateGame();
				//DrawGame();
				UpdatePhysicsTest();
				DrawPhysicsTest();
			}
		}
		return returnCode;
	}

private:
	void UpdateGame()
	{
		NeuronPlayerInput input[2];

		for (int i = 0; i < 2; i++)
		{
			// Sample Input
			if (sf::Joystick::isConnected(i))
			{
				float x = std::clamp(sf::Joystick::getAxisPosition(i, sf::Joystick::X) * 0.01f, -1.0f, 1.0f);
				input[i].m_steering = (x * x) * ((x >= 0.0f) ? 1.0f : -1.0f);
				input[i].m_speed = -sf::Joystick::getAxisPosition(i, sf::Joystick::Z) * 0.01f;
			}
		}

		// Update game with input
		m_testGame.Update(input[0], input[1]);
	}

	void DrawGame()
	{
		sf::View view(sf::Vector2f(50.0f, 40.0f), sf::Vector2f(200.0f, 150.0f));
		m_window.setView(view);
		m_window.clear(sf::Color(0, 0, 32, 255));
		
		NeuronGameDisplay gameDisplay(m_testGame);
		gameDisplay.Draw(m_window);

		m_window.display();
	}

	void UpdatePhysicsTest()
	{
		constexpr float k_fieldLength = 100.0f;
		constexpr float k_fieldWidth = 80.0f;
		constexpr float k_secondsPerFrame = 1.0f / 60.0f;

		constexpr float k_minRadius = 1.0f;
		constexpr float k_maxRadius = 5.0f;
		constexpr float k_maxSpeed = 30.0f;

		if (m_initialized == false)
		{
			for (Circle& circle : m_shapes)
			{
				circle.m_pos = Vector2(Random::NextFloat(0.0f, k_fieldLength), Random::NextFloat(0.0f, k_fieldWidth));
				circle.m_velocity = Vector2(Random::NextFloat(-k_maxSpeed, k_maxSpeed), Random::NextFloat(-k_maxSpeed, k_maxSpeed));
				circle.m_facing = Random::NextFloat(0.0f, k_2pi);
				circle.m_radialVelocity = Random::NextFloat(-k_2pi, k_2pi);
				circle.m_radius = Random::NextFloat(k_minRadius, k_maxRadius);
			}
			m_initialized = true;
		}

		// Move circles and clamp to field
		for (Circle& circle : m_shapes)
		{
			circle.m_pos += circle.m_velocity * k_secondsPerFrame;
			circle.m_facing += circle.m_radialVelocity * k_secondsPerFrame;

			// Temp collision against bounds of the field
			const Vector2 minBound(circle.m_radius, circle.m_radius);
			const Vector2 maxBound(k_fieldLength - circle.m_radius, k_fieldWidth - circle.m_radius);

			const Vector2 oldPos = circle.m_pos;
			circle.m_pos.x = Math::Clamp(circle.m_pos.x, minBound.x, maxBound.x);
			circle.m_pos.y = Math::Clamp(circle.m_pos.y, minBound.y, maxBound.y);

			// If position changed, there was a collision
			if (circle.m_pos.x != oldPos.x)
			{
				circle.m_velocity.x = -circle.m_velocity.x;
			}
			if (circle.m_pos.y != oldPos.y)
			{
				circle.m_velocity.y = -circle.m_velocity.y;
			}
		}

		for (int i = 0; i < m_shapes.Count(); i++)
		{
			for (int j = i + 1; j < m_shapes.Count(); j++)
			{
				Circle& c0 = m_shapes[i];
				Circle& c1 = m_shapes[j];
				CollisionResponse response = c0.Collide(c1);
				if (response.m_collided)
				{
					response.ApplyResponse(c0, c1);
				}
			}
		}
	}

	void DrawPhysicsTest()
	{
		sf::View view(sf::Vector2f(40.0f, 50.0f), sf::Vector2f(200.0f, 150.0f));
		m_window.setView(view);
		m_window.clear(sf::Color(0, 0, 32, 255));

		for (Circle& circle : m_shapes)
		{
			circle.Draw(m_window);
		}

		m_window.display();
	}


private:
	sf::RenderWindow m_window;
	NeuronGame m_testGame;

	// Physics test members
	bool m_initialized = false;
	Array<Circle, 16> m_shapes;
};

int WinMain(const int argc, const char** argv)
{
	App app;
	app.Initialize();
	int returnCode = app.Run();
	return returnCode;
}
