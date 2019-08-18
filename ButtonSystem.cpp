#include "ButtonSystem.h"
#include "Memory.cpp"
using namespace Memory;

ButtonSystem::~ButtonSystem()
{
	DeInit();
}

void ButtonSystem::Init(sf::Vector2f pos, sf::Vector2f posOffset, sf::Vector2f size, int nrButtons, sf::String* strings, bool vertAlign)
{
	m_position = pos;
	m_buttonsOffset = posOffset;
	m_size = size;
	m_nrButtons	= nrButtons;
	m_buttons = new Button[m_nrButtons];
	m_string = new sf::String[m_nrButtons];
	m_verticalAlignment = vertAlign;

	sf::Vector2f finalButtonPos;
	for (int i = 0; i < m_nrButtons; ++i)
	{
		m_string[i] = strings[i];
		if (vertAlign)
		{
			finalButtonPos = pos + sf::Vector2f(0.0f, m_size.y * i + posOffset.y * i);
		}
		else
		{
			finalButtonPos = pos + sf::Vector2f(m_size.x * i + posOffset.x * i, 0.0f);
		}
		m_buttons[i].Init(finalButtonPos, m_size, m_string[i], true);
	}

	m_nrToggledButtons = 0;
	m_lastToggledButton = -1;
	m_buttonList.clear();
}

void ButtonSystem::DeInit()
{
	DestroyArray(m_buttons);
	DestroyArray(m_string);
}

int ButtonSystem::GetToggledButton()
{
	return m_lastToggledButton;
}

void ButtonSystem::ClearAllToggledButtonsExceptLastPressed()
{	
	while (m_buttonList.size() >= 2)
	{
		Button* b = m_buttonList.front();
		m_buttonList.pop_front();
		b->ResetPress();
		m_nrToggledButtons--;
	}
}

void ButtonSystem::ReturnCorrectValueWhenNoButtonIsToggled()
{
	if (!m_buttons[m_lastToggledButton].IsPressed())
	{
		m_lastToggledButton = -1;
		m_buttonList.clear();
		m_nrToggledButtons = 0;
	}
}

void ButtonSystem::Update(float dt, sf::RenderWindow& rw)
{
	for (int i = 0; i < m_nrButtons; ++i)
	{
		if (m_buttons[i].IsPressed())
		{
			if(m_lastToggledButton != i && m_nrToggledButtons <= 1)
			{
				m_buttonList.push_back(&m_buttons[i]);
				m_lastToggledButton = i;
				m_nrToggledButtons++;
			}
		}
	}

	ClearAllToggledButtonsExceptLastPressed();
	ReturnCorrectValueWhenNoButtonIsToggled();
	
	for (int i = 0; i < m_nrButtons; ++i)
	{
		m_buttons[i].Update(dt, rw);
	}
}

void ButtonSystem::Draw(sf::RenderTarget& rt)
{
	for (int i = 0; i < m_nrButtons; ++i)
	{
		m_buttons[i].Draw(rt);
	}
}