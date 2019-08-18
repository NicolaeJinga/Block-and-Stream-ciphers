#include "TextBoxSystem.h"
#include "Memory.cpp"
using namespace Memory;

TextBoxSystem::~TextBoxSystem()
{
	DeInit();
}

void TextBoxSystem::Init(sf::Vector2f pos, sf::Vector2f posOffset, sf::Vector2f size, int nrTextBoxes, sf::String* strings, bool vertAlign)
{
	m_position = pos;
	m_positionOffset = posOffset;
	m_size = size;
	m_nrTextBoxes = nrTextBoxes;
	m_textBoxes = new TextBox[m_nrTextBoxes];
	m_strings = new sf::String[m_nrTextBoxes];
	m_verticalAlignment = vertAlign;

	sf::Vector2f finalTextBoxPos;
	for (int i = 0; i < m_nrTextBoxes; ++i)
	{
		m_strings[i] = strings[i];
		if (vertAlign)
		{
			finalTextBoxPos = pos + sf::Vector2f(0.0f, m_size.y * i + posOffset.y * i);
		}
		else
		{
			finalTextBoxPos = pos + sf::Vector2f(m_size.x * i + posOffset.x * i, 0.0f);
		}
		m_textBoxes[i].Init(finalTextBoxPos, m_size, m_strings[i]);
	}

	m_nrToggledTextBoxes = 0;
	m_lastToggledTextBox = -1;
	m_textBoxList.clear();
	//Input text box is set to active
	m_textBoxes[0].m_isButtonPressed = true;
}

void TextBoxSystem::DeInit()
{
	DestroyArray(m_textBoxes);
	DestroyArray(m_strings);
}

int TextBoxSystem::GetToggledTextBox()
{
	return m_lastToggledTextBox;
}

void TextBoxSystem::ClearAllToggledTextBoxesExceptLastPressed()
{
	while (m_textBoxList.size() >= 2)
	{
		TextBox* b = m_textBoxList.front();
		m_textBoxList.pop_front();
		b->ResetPress();
		m_nrToggledTextBoxes--;
	}
}

void TextBoxSystem::ReturnCorrectValuenWhenNoTextBoxIsToggled()
{
	if (!m_textBoxes[m_lastToggledTextBox].IsPressed())
	{
		m_lastToggledTextBox = -1;
		m_textBoxList.clear();
		m_nrToggledTextBoxes = 0;
	}
}

sf::String& TextBoxSystem::GetString()
{
	return m_textBoxes[m_lastToggledTextBox].m_string;
}

int& TextBoxSystem::GetInputLimit()
{
	return m_textBoxes[m_lastToggledTextBox].m_inputLimit;
}

void TextBoxSystem::Update(float dt, sf::RenderWindow& rw)
{
	for (int i = 0; i < m_nrTextBoxes; ++i)
	{
		if (m_textBoxes[i].IsPressed())
		{
			if (m_lastToggledTextBox != i && m_nrToggledTextBoxes <= 1)
			{
				m_textBoxList.push_back(&m_textBoxes[i]);
				m_lastToggledTextBox = i;
				m_nrToggledTextBoxes++;
			}
		}
	}

	ClearAllToggledTextBoxesExceptLastPressed();
	ReturnCorrectValuenWhenNoTextBoxIsToggled();

	for (int i = 0; i < m_nrTextBoxes; ++i)
	{
		m_textBoxes[i].Update(dt, rw);
	}
}

void TextBoxSystem::Draw(sf::RenderTarget& rt)
{
	for (int i = 0; i < m_nrTextBoxes; ++i)
	{
		m_textBoxes[i].Draw(rt);
	}
}