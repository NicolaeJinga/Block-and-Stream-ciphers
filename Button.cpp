#include "Button.h"

void Button::Init(sf::Vector2f pos, sf::Vector2f size, sf::String string, bool toggleable, sf::Vector2f textOffsetOnButton)
{
	m_position = pos;
	m_size = size;

	m_idleColor = sf::Color(230, 230, 230);
	m_hoverColor = sf::Color(180, 180, 180);
	m_pressedColor = sf::Color(150, 200, 255); //blue

	m_outlineColorIdle = sf::Color(115, 115, 115);
	m_outlineColorHover = sf::Color(155, 69, 147); // purple

	m_rectButtonCurrentColor = m_idleColor;
	m_rectButtonCurrentOutlineColor = m_outlineColorIdle;

	m_cooldownUntilNextPress = 0.2f;
	m_cooldownElapsedTime = 0.0f;

	m_toggleable = toggleable;
	m_isButtonPressed = false;

	m_text.setFont(*m_font);
	m_text.setString(string);
	m_text.setCharacterSize(40);
	m_text.setFillColor(m_outlineColorIdle);
	m_text.setPosition(sf::Vector2f(m_position.x + m_size.x/2.0f - m_text.getLocalBounds().width/2.0f + textOffsetOnButton.x, m_position.y + m_size.y/2.0f - m_text.getLocalBounds().height / 2.0f - 10.0f + textOffsetOnButton.y));

	memset(m_keyPressed, 0, sf::Mouse::ButtonCount*sizeof(bool));
}

void Button::LoadFont()
{
	m_font = new sf::Font;
	m_font->loadFromFile("arial.ttf");
}

void Button::SetPosition(sf::Vector2f pos)
{
	m_position = pos;
}

void Button::SetSize(sf::Vector2f size)
{
	m_size = size;
}

sf::Vector2f Button::GetPosition()
{
	return m_position;
}

sf::Vector2f Button::GetSize()
{
	return m_size;
}

bool Button::OnButtonPress(sf::Mouse::Button button)
{
	if (!sf::Mouse::isButtonPressed(button) && m_keyPressed[button])
	{
		m_keyPressed[button] = false;
	}
	if (sf::Mouse::isButtonPressed(button) && !m_keyPressed[button])
	{
		m_keyPressed[button] = true;
		return true;
	}
	return false;
}

bool Button::IsPressed()
{
	return m_isButtonPressed;
}

void Button::ResetPress()
{
	m_isButtonPressed = false;
}

float Button::GetCDElapsedTime()
{
	return m_cooldownElapsedTime;
}

void Button::SetCDElapsedTime(float cdet)
{
	m_cooldownElapsedTime = cdet;
}

float Button::GetCooldown()
{
	return m_cooldownUntilNextPress;
}

void Button::SetCooldown(float cd)
{
	m_cooldownUntilNextPress = cd;
}

void Button::Update(float dt, sf::RenderWindow& rw)
{
	m_rectButtonCurrentColor = m_idleColor;
	m_rectButtonCurrentOutlineColor = m_outlineColorIdle;
	sf::Vector2i mousePos = sf::Mouse::getPosition(rw);

	if (mousePos.x > m_position.x && mousePos.x < m_position.x + m_size.x
		&& mousePos.y > m_position.y && mousePos.y < m_position.y + m_size.y)
	{
		m_rectButtonCurrentColor = m_hoverColor;
		m_rectButtonCurrentOutlineColor = m_outlineColorHover;
		if (OnButtonPress(sf::Mouse::Button::Left))
		{
			if (m_toggleable)
			{
				m_isButtonPressed = !m_isButtonPressed;
			}
			else
			{
				m_isButtonPressed = true;
			}
		}
	}
	if (m_isButtonPressed)
	{
		m_rectButtonCurrentColor = m_pressedColor;
	}
}

void Button::Draw(sf::RenderTarget& rt)
{
	m_rectButton.setPosition(m_position);
	m_rectButton.setSize(m_size);
	m_rectButton.setFillColor(m_rectButtonCurrentColor);
	m_rectButton.setOutlineColor(m_rectButtonCurrentOutlineColor);
	m_rectButton.setOutlineThickness(3.0f);
	rt.draw(m_rectButton);
	m_text.setFillColor(m_rectButtonCurrentOutlineColor);
	rt.draw(m_text);
}

sf::Font* Button::m_font;