#include "Menu.h"
#include "Memory.cpp"

using namespace Memory;

Menu::Menu()
{
	m_window.create(sf::VideoMode(1600, 900), "Metode criptografice de tip block si de tip flux");
	m_currentState = EStates::MENU;
	Button::LoadFont();
	memset(m_keyPressed, 0, sf::Keyboard::KeyCount*sizeof(bool));
	if (!m_font.loadFromFile("arial.ttf"))
	{
		printf("Error loading arial.ttf in Menu::Menu()\n");
	}

	InitGUISystems();

	//situated above ciphers button
	m_ciphersTitle.setCharacterSize(36);
	m_ciphersTitle.setFillColor(sf::Color(155,155,155));
	m_ciphersTitle.setFont(m_font);
	m_ciphersTitle.setString("Ciphers");
	m_ciphersTitle.setPosition(sf::Vector2f(m_ciphersButtonSystem->m_position.x + 90, m_ciphersButtonSystem->m_position.y - 60));

	//situated above modes buttons
	m_modesTitle.setCharacterSize(36);
	m_modesTitle.setFillColor(sf::Color(155, 155, 155));
	m_modesTitle.setFont(m_font);
	m_modesTitle.setString("Operation Modes");
	m_modesTitle.setPosition(sf::Vector2f(m_modesButtonSystem->m_position.x - 10, m_modesButtonSystem->m_position.y - 60));

	m_currentCipherButtonPressed = -1;
	m_lastCipherButtonPressed = -1;

	m_currentModeButtonPressed = -1;

	m_currentCipher = ECipher::NONE;

	m_bgTexture.loadFromFile("bg169.jpg");
	m_bgSprite.setTexture(m_bgTexture);
	m_bgSprite.setColor(sf::Color(255, 255, 255, 80));

}

Menu::~Menu()
{
	DeInit();
}

void Menu::Init()
{
	//init ciphers
}

void Menu::InitGUISystems()
{
	//ciphers button system
	m_ciphersNrButtons = 4;
	m_ciphersButtonSystem = new ButtonSystem;
	m_ciphersButtonStrings = new sf::String[m_ciphersNrButtons];
	char* buttonStrings[] = { "DES", "3DES", "AES", "RC4" };
	for (int i = 0; i < m_ciphersNrButtons; ++i)
	{
		m_ciphersButtonStrings[i] = buttonStrings[i];
	}
	m_ciphersButtonSystem->Init(sf::Vector2f(1270, 120), sf::Vector2f(30, 30), sf::Vector2f(300, 100), m_ciphersNrButtons, m_ciphersButtonStrings);

	//modes button system
	m_modesNrButtons = 2;
	m_modesButtonSystem = new ButtonSystem;
	m_modesButtonStrings = new sf::String[m_modesNrButtons];
	char* modesStrings[] = { "ECB", "CBC" };
	for (int i = 0; i < m_modesNrButtons; ++i)
	{
		m_modesButtonStrings[i] = modesStrings[i];
	}
	m_modesButtonSystem->Init(sf::Vector2f(1270, 750), sf::Vector2f(60, 50), sf::Vector2f(120, 50), m_modesNrButtons, m_modesButtonStrings, false);

	//encrypt/decrypt buttons
	m_encryptButton = new Button;
	m_encryptButton->Init(sf::Vector2f(450, 700), sf::Vector2f(300, 100), "Encrypt", false);

	m_decryptButton = new Button;
	m_decryptButton->Init(sf::Vector2f(850, 700), sf::Vector2f(300, 100), "Decrypt", false);

	//a toggle button to add .jpg to decrypted files
	m_addExtensionButton = new Button;
	m_addExtensionButton->Init(sf::Vector2f(960, 825), sf::Vector2f(80, 50), ".ext", true);

	//toggle button to encrypt pixels instead of bits/bytes
	m_encryptPixels = new Button;
	m_encryptPixels->Init(sf::Vector2f(540, 825), sf::Vector2f(120, 50), "Pixels", true);


	//filepath textbox
	m_filepathTextbox = new TextBox;
	m_filepathTextbox->Init(sf::Vector2f(450, 550), sf::Vector2f(700, 50), "File path:", true);
	m_filepathTextbox->SetInputLimit(35);
	m_filepathTextbox->SetDisplayString("demo/linux.png");
	m_filepathTextbox->m_isButtonPressed = true;
	
}

void Menu::DeInit()
{
	Destroy(m_ciphersButtonSystem);
	DestroyArray(m_ciphersButtonStrings);
	Destroy(m_modesButtonSystem);
	DestroyArray(m_modesButtonStrings);

	Destroy(m_encryptButton);
	Destroy(m_decryptButton);
	Destroy(m_addExtensionButton);
	Destroy(m_filepathTextbox);
}

bool Menu::OnKeyPress(sf::Keyboard::Key key)
{
	if (!sf::Keyboard::isKeyPressed(key) && m_keyPressed[key])
	{
		m_keyPressed[key] = false;
	}
	if (sf::Keyboard::isKeyPressed(key) && !m_keyPressed[key])
	{
		m_keyPressed[key] = true;
		return true;
	}
	return false;
}

sf::String Menu::GetFileStringExtension(sf::String filename)
{
	if (!filename.isEmpty())
	{
		int i;
		for (i = filename.getSize()-1; i >= 0 ; --i)
		{
			if (filename[i] == '.')
			{
				return filename.substring(i, filename.getSize() - i);
			}
		}
	}
	return sf::String(".ERROR");
}

void Menu::InputTextBox()
{
	if (m_filepathTextbox->m_string.getSize() >= (size_t)m_filepathTextbox->m_inputLimit)
	{
		m_filepathTextbox->m_maxInputChars = true;
	}

	if (m_filepathTextbox->m_maxInputChars)
	{
		m_filepathTextbox->m_maxInputChars = false;
	}

	//get the input from keyboard
	if (m_currentState == MENU && m_event.type == sf::Event::TextEntered
		&& m_filepathTextbox->m_string.getSize() < (size_t)m_filepathTextbox->m_inputLimit
		&& !m_filepathTextbox->m_maxInputChars
		&& !sf::Keyboard::isKeyPressed(sf::Keyboard::BackSpace))
	{
		m_filepathTextbox->m_string.insert(m_filepathTextbox->m_string.getSize(), m_event.text.unicode);
	}
	else if (m_event.type == sf::Event::KeyPressed)
	{
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::BackSpace) && m_filepathTextbox->m_string.getSize() > 0)
		{
			m_filepathTextbox->m_string.erase(m_filepathTextbox->m_string.getSize() - 1);
			m_filepathTextbox->m_maxInputChars = false;
		}
	}
}

void Menu::ButtonsUpdate(float dt)
{
	m_encryptPixels->Update(dt, m_window);

	m_encryptButton->Update(dt, m_window);
	if (m_encryptButton->IsPressed())
	{
		if (m_encryptButton->GetCDElapsedTime() == 0.0f)
		{
			if (m_currentCipher == ECipher::DES)
			{
				printf("DES Encryption started\n");

				m_encString = m_filepathTextbox->m_string;
				m_encString.insert(m_encString.getSize(), ".enc.des");
				if (m_currentModeButtonPressed == 1)
					m_encString.insert(m_encString.getSize(), ".cbc");
				else
					m_encString.insert(m_encString.getSize(), ".ecb");

				if (m_encryptPixels->IsPressed())
				{
					m_des.LoadKey("DESKey.txt");
					
					if (!m_showoffImage.loadFromFile(m_filepathTextbox->m_string))
					{
						cout << "Failed to load image from file." << endl;
						goto safetyencryptprecaution;
					}
					m_showoffPixelsBuffer = (char*)m_showoffImage.getPixelsPtr();

					char* pxlptr = m_des.EncryptPixels(m_showoffPixelsBuffer, (long long int)m_showoffImage.getSize().x * (long long int)m_showoffImage.getSize().y * 4, m_des.m_keyBuffer, m_currentModeButtonPressed);

					m_showoffECBImage.create(m_showoffImage.getSize().x, m_showoffImage.getSize().y, (sf::Uint8*)pxlptr);

					m_encString.insert(m_encString.getSize(), ".pxl");
					m_encString.insert(m_encString.getSize(), GetFileStringExtension(m_filepathTextbox->m_string));
					m_showoffECBImage.saveToFile(m_encString);

					m_showoffECBTexture.loadFromImage(m_showoffECBImage);
					m_showoffECBSprite.setTexture(m_showoffECBTexture, true);
				}
				else
				{
					m_des.Encrypt((char*)m_filepathTextbox->m_string.toAnsiString().c_str(), (char*)m_encString.toAnsiString().c_str(), "DESKey.txt", m_currentModeButtonPressed);
				}

				printf("DES Encryption ended\n");
			}

			if (m_currentCipher == ECipher::DES3)
			{
				printf("3DES Encryption started\n");

				m_encString = m_filepathTextbox->m_string;
				m_encString.insert(m_encString.getSize(), ".enc.3des");
				if (m_currentModeButtonPressed == 1)
					m_encString.insert(m_encString.getSize(), ".cbc");
				else
					m_encString.insert(m_encString.getSize(), ".ecb");

				if (m_encryptPixels->IsPressed())
				{
					m_des.Load3DESKey("3DESKey.txt");
					if (!m_showoffImage.loadFromFile(m_filepathTextbox->m_string))
					{
						cout << "Failed to load image from file." << endl;
						goto safetyencryptprecaution;
					}
					m_showoffPixelsBuffer = (char*)m_showoffImage.getPixelsPtr();

					char* pxlptr = m_des.Encrypt3DESPixels(m_showoffPixelsBuffer, (long long int)m_showoffImage.getSize().x * (long long int)m_showoffImage.getSize().y * 4, m_des.m_3DESkeyBuffer, m_currentModeButtonPressed);

					m_showoffECBImage.create(m_showoffImage.getSize().x, m_showoffImage.getSize().y, (sf::Uint8*)pxlptr);

					m_encString.insert(m_encString.getSize(), ".pxl");
					m_encString.insert(m_encString.getSize(), GetFileStringExtension(m_filepathTextbox->m_string));
					m_showoffECBImage.saveToFile(m_encString);

					m_showoffECBTexture.loadFromImage(m_showoffECBImage);
					m_showoffECBSprite.setTexture(m_showoffECBTexture, true);
				}
				else
				{
					m_des.Encrypt3DES((char*)m_filepathTextbox->m_string.toAnsiString().c_str(), (char*)m_encString.toAnsiString().c_str(), "3DESKey.txt", m_currentModeButtonPressed);
				}
				
				printf("3DES Encryption ended\n");
			}

			if (m_currentCipher == ECipher::AES)
			{
				printf("AES Encryption started\n");
				m_encString = m_filepathTextbox->m_string;
				m_encString.insert(m_encString.getSize(), ".enc.aes");
				
				if (m_currentModeButtonPressed == 1)
					m_encString.insert(m_encString.getSize(), ".cbc");
				else
					m_encString.insert(m_encString.getSize(), ".ecb");

				if (m_encryptPixels->IsPressed())
				{
					m_aes.KeySize = m_aes.LoadKey("AESKey.txt");

					if (!m_showoffImage.loadFromFile(m_filepathTextbox->m_string))
					{
						cout << "Failed to load image from file." << endl;
						goto safetyencryptprecaution;
					}
					m_showoffPixelsBuffer = (char*)m_showoffImage.getPixelsPtr();

					char* pxlptr = m_aes.EncryptPixels(m_showoffPixelsBuffer, (long long int)m_showoffImage.getSize().x * (long long int)m_showoffImage.getSize().y * 4, m_aes.m_keyBuffer, m_currentModeButtonPressed);

					m_showoffECBImage.create(m_showoffImage.getSize().x, m_showoffImage.getSize().y, (sf::Uint8*)pxlptr);

					m_encString.insert(m_encString.getSize(), ".pxl");
					m_encString.insert(m_encString.getSize(), GetFileStringExtension(m_filepathTextbox->m_string));
					m_showoffECBImage.saveToFile(m_encString);

					m_showoffECBTexture.loadFromImage(m_showoffECBImage);
					m_showoffECBSprite.setTexture(m_showoffECBTexture, true);
				}
				else
				{
					m_aes.Encrypt((char*)m_filepathTextbox->m_string.toAnsiString().c_str(), (char*)m_encString.toAnsiString().c_str(), "AESKey.txt", m_currentModeButtonPressed);
				}
				printf("AES Encryption ended\n");
			}

			if (m_currentCipher == ECipher::RC4)
			{
				printf("RC4 Encryption started\n");

				m_encString = m_filepathTextbox->m_string;
				m_encString.insert(m_encString.getSize(), ".enc.rc4");

				if (m_encryptPixels->IsPressed())
				{
					m_rc4.LoadKey("RC4Key.txt");

					if (!m_showoffImage.loadFromFile(m_filepathTextbox->m_string))
					{
						cout << "Failed to load image from file." << endl;
						goto safetyencryptprecaution;
					}
					m_showoffPixelsBuffer = (char*)m_showoffImage.getPixelsPtr();

					long long int sz = (long long int)m_showoffImage.getSize().x * (long long int)m_showoffImage.getSize().y * 4;
					char* buffer = new char[sz];
					m_rc4.Input = buffer;
					m_rc4.Output = new unsigned char[sz];
					m_rc4.outBuff = new unsigned char[sz];

					m_rc4.m_inputLength = sz;

					char* pxlptr = (char*)m_rc4.CryptPixels(m_showoffPixelsBuffer);
					

					m_showoffECBImage.create(m_showoffImage.getSize().x, m_showoffImage.getSize().y, (sf::Uint8*)pxlptr);

					m_encString.insert(m_encString.getSize(), ".pxl");
					m_encString.insert(m_encString.getSize(), GetFileStringExtension(m_filepathTextbox->m_string));
					m_showoffECBImage.saveToFile(m_encString);

					m_showoffECBTexture.loadFromImage(m_showoffECBImage);
					m_showoffECBSprite.setTexture(m_showoffECBTexture, true);

					delete[] buffer;
					delete[] m_rc4.Output;
					delete[] m_rc4.outBuff;
					m_rc4.Input = NULL;
					m_rc4.Output = NULL;
					m_rc4.outBuff = NULL;
				}
				else
				{
					m_rc4.Crypt((char*)m_filepathTextbox->m_string.toAnsiString().c_str(), (char*)m_encString.toAnsiString().c_str(), "RC4Key.txt");
				}
				
				printf("RC4 Encryption ended\n");
			}
		}
		safetyencryptprecaution:
		m_encryptButton->SetCDElapsedTime(m_encryptButton->GetCDElapsedTime() + dt);
		if (m_encryptButton->GetCDElapsedTime() >= m_encryptButton->GetCooldown())
		{
			m_encryptButton->ResetPress();
			m_encryptButton->SetCDElapsedTime(0.0f);
		}
	}

	m_addExtensionButton->Update(dt, m_window);

	m_decryptButton->Update(dt, m_window);
	if (m_decryptButton->IsPressed())
	{
		if (m_decryptButton->GetCDElapsedTime() == 0.0f)
		{
			if (m_currentCipher == ECipher::DES)
			{
				printf("DES Decryption started\n");

				m_decString = m_filepathTextbox->m_string;
				m_decString.insert(m_decString.getSize(), ".dec.des");
				if (m_currentModeButtonPressed == 1)
					m_decString.insert(m_decString.getSize(), ".cbc");
				else
					m_decString.insert(m_decString.getSize(), ".ecb");

				if (m_encryptPixels->IsPressed())
				{
					m_des.LoadKey("DESKey.txt");
					
					if (!m_showoffImage.loadFromFile(m_filepathTextbox->m_string))
					{
						cout << "Failed to load image from file." << endl;
						goto safetydecryptprecaution;
					}
					m_showoffPixelsBuffer = (char*)m_showoffImage.getPixelsPtr();

					char* pxlptr = m_des.DecryptPixels(m_showoffPixelsBuffer, (long long int)m_showoffImage.getSize().x * (long long int)m_showoffImage.getSize().y * 4, m_des.m_keyBuffer, m_currentModeButtonPressed);

					m_showoffECBImage.create(m_showoffImage.getSize().x, m_showoffImage.getSize().y, (sf::Uint8*)pxlptr);

					m_decString.insert(m_decString.getSize(), ".pxl");
					m_decString.insert(m_decString.getSize(), GetFileStringExtension(m_filepathTextbox->m_string));
					m_showoffECBImage.saveToFile(m_decString);

					m_showoffECBTexture.loadFromImage(m_showoffECBImage);
					m_showoffECBSprite.setTexture(m_showoffECBTexture, true);
				}
				else
				{
					if(m_addExtensionButton->IsPressed())
						m_decString.insert(m_decString.getSize(), GetFileStringExtension(m_filepathTextbox->m_string));
					m_des.Decrypt((char*)m_filepathTextbox->m_string.toAnsiString().c_str(), (char*)m_decString.toAnsiString().c_str(), "DESKey.txt", m_currentModeButtonPressed);
				}

				printf("DES Decryption ended\n");
			}

			if (m_currentCipher == ECipher::DES3)
			{
				printf("3DES Decryption started\n");

				m_decString = m_filepathTextbox->m_string;
				m_decString.insert(m_decString.getSize(), ".dec.3des");
				if (m_currentModeButtonPressed == 1)
					m_decString.insert(m_decString.getSize(), ".cbc");
				else
					m_decString.insert(m_decString.getSize(), ".ecb");
				if (m_encryptPixels->IsPressed())
				{
					m_des.Load3DESKey("3DESKey.txt");
					if (!m_showoffImage.loadFromFile(m_filepathTextbox->m_string))
					{
						cout << "Failed to load image from file." << endl;
						goto safetydecryptprecaution;
					}
					m_showoffPixelsBuffer = (char*)m_showoffImage.getPixelsPtr();

					char* pxlptr = m_des.Decrypt3DESPixels(m_showoffPixelsBuffer, (long long int)m_showoffImage.getSize().x * (long long int)m_showoffImage.getSize().y * 4, m_des.m_3DESkeyBuffer, m_currentModeButtonPressed);

					m_showoffECBImage.create(m_showoffImage.getSize().x, m_showoffImage.getSize().y, (sf::Uint8*)pxlptr);

					m_decString.insert(m_decString.getSize(), ".pxl");
					m_decString.insert(m_decString.getSize(), GetFileStringExtension(m_filepathTextbox->m_string));
					m_showoffECBImage.saveToFile(m_decString);

					m_showoffECBTexture.loadFromImage(m_showoffECBImage);
					m_showoffECBSprite.setTexture(m_showoffECBTexture, true);
				}
				else
				{
					if (m_addExtensionButton->IsPressed())
						m_decString.insert(m_decString.getSize(), GetFileStringExtension(m_filepathTextbox->m_string));
					m_des.Decrypt3DES((char*)m_filepathTextbox->m_string.toAnsiString().c_str(), (char*)m_decString.toAnsiString().c_str(), "3DESKey.txt", m_currentModeButtonPressed);
				}
				
				printf("3DES Decryption ended\n");
			}

			if (m_currentCipher == ECipher::AES)
			{
				printf("AES Decryption started\n");
				m_decString = m_filepathTextbox->m_string;
				m_decString.insert(m_decString.getSize(), ".dec.aes");
				
				if (m_currentModeButtonPressed == 1)
					m_decString.insert(m_decString.getSize(), ".cbc");
				else
					m_decString.insert(m_decString.getSize(), ".ecb");

				if (m_encryptPixels->IsPressed())
				{
					m_aes.KeySize = m_aes.LoadKey("AESKey.txt");

					if (!m_showoffImage.loadFromFile(m_filepathTextbox->m_string))
					{
						cout << "Failed to load image from file." << endl;
						goto safetydecryptprecaution;
					}
					m_showoffPixelsBuffer = (char*)m_showoffImage.getPixelsPtr();

					char* pxlptr = m_aes.DecryptPixels(m_showoffPixelsBuffer, (long long int)m_showoffImage.getSize().x * (long long int)m_showoffImage.getSize().y * 4, m_aes.m_keyBuffer, m_currentModeButtonPressed);

					m_showoffECBImage.create(m_showoffImage.getSize().x, m_showoffImage.getSize().y, (sf::Uint8*)pxlptr);

					m_decString.insert(m_decString.getSize(), ".pxl");
					m_decString.insert(m_decString.getSize(), GetFileStringExtension(m_filepathTextbox->m_string));
					m_showoffECBImage.saveToFile(m_decString);

					m_showoffECBTexture.loadFromImage(m_showoffECBImage);
					m_showoffECBSprite.setTexture(m_showoffECBTexture, true);
				}
				else
				{
					if (m_addExtensionButton->IsPressed())
						m_decString.insert(m_decString.getSize(), GetFileStringExtension(m_filepathTextbox->m_string));
					m_aes.Decrypt((char*)m_filepathTextbox->m_string.toAnsiString().c_str(), (char*)m_decString.toAnsiString().c_str(), "AESKey.txt", m_currentModeButtonPressed);
				}
				printf("AES Decryption ended\n");
			}

			if (m_currentCipher == ECipher::RC4)
			{
				printf("RC4 Decryption started\n");
				m_decString = m_filepathTextbox->m_string;
				m_decString.insert(m_decString.getSize(), ".dec.rc4");
				
				if (m_encryptPixels->IsPressed())
				{
					m_rc4.LoadKey("RC4Key.txt");

					if (!m_showoffImage.loadFromFile(m_filepathTextbox->m_string))
					{
						cout << "Failed to load image from file." <<endl;
						goto safetydecryptprecaution;
					};
					m_showoffPixelsBuffer = (char*)m_showoffImage.getPixelsPtr();

					long long int sz = (long long int)m_showoffImage.getSize().x * (long long int)m_showoffImage.getSize().y * 4;
					char* buffer = new char[sz];
					m_rc4.Input = buffer;
					m_rc4.Output = new unsigned char[sz];
					m_rc4.outBuff = new unsigned char[sz];

					m_rc4.m_inputLength = sz;

					char* pxlptr = (char*)m_rc4.CryptPixels(m_showoffPixelsBuffer);

					m_showoffECBImage.create(m_showoffImage.getSize().x, m_showoffImage.getSize().y, (sf::Uint8*)pxlptr);

					m_decString.insert(m_decString.getSize(), ".pxl");
					m_decString.insert(m_decString.getSize(), GetFileStringExtension(m_filepathTextbox->m_string));
					m_showoffECBImage.saveToFile(m_decString);

					m_showoffECBTexture.loadFromImage(m_showoffECBImage);
					m_showoffECBSprite.setTexture(m_showoffECBTexture, true);

					delete[] buffer;
					delete[] m_rc4.Output;
					delete[] m_rc4.outBuff;
					m_rc4.Input = NULL;
					m_rc4.Output = NULL;
					m_rc4.outBuff = NULL;
				}
				else
				{
					if (m_addExtensionButton->IsPressed())
						m_decString.insert(m_decString.getSize(), GetFileStringExtension(m_filepathTextbox->m_string));
					m_rc4.Crypt((char*)m_filepathTextbox->m_string.toAnsiString().c_str(), (char*)m_decString.toAnsiString().c_str(), "RC4Key.txt");
				}
				printf("RC4 Decryption ended\n");
			}
		}
		safetydecryptprecaution:
		m_decryptButton->SetCDElapsedTime(m_decryptButton->GetCDElapsedTime() + dt);
		if (m_decryptButton->GetCDElapsedTime() >= m_decryptButton->GetCooldown())
		{
			m_decryptButton->ResetPress();
			m_decryptButton->SetCDElapsedTime(0.0f);
		}
	}
}

void Menu::Update(float dt)
{
	m_ciphersButtonSystem->Update(dt, m_window);
	m_currentCipherButtonPressed = m_ciphersButtonSystem->GetToggledButton();
	m_currentCipher = m_ciphersButtonSystem->GetToggledButton() + 1;

	m_modesButtonSystem->Update(dt, m_window);
	m_currentModeButtonPressed = m_modesButtonSystem->GetToggledButton();

	ButtonsUpdate(dt);
	m_filepathTextbox->Update(dt, m_window);
}

void Menu::Draw()
{
	m_window.draw(m_bgSprite);

	m_window.draw(m_showoffECBSprite);

	m_window.draw(m_ciphersTitle);
	m_ciphersButtonSystem->Draw(m_window);

	if (m_currentCipher != ECipher::RC4 && m_currentCipher != ECipher::NONE)
	{
		m_window.draw(m_modesTitle);
		m_modesButtonSystem->Draw(m_window);
	}

	m_encryptButton->Draw(m_window);
	m_decryptButton->Draw(m_window);
	m_addExtensionButton->Draw(m_window);
	m_encryptPixels->Draw(m_window);

	m_filepathTextbox->Draw(m_window);
}