#pragma once

#include "ButtonSystem.h"
#include "TextBox.h"

#include "DES.h"
#include "AES.h"
#include "RC4.h"

class Menu
{
public:
	sf::RenderWindow m_window;
	sf::Event m_event;
	int m_currentState;

	ButtonSystem* m_ciphersButtonSystem;
	sf::String* m_ciphersButtonStrings;
	int m_ciphersNrButtons;

	ButtonSystem* m_modesButtonSystem;
	sf::String* m_modesButtonStrings;
	int m_modesNrButtons;

	Button* m_encryptButton;
	Button* m_decryptButton;

	Button* m_addExtensionButton; //to decrypt files
	Button* m_encryptPixels;

	TextBox* m_filepathTextbox;

	//ints to handle when a ciphers is changed (another cipher button is pressed)
	int m_currentCipherButtonPressed;
	int m_lastCipherButtonPressed;

	int m_currentModeButtonPressed;

	bool m_keyPressed[sf::Keyboard::KeyCount];

	sf::Font m_font;
	sf::Text m_ciphersTitle; //situated above ciphers buttons
	sf::Text m_modesTitle;	 //situated above  modes  buttons

	sf::Texture m_bgTexture;
	sf::Sprite m_bgSprite;

	//showoffimage modes (default / ecb / cbc)
	sf::Image m_showoffImage;
	sf::Texture m_showoffTexture;
	sf::Sprite m_showoffSprite;

	sf::Image m_showoffECBImage;
	sf::Texture m_showoffECBTexture;
	sf::Sprite m_showoffECBSprite;

	sf::Image m_showoffCBCImage;
	sf::Texture m_showoffCBCTexture;
	sf::Sprite m_showoffCBCSprite;

	char* m_showoffPixelsBuffer;

	sf::String m_originalFileName;
	sf::String m_encString;
	sf::String m_decString;

	enum ESubmit
	{
		NOSUBMIT = -1,
		ENCRYPT = 0,
		DECRYPT
	};

	DES m_des;
	AES  m_aes;
	RC4  m_rc4;

	enum ECipher
	{
		NONE = 0,
		DES,
		DES3,
		AES,
		RC4,
		CIPHERS_COUNT
	};

	int m_currentCipher = ECipher::NONE;

	enum EStates
	{
		MENU = 0,
		STATES_COUNT
	};

	Menu();
	~Menu();
	void Init();
	void InitGUISystems();
	void DeInit();
	bool OnKeyPress(sf::Keyboard::Key key);
	void MakeJPG();
	sf::String GetFileStringExtension(sf::String filename);
	void InputTextBox();
	void ButtonsUpdate(float dt);
	void Update(float dt);
	void Draw();
};