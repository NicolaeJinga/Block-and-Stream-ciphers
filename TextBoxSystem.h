#pragma once

#include<list>
#include "TextBox.h"

class TextBoxSystem
{
public:
	int m_nrTextBoxes;
	TextBox* m_textBoxes;
	std::list<TextBox*> m_textBoxList;
	sf::Vector2f m_position;
	sf::Vector2f m_positionOffset;
	sf::Vector2f m_size;
	sf::String* m_strings;
	bool m_verticalAlignment;

	int m_nrToggledTextBoxes;
	int m_lastToggledTextBox;

	~TextBoxSystem();
	void Init(sf::Vector2f pos, sf::Vector2f posOffset, sf::Vector2f size, int nrTextBoxes, sf::String* strings, bool verticalAlign = true);
	void DeInit();
	int GetToggledTextBox();
	void ClearAllToggledTextBoxesExceptLastPressed();
	void ReturnCorrectValuenWhenNoTextBoxIsToggled();
	sf::String& GetString();
	int& GetInputLimit();
	void Update(float dt, sf::RenderWindow& rw);
	void Draw(sf::RenderTarget& rt);
};