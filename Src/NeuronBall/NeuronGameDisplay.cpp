#include "NeuronGameDisplay.h"
#include "NeuronGame.h"
#include <SFML/Graphics.hpp>

void NeuronGameDisplay::Draw(sf::RenderWindow& window) const
{
	const float width = m_neuronGame.GetFieldWidth();
	const float length = m_neuronGame.GetFieldLength();
	const float goalWidth = m_neuronGame.GetGoalWidth();
	const float goalDepth = goalWidth * 0.2f;	// Goal depth is only visual, so it's not tracked by NeuronBallGame
	const float outlineThickness = 1.0f;

	sf::Color outlineColor(192, 64, 64);
	sf::Color playerColor1(128, 0, 0);
	sf::Color playerColor2(0, 0, 128);

	// Draw goal outlines
	sf::RectangleShape goal;
	goal.setSize(sf::Vector2f(goalWidth, goalDepth));
	goal.setFillColor(sf::Color::Transparent);
	goal.setOutlineColor(outlineColor);
	goal.setOutlineThickness(outlineThickness);
	goal.setPosition((width - goalWidth) * 0.5f, -goalDepth);
	window.draw(goal);

	goal.setFillColor(playerColor2);
	goal.setPosition((width - goalWidth) * 0.5f, length);
	window.draw(goal);

	// Draw field
	sf::RectangleShape field;
	field.setSize(sf::Vector2f(width, length));
	field.setFillColor(sf::Color(0, 64, 0));
	field.setOutlineColor(sf::Color(192, 64, 64));
	field.setOutlineThickness(outlineThickness);
	field.setPosition(0.0f, 0.0f);
	window.draw(field);

	// Draw inside of goals
	goal.setFillColor(playerColor1);
	goal.setOutlineThickness(0.0f);
	goal.setPosition((width - goalWidth) * 0.5f, -goalDepth);
	window.draw(goal);

	goal.setFillColor(playerColor2);
	goal.setPosition((width - goalWidth) * 0.5f, length);
	window.draw(goal);

	// Draw players
	// Draw ball
}
