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

		constexpr float k_defaultDensity = 1.0f;

		if (m_initialized == false)
		{
			for (Shape*& shape : m_shapes)
			{
				if (Random::NextInt(0, 2) == 1)
				{
					Circle* newCircle = new Circle();
					newCircle->m_pos = Vector2(Random::NextFloat(0.0f, k_fieldLength), Random::NextFloat(0.0f, k_fieldWidth));
					newCircle->m_velocity = Vector2(Random::NextFloat(-k_maxSpeed, k_maxSpeed), Random::NextFloat(-k_maxSpeed, k_maxSpeed));
					newCircle->m_facing = Random::NextFloat(0.0f, k_2pi);
					newCircle->m_radialVelocity = Random::NextFloat(-k_2pi, k_2pi);
					newCircle->m_radius = Random::NextFloat(k_minRadius, k_maxRadius);
					shape = newCircle;
				}
				else
				{
					Rectangle* newRectangle = new Rectangle();
					newRectangle->m_pos = Vector2(Random::NextFloat(0.0f, k_fieldLength), Random::NextFloat(0.0f, k_fieldWidth));
					newRectangle->m_velocity = Vector2(Random::NextFloat(-k_maxSpeed, k_maxSpeed), Random::NextFloat(-k_maxSpeed, k_maxSpeed));
					newRectangle->m_facing = Random::NextFloat(0.0f, k_2pi);
					newRectangle->m_radialVelocity = Random::NextFloat(-k_2pi, k_2pi);
					newRectangle->m_halfLength = Random::NextFloat(k_minRadius, k_maxRadius);
					newRectangle->m_halfWidth = Random::NextFloat(k_minRadius, k_maxRadius);
					shape = newRectangle;
				}
				// Compute mass based on area
				shape->m_mass = shape->ComputeMass(k_defaultDensity);
			}

			m_initialized = true;
		}

		// Move shapes and clamp to field
		for (Shape* shape : m_shapes)
		{
			shape->m_pos += shape->m_velocity * k_secondsPerFrame;
			shape->m_facing += shape->m_radialVelocity * k_secondsPerFrame;

			// TODO: Test the actual shape against the world bounds instead of the center point
			// Temp collision against bounds of the field
			const Vector2 minBound(0.0f, 0.0f);
			const Vector2 maxBound(k_fieldLength, k_fieldWidth);

			const Vector2 oldPos = shape->m_pos;
			shape->m_pos.x = Math::Clamp(shape->m_pos.x, minBound.x, maxBound.x);
			shape->m_pos.y = Math::Clamp(shape->m_pos.y, minBound.y, maxBound.y);

			// If position changed, there was a collision
			if (shape->m_pos.x != oldPos.x)
			{
				shape->m_velocity.x = -shape->m_velocity.x;
			}
			if (shape->m_pos.y != oldPos.y)
			{
				shape->m_velocity.y = -shape->m_velocity.y;
			}
		}

		for (int i = 0; i < m_shapes.Count(); i++)
		{
			for (int j = i + 1; j < m_shapes.Count(); j++)
			{
				Shape& c0 = *m_shapes[i];
				Shape& c1 = *m_shapes[j];
				CollisionResponse response = c0.Collide(c1);
				if (response.m_collided)
				{
					response.ApplyResponse(c1, c0);
				}
			}
		}
	}

	void DrawPhysicsTest()
	{
		sf::View view(sf::Vector2f(40.0f, 50.0f), sf::Vector2f(200.0f, 150.0f));
		m_window.setView(view);
		m_window.clear(sf::Color(0, 0, 32, 255));

		for (const Shape* shape : m_shapes)
		{
			shape->Draw(m_window);
		}

		m_window.display();
	}


private:
	sf::RenderWindow m_window;
	NeuronGame m_testGame;

	// Physics test members
	bool m_initialized = false;
	Array<Shape*, 16> m_shapes;
};

int WinMain(const int argc, const char** argv)
{
	App app;
	app.Initialize();
	int returnCode = app.Run();
	return returnCode;
}
