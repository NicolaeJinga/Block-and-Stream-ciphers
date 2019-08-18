#include <iostream>
#include <iomanip>
#include <fstream>
#include <limits>
#include "DES.h"
#include "AES.h"
#include "RC4.h"
#include <ctime>

#include "SFML/Graphics.hpp"
#include "Menu.h"

using namespace std;

int main()
{
	Menu menu;
	menu.Init();
	sf::Clock clock;
	float dt = 0.0f;
	while (menu.m_window.isOpen())
	{
		while (menu.m_window.pollEvent(menu.m_event))
		{
			if (menu.m_event.type == sf::Event::Closed)
				menu.m_window.close();
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
				menu.m_window.close();
			if (menu.m_filepathTextbox->IsPressed())
				menu.InputTextBox();
		}
		dt = clock.restart().asSeconds();
		menu.Update(dt);
		menu.m_window.clear();
		menu.Draw();
		menu.m_window.display();
	}

	return 0;
}