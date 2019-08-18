#include "RC4.h"

void RC4::KeySchedule()
{
	for (int i = 0; i < 256; ++i)
	{
		State[i] = i;
	}
	int j = 0;
	for (int i = 0; i < 256; ++i)
	{
		j = (j + State[i] + m_keyBuffer[i % m_keyLen]) % 256;
		swapUChars(State[i], State[j]);
	}
}

void RC4::PseudoRandomGen()
{
	int i = 0, j = 0;
	for (int genNr = 0; genNr < m_inputLength; ++genNr)
	{
		i = (i + 1) % 256;
		j = (j + State[i]) % 256;
		swapUChars(State[i], State[j]);
		Output[genNr] = State[ (State[i] + State[j]) % 256 ];
	}
}

unsigned char* RC4::Cypher()
{
	KeySchedule();
	PseudoRandomGen();
	for (int i = 0; i < m_inputLength; ++i)
	{
		outBuff[i] = Input[i] ^ Output[i];
	}
	return outBuff;
}

void RC4::LoadKey(char* fileInput)
{
	m_keyFile.open(fileInput, ios::binary);
	m_keyFile.ignore(std::numeric_limits<std::streamsize>::max());
	m_keyLen = m_keyFile.gcount();
	if (m_keyLen > 32)
		m_keyLen = 32;
	m_keyFile.clear();
	m_keyFile.seekg(0, m_keyFile.beg);
	m_keyFile.read(m_keyBuffer, m_keyLen);
	m_keyFile.close();
}

void RC4::Crypt(char* fileInput, char* fileOutput, char* keyFile)
{
	LoadKey(keyFile);
	m_fileInput.open(fileInput, ios::binary);
	m_fileOutput.open(fileOutput, ios::binary);

	m_fileInput.ignore(std::numeric_limits<std::streamsize>::max());
	m_fileInputLen = m_fileInput.gcount();
	m_fileInput.clear();
	m_fileInput.seekg(0, m_fileInput.beg);

	char* buffer = new char[m_fileInputLen];
	m_fileInput.read(buffer, m_fileInputLen);

	Input = buffer;
	Output = new unsigned char[m_fileInputLen];
	outBuff = new unsigned char[m_fileInputLen];

	m_inputLength = m_fileInputLen;

	Cypher();

	m_fileOutput.write((char*)outBuff, m_inputLength);

	m_fileInput.close();
	m_fileOutput.close();
	delete[] buffer;
	delete[] Output;
	delete[] outBuff;
	Input = NULL;
	Output = NULL;
	outBuff = NULL;
}

unsigned char* RC4::CryptPixels(char* buffer)
{
	KeySchedule();
	PseudoRandomGen();
	for (int i = 0; i < m_inputLength; ++i)
	{
		outBuff[i] = buffer[i] ^ Output[i];
	}
	return outBuff;
}