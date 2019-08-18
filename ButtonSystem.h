#pragma once

#include <list>
#include "Button.h"
class ButtonSystem
{
public:
	int m_nrButtons;
	Button* m_buttons;
	std::list<Button*> m_buttonList;
	sf::Vector2f m_position;
	sf::Vector2f m_buttonsOffset;
	sf::Vector2f m_size;
	sf::String*  m_string;
	bool m_verticalAlignment;

	int m_nrToggledButtons;
	int m_lastToggledButton;
public:
	~ButtonSystem();
	void Init(sf::Vector2f pos, sf::Vector2f posOffset, sf::Vector2f size, int nrButtons, sf::String* strings, bool vertAlign = true);
	void DeInit();
	int GetToggledButton();
	void ClearAllToggledButtonsExceptLastPressed();
	void ReturnCorrectValueWhenNoButtonIsToggled();
	void Update(float dt, sf::RenderWindow& rw);
	void Draw(sf::RenderTarget& rt);
};