#include "pch.h"
#include "NeuronGameDisplay.h"

#include "NeuronGame.h"
#include <SFML/Graphics.hpp>

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
		neuronPlayer.m_shape->Draw(
			window,
			(i == 0) ? playerColor1 : playerColor2,
			playerOutlineColor,
			playerOutlineThickness
		);
	}

	// Draw ball
	const NeuronBall& neuronBall = m_neuronGame.GetBall();
	neuronBall.m_shape.Draw(window, ballFillColor, ballOutlineColor, ballOutlineThickness);
}
