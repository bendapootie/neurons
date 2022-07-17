#include "pch.h"
#include "NeuronGameDisplay.h"

#include "NeuronGame.h"
#include <SFML/Graphics.hpp>

void NeuronGameDisplay::Draw(sf::RenderWindow& window) const
{
	const float width = m_neuronGame.GetFieldWidth();
	const float length = m_neuronGame.GetFieldLength();
	const float goalWidth = m_neuronGame.GetGoalWidth();
	const float goalDepth = goalWidth * 0.2f;	// Goal depth is only visual, so it's not tracked by NeuronBallGame
	const float fieldOutlineThickness = 1.0f;
	const float playerWidth = NeuronPlayer::GetPlayerWidth();
	const float playerLength = NeuronPlayer::GetPlayerLength();
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
	goal.setSize(sf::Vector2f(goalWidth, goalDepth));
	goal.setFillColor(sf::Color::Transparent);
	goal.setOutlineColor(fieldOutlineColor);
	goal.setOutlineThickness(fieldOutlineThickness);
	goal.setPosition((width - goalWidth) * 0.5f, -goalDepth);
	window.draw(goal);

	goal.setFillColor(playerColor2);
	goal.setPosition((width - goalWidth) * 0.5f, length);
	window.draw(goal);

	// Draw field
	sf::RectangleShape field;
	field.setSize(sf::Vector2f(width, length));
	field.setFillColor(sf::Color(0, 64, 0));
	field.setOutlineColor(fieldOutlineColor);
	field.setOutlineThickness(fieldOutlineThickness);
	field.setPosition(0.0f, 0.0f);
	window.draw(field);

	// Draw inside of goals
	goal.setFillColor(goalColor1);
	goal.setOutlineThickness(0.0f);
	goal.setPosition((width - goalWidth) * 0.5f, -goalDepth);
	window.draw(goal);

	goal.setFillColor(goalColor2);
	goal.setPosition((width - goalWidth) * 0.5f, length);
	window.draw(goal);

	// Draw players
	sf::RectangleShape player;
	for (int i = 0; i < m_neuronGame.GetNumPlayers(); i++)
	{
		const NeuronPlayer& neuronPlayer = m_neuronGame.GetPlayer(i);

		player.setSize(sf::Vector2f(playerWidth, playerLength));
		player.setOrigin(player.getSize() * 0.5f);
		player.setFillColor((i == 0) ? playerColor1 : playerColor2);
		player.setOutlineColor(playerOutlineColor);
		player.setOutlineThickness(playerOutlineThickness);
		player.setPosition(neuronPlayer.m_pos.x, neuronPlayer.m_pos.y);
		player.setRotation(neuronPlayer.GetRotationDegrees() + 90.0f);
		window.draw(player);
	}

	// Draw ball
	sf::CircleShape ball;
	const NeuronBall& neuronBall = m_neuronGame.GetBall();
	ball.setRadius(neuronBall.GetRadius());
	ball.setOrigin(ball.getRadius(), ball.getRadius());
	ball.setFillColor(ballFillColor);
	ball.setOutlineColor(ballOutlineColor);
	ball.setOutlineThickness(ballOutlineThickness);
	ball.setPosition(neuronBall.m_shape.m_pos.x, neuronBall.m_shape.m_pos.y);
	window.draw(ball);
}
