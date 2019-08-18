#pragma once

#include "SFML/Graphics.hpp"

class Button
{
protected:
	sf::Vector2f m_position;
	sf::Vector2f m_size;

	sf::Color m_idleColor;
	sf::Color m_hoverColor;
	sf::Color m_pressedColor;

	sf::Color m_outlineColorIdle;
	sf::Color m_outlineColorHover;
	sf::Color m_outlineColorPressed;

	sf::RectangleShape m_rectButton;
	sf::Color m_rectButtonCurrentColor;
	sf::Color m_rectButtonCurrentOutlineColor;

	float m_cooldownUntilNextPress = 2.0f;
	float m_cooldownElapsedTime = 0.0f;

	static sf::Font* m_font;
	sf::Text m_text;

	bool m_toggleable;
	
	bool m_keyPressed[sf::Mouse::ButtonCount];
public:
	
	bool m_isButtonPressed;
	void Init(sf::Vector2f pos, sf::Vector2f size,
		sf::String string, bool toggleable = false,
		sf::Vector2f textOffsetOnButton = sf::Vector2f(0.0f,0.0f));
	static void LoadFont();
	void SetPosition(sf::Vector2f pos);
	void SetSize(sf::Vector2f size);
	sf::Vector2f GetPosition();
	sf::Vector2f GetSize();
	bool OnButtonPress(sf::Mouse::Button button);
	bool IsPressed();
	void ResetPress();
	float GetCDElapsedTime();
	void SetCDElapsedTime(float cdet);
	float GetCooldown();
	void SetCooldown(float cd);
	void Update(float dt, sf::RenderWindow& rw);
	void Draw(sf::RenderTarget& rt);
};