#include "AES.h"
#include<iostream>

AES::AES()
{
	memset(&Message, 0, 16 * sizeof(char));
	memset(&Key, 0, 32 * sizeof(char));
}

void AES::SubBytes()
{
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			State[i][j] = SBox[State[i][j] >> 4][State[i][j] & 0x0F];
		}
	}
}

void AES::ShiftRows()
{
	//left shift row 1 by 1
	unsigned int stateElem = State[1][0];
	State[1][0] = State[1][1];
	State[1][1] = State[1][2];
	State[1][2] = State[1][3];
	State[1][3] = stateElem;
	//left shift row 2 by 2
	stateElem = State[2][0];
	State[2][0] = State[2][2];
	State[2][2] = stateElem;
	stateElem = State[2][1];
	State[2][1] = State[2][3];
	State[2][3] = stateElem;
	//left shift row 3 by 3;
	stateElem = State[3][0];
	State[3][0] = State[3][3];
	State[3][3] = State[3][2];
	State[3][2] = State[3][1];
	State[3][1] = stateElem;
}

//Multiplication by x: multiply a byte by 02 (which translate to x in GF(2^8); 01*byte=byte; 03*byte=(02+01)*byte=02*byte+byte; 02*byte = xtime(byte)
unsigned char AES::xtime(unsigned char x)
{
	//x = (x0,x1,..,x7); if x0 = 0, XOR with 0, else XOR with 0x1b (=0001 1011) (=irr polyn x^8+x^4+x^3+x+1)
	return (x << 1) ^ (x & 0x80 ? 0x1b : 0); 
}

void AES::MixColumns()
{
	unsigned char state_s[4][4];
	for (int i = 0; i < 4; ++i)
	{
		state_s[0][i] = xtime(State[0][i]) ^ (xtime(State[1][i]) ^ State[1][i]) ^ State[2][i] ^ State[3][i];
		state_s[1][i] = State[0][i] ^ xtime(State[1][i]) ^ (xtime(State[2][i]) ^ State[2][i]) ^ State[3][i];
		state_s[2][i] = State[0][i] ^ State[1][i] ^ xtime(State[2][i]) ^ (xtime(State[3][i]) ^ State[3][i]);
		state_s[3][i] = (xtime(State[0][i]) ^ State[0][i]) ^ State[1][i] ^ State[2][i] ^ xtime(State[3][i]);
	}
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			State[i][j] = state_s[i][j];
		}
	}
}

void AES::AddRoundKey(unsigned int w[])
{
	unsigned char subkey[4];
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			subkey[j] = w[i] >> ((3-j) * 8);
			State[j][i] ^= subkey[j];
		}
	}
}


unsigned int AES::SubWord(unsigned int word)
{
	unsigned int subword = 0;
	subword  = (int)SBox[(word >>  4) & 0x0000000F][ word        & 0x0000000F]		;
	subword ^= (int)SBox[(word >> 12) & 0x0000000F][(word >>  8) & 0x0000000F] <<  8;
	subword ^= (int)SBox[(word >> 20) & 0x0000000F][(word >> 16) & 0x0000000F] << 16;
	subword ^= (int)SBox[(word >> 28) & 0x0000000F][(word >> 24) & 0x0000000F] << 24;
	return subword;
}

unsigned int AES::RotWord(unsigned word)
{
	return ((word << 8) | (word >> 24));
}

void AES::KeyExpansion(unsigned char key[], unsigned int w[], int keysize)
{
	int Nb = 4, Nr, Nk;
	switch (keysize)
	{
	case 128:
	{
		Nr = 10;
		Nk = 4;
		break;
	}
	case 192:
	{
		Nr = 12;
		Nk = 6;
		break;
	}
	case 256:
	{
		Nr = 14;
		Nk = 8;
		break;
	}
	default:
	{
		return;
	}
	}
	unsigned int Rcon[] = { 0x01000000, 0x02000000, 0x04000000, 0x08000000, 0x10000000, 0x20000000, 0x40000000, 0x80000000,
							0x1b000000, 0x36000000, 0x6c000000, 0xd8000000, 0xab000000, 0x4d000000, 0x9a000000 };
	unsigned int temp;
	for (int i = 0; i < Nk; ++i)
	{
		w[i] = ((key[4 * i]) << 24) | ((key[4 * i + 1]) << 16) | ((key[4 * i + 2]) << 8) | (key[4 * i + 3]);
	}
	for (int i = Nk; i < Nb*(Nr + 1); ++i)
	{
		temp = w[i - 1];
		if (i % Nk == 0)
		{
			temp = SubWord(RotWord(temp)) ^ Rcon[(i - 1) / Nk];
		}
		else if (Nk > 6 && (i % Nk == 4))
		{
			temp = SubWord(temp);
		}
		w[i] = w[i - Nk] ^ temp;
	}
}

void AES::SetMessage(unsigned char in[])
{
	for (int i = 0; i < 16; ++i)
	{
		State[i % 4][i / 4] = in[i];
	}
}

void AES::CopyStateToOut(unsigned char out[])
{
	for (int i = 0; i < 16; ++i)
	{
		out[i] = State[i % 4][i / 4];
	}
}

unsigned char* AES::Encipher()
{
	SetMessage(Message);

	KeyExpansion(Key, KeySchedule, KeySize);
	AddRoundKey(&KeySchedule[0]);
	for (int i = 1; i < 10; ++i)
	{
		SubBytes();
		ShiftRows();
		MixColumns();
		AddRoundKey(&KeySchedule[i * 4]);
	}
	//decide which round is last based on key length
	if (KeySize == 128)
	{
		SubBytes(); ShiftRows(); AddRoundKey(&KeySchedule[40]);
	}
	else
	{
		SubBytes(); ShiftRows(); MixColumns(); AddRoundKey(&KeySchedule[40]);
		SubBytes(); ShiftRows(); MixColumns(); AddRoundKey(&KeySchedule[44]);
		if (KeySize == 192)
		{
			SubBytes(); ShiftRows(); AddRoundKey(&KeySchedule[48]);
		}
		else if(KeySize == 256)
		{
			SubBytes(); ShiftRows(); MixColumns(); AddRoundKey(&KeySchedule[48]);
			SubBytes(); ShiftRows(); MixColumns(); AddRoundKey(&KeySchedule[52]);
			SubBytes(); ShiftRows(); AddRoundKey(&KeySchedule[56]);
		}
	}

	CopyStateToOut(Output);
	return Output;
}

void AES::InvShiftRows()
{
	//right shift row 1 by 1
	unsigned int stateElem = State[1][3];
	State[1][3] = State[1][2];
	State[1][2] = State[1][1];
	State[1][1] = State[1][0];
	State[1][0] = stateElem;
	//right shift row 2 by 2
	stateElem = State[2][3];
	State[2][3] = State[2][1];
	State[2][1] = stateElem;
	stateElem = State[2][2];
	State[2][2] = State[2][0];
	State[2][0] = stateElem;
	//right shift row 3 by 3
	stateElem = State[3][3];
	State[3][3] = State[3][0];
	State[3][0] = State[3][1];
	State[3][1] = State[3][2];
	State[3][2] = stateElem;
}

void AES::InvSubBytes()
{
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			State[i][j] = InvSBox[State[i][j] >> 4][State[i][j] & 0x0F];
		}
	}
}

unsigned char AES::Multiply(unsigned char a, unsigned char b)
{
	return (((b		 & 1) * a) ^
			((b >> 1 & 1) * xtime(a)) ^
			((b >> 2 & 1) * xtime(xtime(a))) ^
			((b >> 3 & 1) * xtime(xtime(xtime(a)))) ^
			((b >> 4 & 1) * xtime(xtime(xtime(xtime(a))))));
}

void AES::InvMixColumns()
{
	unsigned char a, b, c, d;
	for (int i = 0; i < 4; ++i)
	{
		a = State[0][i];
		b = State[1][i];
		c = State[2][i];
		d = State[3][i];

		State[0][i] = Multiply(a, 0x0e) ^ Multiply(b, 0x0b) ^ Multiply(c, 0x0d) ^ Multiply(d, 0x09);
		State[1][i] = Multiply(a, 0x09) ^ Multiply(b, 0x0e) ^ Multiply(c, 0x0b) ^ Multiply(d, 0x0d);
		State[2][i] = Multiply(a, 0x0d) ^ Multiply(b, 0x09) ^ Multiply(c, 0x0e) ^ Multiply(d, 0x0b);
		State[3][i] = Multiply(a, 0x0b) ^ Multiply(b, 0x0d) ^ Multiply(c, 0x09) ^ Multiply(d, 0x0e);
	}
}

unsigned char* AES::Decipher()
{
	SetMessage(Message);

	KeyExpansion(Key, KeySchedule, KeySize);

	//decide which round is last based on key length
	if (KeySize == 128)
	{
		AddRoundKey(&KeySchedule[40]);
	}
	else
	{

		if (KeySize == 192)
		{
			AddRoundKey(&KeySchedule[48]);
		}
		else if (KeySize == 256)
		{
			AddRoundKey(&KeySchedule[56]);
			InvShiftRows(); InvSubBytes(); AddRoundKey(&KeySchedule[52]); InvMixColumns();
			InvShiftRows(); InvSubBytes(); AddRoundKey(&KeySchedule[48]); InvMixColumns();
		}
		InvShiftRows(); InvSubBytes(); AddRoundKey(&KeySchedule[44]); InvMixColumns();
		InvShiftRows(); InvSubBytes(); AddRoundKey(&KeySchedule[40]); InvMixColumns();
	}
	for (int i = 9; i > 0; --i)
	{
		InvShiftRows(); InvSubBytes(); AddRoundKey(&KeySchedule[4 * i]); InvMixColumns();
	}
	InvShiftRows(); InvSubBytes(); AddRoundKey(&KeySchedule[0]);

	CopyStateToOut(Output);
	return Output;
}

int AES::LoadKey(char* fileInput, bool loadAsHex)
{
	m_keyFile.open(fileInput, ios::binary);
	m_keyFile.ignore(std::numeric_limits<std::streamsize>::max());
	m_keyLen = m_keyFile.gcount();
	if (loadAsHex)
	{
		if (m_keyLen > 64)
			m_keyLen = 64;
	}
	else
	{
		if (m_keyLen > 32)
			m_keyLen = 32;
	}
	m_keyFile.clear();
	m_keyFile.seekg(0, m_keyFile.beg);
	m_keyFile.read(m_keyBuffer, m_keyLen);
	m_keyFile.close();
	if (loadAsHex)
	{
		if (m_keyLen <= 32)
			m_keySize = 128;
		else
			if (m_keyLen <= 48)
				m_keySize = 192;
			else
				if (m_keyLen <= 64)
					m_keySize = 256;
	}
	else
	{
		if (m_keyLen <= 16)
			m_keySize = 128;
		else
			if (m_keyLen <= 24)
				m_keySize = 192;
			else
				if (m_keyLen <= 32)
					m_keySize = 256;
	}
	return m_keySize;
}

void AES::SetKey(char* key, bool hex)
{
	int len = m_keyLen;
	for (int i = 0; i < len; ++i)
	{
		Key[i] = *(key + i);
	}
}

void AES::Encrypt(char* input, long long int inputSize, char* key, ofstream& out, bool hexKey)
{
	SetKey(key, hexKey);
	long long int inputLength = inputSize;
	int mod = inputLength % 16;

	char* cinput = new char[inputLength + 16 - mod];
	memcpy(cinput, input, inputSize);

	//pad the input
	if (mod != 0)
	{
		for (int i = 0; i < 16 - mod; ++i)
		{
			cinput[inputLength++] = 0x00;
		}
	}
	
	char* msg = new char[16];
	char* outbuff = new char[inputLength];
	char* currentPos = outbuff;

	if (m_mode == Mode::CBC)
	{
		unsigned char iv[16] = { 0xd7, 0xb3, 0x17, 0x9d, 0x7e, 0xf5, 0xbd, 0x18, 0xbb, 0xb2, 0xb8, 0x64, 0xe5, 0x90, 0xa2, 0x8d };
		for (int i = 0; i < 16; ++i)
			InitVector[i] = iv[i];
	}

	for (int i = 0; i < inputLength / 16; ++i) //for each block
	{
		memcpy(&Message, &cinput[i * 16], 16);

		if (m_mode == Mode::CBC)
		{
			if (i != 0)
				for (int j = 0; j < 16; ++j)
					Message[j] ^= CBCCarryOver[j];
			else
				for (int j = 0; j < 16; ++j)
					Message[j] ^= InitVector[j];
		}

		Encipher();

		for (int j = 0; j < 16; ++j)
		{
			Message[j] = Output[j];
		}

		if (m_mode == Mode::CBC)
		{
			for (int j = 0; j < 16; ++j)
				CBCCarryOver[j] = Message[j];
		}

		
		memcpy(currentPos, &Message, 16);
		currentPos += 16;
	}
	out.write(outbuff, inputLength);

	delete[] outbuff;
	delete[] msg;
	delete[] cinput;
}

char* AES::EncryptPixels(char* input, long long int inputSize, char* key, int mode, bool hexKey)
{
	m_mode = (Mode)mode;
	SetKey(key, hexKey);
	long long int inputLength = inputSize;
	int mod = inputLength % 16;

	char* cinput = new char[inputLength + 16 - mod];
	memcpy(cinput, input, inputSize);

	//pad the input
	if (mod != 0)
	{
		for (int i = 0; i < 16 - mod; ++i)
		{
			cinput[inputLength++] = 0x00;
		}
	}

	char* msg = new char[16];
	char* outbuff = new char[inputLength];
	char* currentPos = outbuff;

	if (m_mode == Mode::CBC)
	{
		unsigned char iv[16] = { 0xd7, 0xb3, 0x17, 0x9d, 0x7e, 0xf5, 0xbd, 0x18, 0xbb, 0xb2, 0xb8, 0x64, 0xe5, 0x90, 0xa2, 0x8d };
		for (int i = 0; i < 16; ++i)
			InitVector[i] = iv[i];
	}

	for (int i = 0; i < inputLength / 16; ++i) //for each block
	{
		memcpy(&Message, &cinput[i * 16], 16);

		if (m_mode == Mode::CBC)
		{
			if (i != 0)
				for (int j = 0; j < 16; ++j)
					Message[j] ^= CBCCarryOver[j];
			else
				for (int j = 0; j < 16; ++j)
					Message[j] ^= InitVector[j];
		}

		Encipher();

		for (int j = 0; j < 16; ++j)
		{
			Message[j] = Output[j];
		}

		if (m_mode == Mode::CBC)
		{
			for (int j = 0; j < 16; ++j)
				CBCCarryOver[j] = Message[j];
		}


		memcpy(currentPos, &Message, 16);
		currentPos += 16;
	}

	delete[] msg;
	delete[] cinput;
	return outbuff;
}

void AES::Decrypt(char* input, long long int inputSize, char* key, ofstream& out, bool hexKey)
{
	SetKey(key, hexKey);
	long long int inputLength = inputSize;
	int mod = inputLength % 16;

	char* cinput = new char[inputLength + 16 - mod];
	memcpy(cinput, input, inputSize);

	//pad the input
	if (mod != 0)
	{
		for (int i = 0; i < 16 - mod; ++i)
		{
			cinput[inputLength++] = 0x00;
		}
	}

	char* msg = new char[16];
	char* outbuff = new char[inputLength];
	char* currentPos = outbuff;

	if (m_mode == Mode::CBC)
	{
		unsigned char iv[16] = { 0xd7, 0xb3, 0x17, 0x9d, 0x7e, 0xf5, 0xbd, 0x18, 0xbb, 0xb2, 0xb8, 0x64, 0xe5, 0x90, 0xa2, 0x8d };
		for (int i = 0; i < 16; ++i)
			InitVector[i] = iv[i];
	}

	for (int i = 0; i < inputLength / 16; ++i) //for each block
	{
		memcpy(&Message, &cinput[i * 16], 16);

		if (m_mode == Mode::CBC)
		{
			for (int j = 0; j < 16; ++j)
				CBCCarryOver[j] = Message[j];
		}

		Decipher();

		for (int j = 0; j < 16; ++j)
		{
			Message[j] = Output[j];
		}

		if (m_mode == Mode::CBC)
		{
			if (i != 0)
				for (int j = 0; j < 16; ++j)
					Message[j] ^= DecryptCBCCarryOver[j];
			else
				for (int j = 0; j < 16; ++j)
					Message[j] ^= InitVector[j];

			for (int j = 0; j < 16; ++j)
				DecryptCBCCarryOver[j] = CBCCarryOver[j];
		}

		
		memcpy(currentPos, &Message, 16);
		currentPos += 16;
	}
	out.write(outbuff, inputLength);

	delete[] outbuff;
	delete[] msg;
	delete[] cinput;
}

char* AES::DecryptPixels(char* input, long long int inputSize, char* key, int mode, bool hexKey)
{
	m_mode = (Mode)mode;
	SetKey(key, hexKey);
	long long int inputLength = inputSize;
	int mod = inputLength % 16;

	char* cinput = new char[inputLength + 16 - mod];
	memcpy(cinput, input, inputSize);

	//pad the input
	if (mod != 0)
	{
		for (int i = 0; i < 16 - mod; ++i)
		{
			cinput[inputLength++] = 0x00;
		}
	}

	char* msg = new char[16];
	char* outbuff = new char[inputLength];
	char* currentPos = outbuff;

	if (m_mode == Mode::CBC)
	{
		unsigned char iv[16] = { 0xd7, 0xb3, 0x17, 0x9d, 0x7e, 0xf5, 0xbd, 0x18, 0xbb, 0xb2, 0xb8, 0x64, 0xe5, 0x90, 0xa2, 0x8d };
		for (int i = 0; i < 16; ++i)
			InitVector[i] = iv[i];
	}

	for (int i = 0; i < inputLength / 16; ++i) //for each block
	{
		memcpy(&Message, &cinput[i * 16], 16);

		if (m_mode == Mode::CBC)
		{
			for (int j = 0; j < 16; ++j)
				CBCCarryOver[j] = Message[j];
		}

		Decipher();

		for (int j = 0; j < 16; ++j)
		{
			Message[j] = Output[j];
		}

		if (m_mode == Mode::CBC)
		{
			if (i != 0)
				for (int j = 0; j < 16; ++j)
					Message[j] ^= DecryptCBCCarryOver[j];
			else
				for (int j = 0; j < 16; ++j)
					Message[j] ^= InitVector[j];

			for (int j = 0; j < 16; ++j)
				DecryptCBCCarryOver[j] = CBCCarryOver[j];
		}


		memcpy(currentPos, &Message, 16);
		currentPos += 16;
	}

	delete[] msg;
	delete[] cinput;
	return outbuff;
}

void AES::Encrypt(char* fileInput, char* fileOutput, char* keyFile, int mode, bool readKeyAsHex)
{
	m_mode = (Mode)mode;
	KeySize = LoadKey(keyFile, readKeyAsHex);
	m_fileInput.open(fileInput, ios::binary);
	m_fileOutput.open(fileOutput, ios::binary);

	m_fileInput.ignore(std::numeric_limits<std::streamsize>::max());
	m_fileInputLen = m_fileInput.gcount();
	m_fileInput.clear();
	m_fileInput.seekg(0, m_fileInput.beg);

	char* buffer = new char[m_fileInputLen];
	m_fileInput.read(buffer, m_fileInputLen);
	Encrypt(buffer, m_fileInputLen, m_keyBuffer, m_fileOutput, readKeyAsHex);

	m_fileInput.close();
	m_fileOutput.close();
	delete[] buffer;
}

void AES::Decrypt(char* fileInput, char* fileOutput, char* keyFile, int mode, bool readKeyAsHex)
{
	m_mode = (Mode)mode;
	KeySize = LoadKey(keyFile, readKeyAsHex);
	m_fileInput.open(fileInput, ios::binary);
	m_fileOutput.open(fileOutput, ios::binary);

	m_fileInput.ignore(std::numeric_limits<std::streamsize>::max());
	m_fileInputLen = m_fileInput.gcount();
	m_fileInput.clear();
	m_fileInput.seekg(0, m_fileInput.beg);

	char* buffer = new char[m_fileInputLen];
	m_fileInput.read(buffer, m_fileInputLen);
	Decrypt(buffer, m_fileInputLen, m_keyBuffer, m_fileOutput, readKeyAsHex);

	m_fileInput.close();
	m_fileOutput.close();
	delete[] buffer;
}