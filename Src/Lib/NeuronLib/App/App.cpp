#include "pch.h"
#include "App.h"

#include <SFML/Graphics.hpp>

// TODO: These are just included for testing. Remove them when not needed anymore.
#include "NeuronBall/NeuronGame.h"
#include "NeuronBall/NeuronGameDisplay.h"
#include "Util/Random.h"
#include "Util/Shapes.h"


constexpr bool k_playGame = true;
constexpr bool k_randomInit = false;


void App::Initialize()
{
	m_window.create(sf::VideoMode(800, 450), "Neurons");
	m_window.setVerticalSyncEnabled(true);
}

int App::Run()
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
			if (k_playGame)
			{
				UpdateGame();
				DrawGame();
			}
			else
			{
				UpdatePhysicsTest();
				DrawPhysicsTest();
			}
		}
	}
	return returnCode;
}

void App::UpdateGame()
{
	NeuronPlayerInput input[2];

	for (int i = 0; i < 2; i++)
	{
		// Sample Input
		if (sf::Joystick::isConnected(i))
		{
			float x = Math::Clamp(sf::Joystick::getAxisPosition(i, sf::Joystick::X) * 0.01f, -1.0f, 1.0f);
			input[i].m_steering = (x * x) * ((x >= 0.0f) ? 1.0f : -1.0f);
			input[i].m_speed = -sf::Joystick::getAxisPosition(i, sf::Joystick::Z) * 0.01f;
			input[i].m_boost = sf::Joystick::isButtonPressed(i, 0);
		}
	}

	// Update game with input
	m_testGame.Update(input[0], input[1]);
}

void App::DrawGame()
{
	sf::View view(sf::Vector2f(40.0f, 50.0f), sf::Vector2f(240.0f, 135.0f));
	m_window.setView(view);
	m_window.clear(sf::Color(0, 0, 32, 255));

	NeuronGameDisplay gameDisplay(m_testGame);
	gameDisplay.Draw(m_window);

	m_window.display();
}

void App::UpdatePhysicsTest()
{
	constexpr float k_fieldLength = 160.0f;
	constexpr float k_fieldWidth = 90.0f;
	constexpr float k_secondsPerFrame = 1.0f / 60.0f;

	constexpr Vector2 k_gravity(0.0f, 0.0f);

	constexpr float k_minRadius = 1.0f;
	constexpr float k_maxRadius = 5.0f;
	constexpr float k_maxSpeed = 30.0f;

	constexpr float k_maxCircleRotationalVelocity = Math::DegToRad(180.0f);
	constexpr float k_maxRectRotationalVelocity = Math::DegToRad(90.0f);

	constexpr float k_defaultDensity = 1.0f;

	if (m_initialized == false)
	{
		m_shapes.Zero();

		if constexpr (k_randomInit)
		{
			for (Shape*& shape : m_shapes)
			{
				if (Random::NextInt(0, 2) == 1)
				{
					Circle* newCircle = new Circle();
					newCircle->m_pos = Vector2(Random::NextFloat(0.0f, k_fieldLength), Random::NextFloat(0.0f, k_fieldWidth));
					newCircle->m_velocity = Vector2(Random::NextFloat(-k_maxSpeed, k_maxSpeed), Random::NextFloat(-k_maxSpeed, k_maxSpeed));
					newCircle->m_facing = Random::NextFloat(0.0f, k_2pi);
					newCircle->m_angularVelocity = Random::NextFloat(-k_maxCircleRotationalVelocity, k_maxCircleRotationalVelocity);
					newCircle->m_radius = Random::NextFloat(k_minRadius, k_maxRadius);
					shape = newCircle;
				}
				else
				{
					Rectangle* newRectangle = new Rectangle();
					newRectangle->m_pos = Vector2(Random::NextFloat(0.0f, k_fieldLength), Random::NextFloat(0.0f, k_fieldWidth));
					newRectangle->m_velocity = Vector2(Random::NextFloat(-k_maxSpeed, k_maxSpeed), Random::NextFloat(-k_maxSpeed, k_maxSpeed));
					newRectangle->m_facing = Random::NextFloat(0.0f, k_2pi);
					newRectangle->m_angularVelocity = Random::NextFloat(-k_maxRectRotationalVelocity, k_maxRectRotationalVelocity);
					newRectangle->m_halfLength = Random::NextFloat(k_minRadius, k_maxRadius);
					newRectangle->m_halfWidth = Random::NextFloat(k_minRadius, k_maxRadius);
					shape = newRectangle;
				}
				// Compute mass and inertia based on area
				shape->ComputeMassAndInertia(k_defaultDensity);
			}
		}
		else
		{
			// Collision between circle and rectangle
			if constexpr (false)
			{
				Circle* c = new Circle();
				c->m_pos = Vector2(10.0f, 50.0f);
				c->m_velocity = Vector2(10.0f, 0.0f);
				c->m_radius = 5.0f;
				c->ComputeMassAndInertia(1.0f);
				m_shapes[0] = c;

				Rectangle* r = new Rectangle();
				r->m_pos = Vector2(50.0f, 50.0f);
				r->m_velocity = Vector2(0.0f, 0.0f);
				r->m_facing = Math::DegToRad(90.0f);
				r->m_halfLength = 15.0f;
				r->m_halfWidth = 1.309f;
				r->ComputeMassAndInertia(1.0f);
				m_shapes[1] = r;
			}

			// Simple collision between two circles
			if constexpr (true)
			{
				{
					Circle* c = new Circle();
					c->m_pos = Vector2(10.0f, 40.0f);
					c->m_velocity = Vector2(10.0f, 0.0f);
					c->m_radius = 5.0f;
					c->ComputeMassAndInertia(1.0f);
					m_shapes[2] = c;
				}
				{
					Circle* c = new Circle();
					c->m_pos = Vector2(40.0f, 40.0f - 10.0f / Math::Sqrt(2.0f));
					c->m_velocity = Vector2(0.0f, 0.0f);
					c->m_radius = 5.0f;
					c->ComputeMassAndInertia(1.0f);
					m_shapes[3] = c;
				}
			}
		}

		m_initialized = true;
	}

	// Move shapes and clamp to field
	for (Shape* shape : m_shapes)
	{
		if (shape == nullptr)
		{
			continue;
		}
		shape->m_pos += shape->m_velocity * k_secondsPerFrame + k_gravity * (0.5f * Math::Sqr(k_secondsPerFrame));
		shape->m_velocity += k_gravity * k_secondsPerFrame;
		shape->m_facing += shape->m_angularVelocity * k_secondsPerFrame;

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
		Shape* s0 = m_shapes[i];
		if (s0 != nullptr)
		{
			for (int j = i + 1; j < m_shapes.Count(); j++)
			{
				Shape* s1 = m_shapes[j];
				if (s1 != nullptr)
				{
					CollisionResponse response = s0->Collide(*s1);
					if (response.m_collided)
					{
						response.ApplyResponse(*s1, *s0);
					}
				}
			}
		}
	}

	// Calculate total energy
	m_totalLinearEnergy = 0.0f;
	m_totalRotationalEnergy = 0.0f;
	for (const auto& shape : m_shapes)
	{
		if (shape != nullptr)
		{
			m_totalLinearEnergy += shape->GetLinearKineticEnergy();
			m_totalRotationalEnergy += shape->GetRotationalKineticEnergy();
		}
	}
}

void App::DrawPhysicsTest()
{
	sf::View view(sf::Vector2f(80.0f, 45.0f), sf::Vector2f(160.0f, 90.0f));
	m_window.setView(view);
	m_window.clear(sf::Color(0, 0, 32, 255));

	for (const Shape* shape : m_shapes)
	{
		if (shape != nullptr)
		{
			shape->Draw(m_window);
		}
	}

	static sf::Font s_font;
	static bool s_initialized = false;
	if (s_initialized == false)
	{
		bool success = s_font.loadFromFile("..\\Data\\Fonts\\Overpass-Regular.ttf");
		s_initialized = true;
	}
	
	sf::Text text;
	text.setFont(s_font);
	wchar_t buffer[64];
	swprintf_s(buffer, L"Lin = %0.6f\nRot = %0.6f\nTot = %0.6f", m_totalLinearEnergy, m_totalRotationalEnergy, m_totalLinearEnergy + m_totalRotationalEnergy);
	text.setString(buffer);
	text.setCharacterSize(16);
	text.setFillColor(sf::Color::White);
	text.setScale(0.2f, 0.2f);
	m_window.draw(text);

	m_window.display();
}
