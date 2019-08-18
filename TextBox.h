#pragma once

#include "SFML/Graphics.hpp"
#include "Button.h"

class TextBox : public Button
{
public:
	sf::String m_string;
	sf::Text m_displayText;
	bool m_maxInputChars;
	int m_inputLimit;
	//toggleable set to true means once you have an active texbox, you cannot uncheck it, leaving no active textbox.
	void Init(sf::Vector2f pos, sf::Vector2f size,
		sf::String string, bool toggleable = false,
		sf::Vector2f textOnButtonOffset = sf::Vector2f(0.0f,0.0f));
	void SetDisplayString(sf::String string);
	void SetPosition(sf::Vector2f pos);
	void SetInputLimit(int inputLimit);
	void Update(float dt, sf::RenderWindow& rw);
	void Draw(sf::RenderTarget& rt);
};