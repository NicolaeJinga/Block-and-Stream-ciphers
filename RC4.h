#pragma once
#include<fstream>
#include "Utilities.h"

using namespace std;

class RC4
{
public:
	unsigned char State[256];
	int m_keyLength = 3;
	unsigned char* Key = (unsigned char*)("Key");

	char* Input;
	unsigned char* Output;
	int m_inputLength = 4;
	unsigned char* outBuff;

	void KeySchedule();
	void PseudoRandomGen();
	unsigned char* Cypher();

	void LoadKey(char* fileInput);
	
	ifstream m_keyFile;
	int m_keyLen = 0;
	char m_keyBuffer[32];
	ifstream m_fileInput;
	ofstream m_fileOutput;
	long long int m_fileInputLen = 0;

	void Crypt(char* fileInput, char* fileOutput, char* keyFile);
	unsigned char* CryptPixels(char* buffer);
};