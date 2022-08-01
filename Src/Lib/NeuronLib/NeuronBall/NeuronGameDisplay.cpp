#include "pch.h"
#include "NeuronGameDisplay.h"

#include "NeuronGame.h"
#include <SFML/Graphics.hpp>

namespace
{
	static sf::Font* s_font = nullptr;

	sf::Font& GetDebugFont()
	{
		if (s_font == nullptr)
		{
			s_font = new sf::Font();
			bool success = s_font->loadFromFile("..\\Data\\Fonts\\Overpass-Regular.ttf");
		}
		return *s_font;
	}
}



void NeuronGameDisplay::Draw(sf::RenderWindow& window) const
{
	const float length = m_neuronGame.GetFieldLength();
	const float width = m_neuronGame.GetFieldWidth();
	const float goalWidth = m_neuronGame.GetGoalWidth();
	const float goalDepth = goalWidth * 0.2f;	// Goal depth is only visual, so it's not tracked by NeuronBallGame
	const float fieldOutlineThickness = 1.0f;
	const float playerOutlineThickness = 0.25f;

	const sf::Color goalColor1(128, 0, 0);
	const sf::Color goalColor2(0, 0, 128);

	const sf::Color fieldOutlineColor(192, 64, 64);
	const sf::Color playerColor1(255, 0, 0);
	const sf::Color playerColor2(0, 0, 255);
	const sf::Color playerColor1Front(255, 128, 128);
	const sf::Color playerColor2Front(128, 128, 255);
	const sf::Color playerOutlineColor(192, 192, 192, 128);

	const sf::Color ballFillColor(255, 255, 255);
	const sf::Color ballOutlineColor(0, 0, 0);
	const float ballOutlineThickness = 0.25f;

	// Draw goal outlines
	sf::RectangleShape goal;
	goal.setSize(sf::Vector2f(goalDepth, goalWidth));
	goal.setFillColor(sf::Color::Transparent);
	goal.setOutlineColor(fieldOutlineColor);
	goal.setOutlineThickness(fieldOutlineThickness);
	goal.setPosition(-goalDepth, (width - goalWidth) * 0.5f);
	window.draw(goal);

	goal.setFillColor(playerColor2);
	goal.setPosition(length, (width - goalWidth) * 0.5f);
	window.draw(goal);

	// Draw field
	sf::RectangleShape field;
	field.setSize(sf::Vector2f(length, width));
	field.setFillColor(sf::Color(0, 64, 0));
	field.setOutlineColor(fieldOutlineColor);
	field.setOutlineThickness(fieldOutlineThickness);
	field.setPosition(0.0f, 0.0f);
	window.draw(field);

	// Draw inside of goals
	goal.setFillColor(goalColor1);
	goal.setOutlineThickness(0.0f);
	goal.setPosition(-goalDepth, (width - goalWidth) * 0.5f);
	window.draw(goal);

	goal.setFillColor(goalColor2);
	goal.setPosition(length, (width - goalWidth) * 0.5f);
	window.draw(goal);

	// Draw players
	sf::RectangleShape player;
	for (int i = 0; i < m_neuronGame.GetNumPlayers(); i++)
	{
		const NeuronPlayer& neuronPlayer = m_neuronGame.GetPlayer(i);
		sf::Color playerColor = (i == 0) ? playerColor1 : playerColor2;
		neuronPlayer.m_shape->Draw(
			window,
			playerColor,
			playerOutlineColor,
			playerOutlineThickness
		);

		// Draw forward line
		constexpr float k_frontLineThickness = 2.0f;
		sf::Color playerColorFront = (i == 0) ? playerColor1Front : playerColor2Front;
		
		sf::RectangleShape line;
		line.setSize(sf::Vector2f(k_frontLineThickness, neuronPlayer.GetPlayerWidth()));
		line.setFillColor(playerColorFront);
		line.setOrigin(sf::Vector2f(k_frontLineThickness - neuronPlayer.GetPlayerHalfLength(), neuronPlayer.GetPlayerHalfWidth()));
		line.setPosition(sf::Vector2f(neuronPlayer.GetPos().x, neuronPlayer.GetPos().y));
		line.setRotation(RadToDeg(neuronPlayer.GetFacing()));
		window.draw(line);
	}

	// Draw ball
	const NeuronBall& neuronBall = m_neuronGame.GetBall();
	neuronBall.m_shape.Draw(window, ballFillColor, ballOutlineColor, ballOutlineThickness);

	// Draw score
	{
		sf::Text text;
		const sf::Font& font = GetDebugFont();
		text.setFont(font);
		const float lineSpacing = font.getLineSpacing(text.getCharacterSize());
		
		wchar_t buffer[32];
		swprintf_s(buffer, L"%d - %d", m_neuronGame.GetPlayerScore(0), m_neuronGame.GetPlayerScore(1));
		text.setString(buffer);
		text.setFillColor(sf::Color::White);
		text.setScale(0.2f, 0.2f);
		text.setOrigin(text.getLocalBounds().width * 0.5f, lineSpacing);
		text.setPosition(sf::Vector2f(length * 0.5f, 0.0f));
		window.draw(text);
		
		const int timeRemaining = static_cast<int>(Math::Ceil(m_neuronGame.GetTimeRemaining()));
		swprintf_s(buffer, L"%d:%02d", timeRemaining / 60, timeRemaining % 60);
		text.setString(buffer);
		text.setOrigin(0.0f, lineSpacing);
		text.setPosition(sf::Vector2f(0.0f, 0.0f));
		window.draw(text);

		if (m_neuronGame.IsGameOver())
		{
			text.setScale(0.4f, 0.4f);
			text.setString(L"Game Over");
			text.setOrigin(text.getLocalBounds().width * 0.5f, text.getLocalBounds().height * 1.0f);
			text.setPosition(sf::Vector2f(length * 0.5f, width * 0.5f));
			window.draw(text);
		}
	}
}
