#include "TextBox.h"

void TextBox::Init(sf::Vector2f pos, sf::Vector2f size, sf::String string, bool toggleable, sf::Vector2f textOnButtonOffset)
{
	Button::Init(pos, size, string, toggleable, textOnButtonOffset);
	m_text.setPosition(sf::Vector2f(m_position.x, m_position.y - m_size.y));
	
	m_displayText.setFont(*m_font);
	m_displayText.setString(m_string);
	m_displayText.setCharacterSize(40);
	m_displayText.setFillColor(m_outlineColorIdle);
	m_displayText.setPosition(sf::Vector2f(m_position.x + 10.0f + textOnButtonOffset.x, m_position.y + m_size.y / 2.0f - m_text.getLocalBounds().height / 2.0f - 8.0f + textOnButtonOffset.y));

	m_maxInputChars = false;
	m_inputLimit = 40;
}

void TextBox::SetPosition(sf::Vector2f pos)
{
	Button::SetPosition(pos);
	m_text.setPosition(pos.x, pos.y - m_size.y);
	m_displayText.setPosition(pos.x + 10.0f, pos.y + m_size.y / 2.0f - m_text.getLocalBounds().height / 2.0f - 8.0f);
}

void TextBox::SetDisplayString(sf::String string)
{
	m_string = string;
	m_displayText.setString(string);
}

void TextBox::SetInputLimit(int inputLimit)
{
	m_inputLimit = inputLimit;
}

void TextBox::Update(float dt, sf::RenderWindow& rw)
{
	Button::Update(dt, rw);
	this->SetDisplayString(this->m_string);
}

void TextBox::Draw(sf::RenderTarget& rt)
{
	Button::Draw(rt);
	rt.draw(m_displayText);
}