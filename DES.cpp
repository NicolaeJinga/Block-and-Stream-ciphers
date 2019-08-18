#include "DES.h"
#include "Utilities.h"
#include <iostream>
#include <iomanip>
//using namespace Utilities;

inline void DES::SetMessage(char* msg, bool hex)
{
	long long int m = 0;
	long long int posInLength = 0;
	//long long int len = strlen(msg);
	if (!hex)
	{
		for (int i = 0; i < 8; ++i)
		{
			m += (long long int)msg[posInLength++] << (7 - i) * 8;
		}
	}
	else
	{
		for (int i = 0; i < 16; ++i)
		{

			m += (long long int)charHexDigitToInt(msg[posInLength++]) << (15 - i) * 4;
		}
	}
	Messagef = m;
}

inline void DES::SetKey(char* key, bool hex)
{
	long long int k = 0;
	int posInLength = 0;
	int len = strlen(key);
	//set key bits; if NOT hex, read input as string, else read as hex
	if (!hex)
	{
		memcpy(&k, key, 8);
	}
	else
	{
		for (int i = 0; i < len; ++i)
		{
			k |= (long long int)charHexDigitToInt(key[posInLength++]) << (15 - i) * 4;
			//(15 - i) * 4 = little endian; (len - 1 - i) * 4 = big endian
		}
	}
	Keyf = k;
}

inline long long int DES::Set3DESKey(char* key, bool hex, bool lastKey)
{
	long long int k = 0;
	int posInLength = 0;
	int len;
	if (hex)
	{
		if (lastKey)
		{
			len = strlen(key);
		}
		else
		{
			len = 16;
		}
		for (int i = 0; i < 16 - len; ++i)
		{
			k |= 0 << (15 - i) * 4;
		}
		for (int i = 0; i < len; ++i)
		{
			k |= (long long int)charHexDigitToInt(key[posInLength++]) << (15 - i) * 4;
		}
	}
	else
	{
		memcpy(&k, key, 8);
	}
	return k;
}

inline void DES::PermutedChoice1()
{
	for (int i = 0; i < 56; ++i)
	{
		changeBitL(KeyAfterPC1, 55 - i, getBitL(Keyf, 63 - (((int*)PC1)[i] - 1)));
	}
}

inline void DES::PermutedChoice2()
{
	for (int i = 0; i < 48; ++i)
	{
		changeBitL(KeyAfterPC2, 47 - i, getBitL(KeyAfterLS, 55 - (((int*)PC2)[i] - 1)));
	}
}

inline void DES::KeySchedule()
{
	int c, d;
	PermutedChoice1();
	//divide 56 key bit into 2 halves, c and d.
	c = KeyAfterPC1 >> 28;
	d = KeyAfterPC1;
	//clean up the 4 left-most bits because c and d are ints (32bits) and we only need 28 bits.
	c &= 0x0FFFFFFF;
	d &= 0x0FFFFFFF;
	int nrShifts = 0;
	int rotBit;
	//int mask = 0xFFFFFFF; // to clear first 4 bits (left2right) of c and d.

	for (int rounds = 0; rounds < 16; ++rounds)
	{
		//rounds 1,2,9,16 do 1 left shift, rest do 2 left shifts.
		if (rounds == 0 || rounds == 1 || rounds == 8 || rounds == 15)
		{
			nrShifts = 1;
		}
		else
		{
			nrShifts = 2;
		}
		while(nrShifts > 0)
		{
			//get left-most bit of c
			rotBit = getBit(c, 27);
			//shift it with 1 pos
			c <<= 1;
			//set the right-most bit with the left-most bit we've just got.
			setBit(c, 0, rotBit);
			//kill the left-most bit which is now the 29th bit.
			c &= 0xFFFFFFF;
			//analog for d.
			rotBit = getBit(d, 27);
			d <<= 1;
			setBit(d, 0, rotBit);
			d &= 0xFFFFFFF;
			nrShifts--;
		}
		//combine c and d into a single variable.
		KeyAfterLS = c;
		KeyAfterLS <<= 28;
		KeyAfterLS |= d;

		PermutedChoice2();

		SubKeys[rounds] = KeyAfterPC2;
	}
}

inline void DES::InitialPermutation()
{
	for (int i = 0; i < 64; ++i)
	{
		changeBitL(MsgAfterIP, 63 - i, getBitL(Messagef, 63 - (((int*)IP)[i] - 1)));
	}
}

inline void DES::SplitMessage()
{
	Left  = MsgAfterIP >> 32;
	Right = MsgAfterIP;
}

inline void DES::ExpansionInF()
{
	for (int i = 0; i < 48; ++i)
	{
		changeBitL(RightAfterExpansion, 47 - i, getBitL(Right, 31 - (((int*)Expansion)[i] - 1)));
	}
}

inline void DES::XorInFeistel(int round, bool decrypt)
{
	if (!decrypt)
	{
		XorInFeistelNetwork = RightAfterExpansion ^ SubKeys[round];
	}
	else
	{
		XorInFeistelNetwork = RightAfterExpansion ^ SubKeys[15-round];
	}
}

inline void DES::XorInFeistelEnc(int round)
{
	XorInFeistelNetwork = RightAfterExpansion ^ SubKeys[round];
}

inline void DES::XorInFeistelDec(int round)
{
	XorInFeistelNetwork = RightAfterExpansion ^ SubKeys[15 - round];
}

inline int DES::GetSBoxElement(int nr, int sbox)
{
	int row = nr >> 5; 
	//int mask = 0x1;
	row &= 0x1;
	row = row << 1; 
	row |= getBit(nr,0);

	int column = nr >> 1;
	//mask = 0xF; // = 001111;
	column &= 0xF;
	return SBoxes[sbox][row][column];
}

inline void DES::SBoxComputation()
{
	RightAfterSBoxes = 0;
	for (int i = 0; i < 8; ++i)
	{
		//int SBoxElement = GetSBoxElement(XorInFeistelNetwork >> (7-i) * 6, i);
		RightAfterSBoxes |= GetSBoxElement(XorInFeistelNetwork >> (7 - i) * 6, i) << ((7 - i) << 2); // "<<2" == "*4"
	}
}

inline void DES::PermutationInF()
{
	for (int i = 0;i < 32; ++i)
	{
		changeBit(RightAfterPermutation, 31 - i, getBit(RightAfterSBoxes, 31 - (((int*)Permutation)[i] - 1)));
	}
}

inline void DES::XorOutOfFeistel()
{
	XorOutOfFeistelNetwork = Left ^ RightAfterPermutation;
}

inline void DES::CrissCross()
{
	Left = Right;
	Right = XorOutOfFeistelNetwork;
}

inline void DES::SwapLeftRight()
{
	for (int i = 0; i < 32; ++i)
	{
		changeBitL(AfterRoundsLeftRightSwap, i + 32, getBit(Right, i));
		changeBitL(AfterRoundsLeftRightSwap, i, getBit(Left, i));
	}
}

inline void DES::InversePermutation()
{
	for (int i = 0; i < 64; ++i)
	{
		changeBitL(MsgAfterInversePermutation, 63 - i, getBitL(AfterRoundsLeftRightSwap, 63 - (((int*)IPInverse)[i]/*[i / 8][i % 8]*/ - 1)));
	}
}

long long int DES::Cypher(bool decrypt)
{
	KeySchedule();
	InitialPermutation();
	SplitMessage();
	for (int round = 0; round < 16; ++round)
	{
		ExpansionInF();
		XorInFeistel(round, decrypt);
		SBoxComputation();
		PermutationInF();
		XorOutOfFeistelNetwork = Left ^ RightAfterPermutation;//XorOutOfFeistel(); 
		CrissCross();
	}
	SwapLeftRight();
	InversePermutation();
	return MsgAfterInversePermutation;
}

long long int DES::Encipher()
{
	KeySchedule();
	InitialPermutation();
	SplitMessage();
	for (int round = 0; round < 16; ++round)
	{
		ExpansionInF();
		XorInFeistelNetwork = RightAfterExpansion ^ SubKeys[round];//XorInFeistelEnc(round);
		SBoxComputation();
		PermutationInF();
		XorOutOfFeistelNetwork = Left ^ RightAfterPermutation;//XorOutOfFeistel(); 
		CrissCross();
	}
	SwapLeftRight();
	InversePermutation();
	return MsgAfterInversePermutation;
}

long long int DES::Decipher()
{
	KeySchedule();
	InitialPermutation();
	SplitMessage();
	for (int round = 0; round < 16; ++round)
	{
		ExpansionInF();
		XorInFeistelNetwork = RightAfterExpansion ^ SubKeys[15 - round];//XorInFeistelDec(round);
		SBoxComputation();
		PermutationInF();
		XorOutOfFeistelNetwork = Left ^ RightAfterPermutation;//XorOutOfFeistel(); 
		CrissCross();
	}
	SwapLeftRight();
	InversePermutation();
	return MsgAfterInversePermutation;
}

void DES::InitInitVector(long long int iv)
{
	InitVector = iv;
}

void DES::Crypt(char* input, long long int inputSize, char* key, ofstream& out, bool decrypt, bool hexInput, bool hexKey)
{

	SetKey(key,hexKey);
	long long int inputLength = inputSize;
	int hexIn = 0;
	if (hexInput)
	{
		hexIn = 16;
	}
	else
	{
		hexIn = 8;
	}
	int mod = inputLength % hexIn;
	
	char* cinput = new char[inputLength + hexIn - mod];
	memcpy(cinput, input, inputSize);

	//pad the input
	if (mod != 0)
	{
		for (int i = 0; i <  hexIn - mod; ++i)
		{
			cinput[inputLength++] = 0x00;
		}
	}

	char* msg = new char[hexIn];
	char* outbuff = new char[inputLength];
	char* currentPos = outbuff;
	for (int i = 0; i < inputLength / hexIn; ++i)//for each block
	{
		SetMessage(msg, hexInput); //compute block
	
		Messagef = Cypher(decrypt); //encrypt block

		memcpy(currentPos, &Messagef, sizeof(Messagef));
		currentPos += sizeof(Messagef);
	}
	out.write(outbuff, inputLength);

	delete[] outbuff;
	delete[] msg;
	delete[] cinput;
}

void DES::Encrypt(char* input, long long int inputSize, char* key, ofstream& out, bool hexKey)
{
	SetKey(key, hexKey);
	long long int inputLength = inputSize;
	int mod = inputLength % 8;

	char* cinput = new char[inputLength + 8 - mod];
	memcpy(cinput, input, inputSize);

	//pad the input
	if (mod != 0)
	{
		for (int i = 0; i < 8 - mod; ++i)
		{
			cinput[inputLength++] = 0x00;
		}
	}

	char* msg = new char[8];
	char* outbuff = new char[inputLength];
	char* currentPos = outbuff;

	if (m_mode == Mode::CBC)
	{
		InitVector = 0xDA39A3EE5E6B4B0D;
	}

	for (int i = 0; i < inputLength / 8; ++i)//for each block
	{
		memcpy(&Messagef, &cinput[i * 8], 8); //set block

		Messagef = swapLong(Messagef);
		if (!hexKey)
			Keyf = swapLong(Keyf);

		if (m_mode == Mode::CBC)
		{
			if (i != 0)
				Messagef ^= CBCCarryOver;
			else
				Messagef ^= InitVector;
		}

		Messagef = Encipher(); //encrypt block		
		
		if (m_mode == Mode::CBC)
		{
			CBCCarryOver = Messagef;
		}

		Messagef = swapLong(Messagef);
		if (!hexKey)
			Keyf = swapLong(Keyf);

		memcpy(currentPos, &Messagef, sizeof(Messagef));
		currentPos += sizeof(Messagef);
	}
	out.write(outbuff, inputLength);

	delete[] outbuff;
	delete[] msg;
	delete[] cinput;
}

char* DES::EncryptPixels(char* input, long long int inputSize, char* key, int mode, bool hexKey)
{
	m_mode = (Mode)mode;
	SetKey(key, hexKey);
	long long int inputLength = inputSize;
	int mod = inputLength % 8;

	char* cinput = new char[inputLength + 8 - mod];
	memcpy(cinput, input, inputSize);

	//pad the input
	if (mod != 0)
	{
		for (int i = 0; i < 8 - mod; ++i)
		{
			cinput[inputLength++] = 0x00;
		}
	}

	char* msg = new char[8];
	char* outbuff = new char[inputLength];
	char* currentPos = outbuff;

	if (m_mode == Mode::CBC)
	{
		InitVector = 0xDA39A3EE5E6B4B0D;
	}

	for (int i = 0; i < inputLength / 8; ++i)//for each block
	{
		memcpy(&Messagef, &cinput[i * 8], 8); //set block

		Messagef = swapLong(Messagef);
		if (!hexKey)
			Keyf = swapLong(Keyf);

		if (m_mode == Mode::CBC)
		{
			if (i != 0)
				Messagef ^= CBCCarryOver;
			else
				Messagef ^= InitVector;
		}

		Messagef = Encipher(); //encrypt block		

		if (m_mode == Mode::CBC)
		{
			CBCCarryOver = Messagef;
		}

		Messagef = swapLong(Messagef);
		if (!hexKey)
			Keyf = swapLong(Keyf);

		memcpy(currentPos, &Messagef, sizeof(Messagef));
		currentPos += sizeof(Messagef);
	}

	delete[] msg;
	delete[] cinput;
	return outbuff;
}

void DES::Decrypt(char* input, long long int inputSize, char* key, ofstream& out, bool hexKey)
{
	SetKey(key, hexKey);
	long long int inputLength = inputSize;
	//cout << inputLength << endl;
	int mod = inputLength % 8;

	char* cinput = new char[inputLength + 8 - mod];
	memcpy(cinput, input, inputSize);

	//pad the input
	if (mod != 0)
	{
		for (int i = 0; i < 8 - mod; ++i)
		{
			cinput[inputLength++] = 0x00;
		}
	}

	char* msg = new char[8];
	char* outbuff = new char[inputLength];
	char* currentPos = outbuff;

	if (m_mode == Mode::CBC)
	{
		InitVector = 0xDA39A3EE5E6B4B0D;
	}

	for (int i = 0; i < inputLength / 8; ++i)//for each block
	{
		memcpy(&Messagef, &cinput[i * 8], 8); //set block

		Messagef = swapLong(Messagef);
		if(!hexKey)
			Keyf = swapLong(Keyf);

		if (m_mode == Mode::CBC)
		{
			CBCCarryOver = Messagef;
		}

		Messagef = Decipher(); //decrypt block		
		
		if (m_mode == Mode::CBC)
		{
			if (i != 0)
				Messagef ^= DecryptCBCCarryOver;
			else
				Messagef ^= InitVector;

			DecryptCBCCarryOver = CBCCarryOver;
		}

		Messagef = swapLong(Messagef);
		if (!hexKey)
			Keyf = swapLong(Keyf);

		memcpy(currentPos, &Messagef, sizeof(Messagef));
		currentPos += sizeof(Messagef);
	}
	out.write(outbuff, inputLength);

	delete[] outbuff;
	delete[] msg;
	delete[] cinput;
}

char* DES::DecryptPixels(char* input, long long int inputSize, char* key, int mode, bool hexKey)
{
	m_mode = Mode(mode);
	SetKey(key, hexKey);
	long long int inputLength = inputSize;
	int mod = inputLength % 8;

	char* cinput = new char[inputLength + 8 - mod];
	memcpy(cinput, input, inputSize);

	//pad the input
	if (mod != 0)
	{
		for (int i = 0; i < 8 - mod; ++i)
		{
			cinput[inputLength++] = 0x00;
		}
	}

	char* msg = new char[8];
	char* outbuff = new char[inputLength];
	char* currentPos = outbuff;

	if (m_mode == Mode::CBC)
	{
		InitVector = 0xDA39A3EE5E6B4B0D;
	}

	for (int i = 0; i < inputLength / 8; ++i)//for each block
	{
		memcpy(&Messagef, &cinput[i * 8], 8); //set block

		Messagef = swapLong(Messagef);
		if (!hexKey)
			Keyf = swapLong(Keyf);

		if (m_mode == Mode::CBC)
		{
			CBCCarryOver = Messagef;
		}

		Messagef = Decipher(); //decrypt block		

		if (m_mode == Mode::CBC)
		{
			if (i != 0)
				Messagef ^= DecryptCBCCarryOver;
			else
				Messagef ^= InitVector;

			DecryptCBCCarryOver = CBCCarryOver;
		}

		Messagef = swapLong(Messagef);
		if (!hexKey)
			Keyf = swapLong(Keyf);

		memcpy(currentPos, &Messagef, sizeof(Messagef));
		currentPos += sizeof(Messagef);
	}

	delete[] msg;
	delete[] cinput;
	return outbuff;
}

void DES::LoadKey(char* fileInput)
{
	m_keyFile.open(fileInput, ios::binary);
	m_keyFile.ignore(std::numeric_limits<std::streamsize>::max());
	m_keyLen = m_keyFile.gcount();
	if (m_keyLen > 16) //16 for hex actually; if plain it will be memcpyed with 8 in SetKey() function
		m_keyLen = 16;
	m_keyFile.clear();
	m_keyFile.seekg(0, m_keyFile.beg);
	m_keyFile.read(m_keyBuffer, m_keyLen);
	m_keyFile.close();
}

void DES::Encrypt(char* fileInput, char* fileOutput, char* keyFile, int mode, bool readKeyAsHex)
{
	m_mode = (Mode)mode;
	LoadKey(keyFile);
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

void DES::Decrypt(char* fileInput, char* fileOutput, char* keyFile, int mode, bool readKeyAsHex)
{
	m_mode = (Mode)mode;
	LoadKey(keyFile);
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

int DES::Load3DESKey(char* fileInput, bool loadAsHex)
{
	m_keyFile.open(fileInput, ios::binary);
	m_keyFile.ignore(std::numeric_limits<std::streamsize>::max());
	m_keyLen = m_keyFile.gcount();
	if (loadAsHex)
	{
		if (m_keyLen > 48)
			m_keyLen = 48;
	}
	else
	{
		if (m_keyLen > 24)
			m_keyLen = 24;
	}
	m_keyFile.clear();
	m_keyFile.seekg(0, m_keyFile.beg);
	m_keyFile.read(m_3DESkeyBuffer, m_keyLen);
	m_keyFile.close();
	if (loadAsHex)
	{
		//keyOption1
		if (m_keyLen <= 16)
			m_nrOf3DESkeys = 1;
		else
			//keyOption2
			if (m_keyLen <= 32)
				m_nrOf3DESkeys = 2;
			else
				//keyOption3
				if (m_keyLen <= 48)
					m_nrOf3DESkeys = 3;
	}
	else
	{
		//keyOption1
		if (m_keyLen <= 8)
			m_nrOf3DESkeys = 1;
		else
			//keyOption2
			if (m_keyLen <= 16)
				m_nrOf3DESkeys = 2;
			else
				//keyOption3
				if (m_keyLen <= 24)
					m_nrOf3DESkeys = 3;
	}
	return m_nrOf3DESkeys;
}

void DES::Encrypt3DES(char* fileInput, char* fileOutput, char* keyFile, int mode, bool readKeyAsHex)
{
	m_mode = (Mode)mode;
	Load3DESKey(keyFile, readKeyAsHex);
	m_fileInput.open(fileInput, ios::binary);
	m_fileOutput.open(fileOutput, ios::binary);
	
	m_fileInput.ignore(std::numeric_limits<std::streamsize>::max());
	m_fileInputLen = m_fileInput.gcount();
	m_fileInput.clear();
	m_fileInput.seekg(0, m_fileInput.beg);
	
	char* buffer = new char[m_fileInputLen];
	m_fileInput.read(buffer, m_fileInputLen);
	
	if (m_mode == Mode::CBC)
	{
		InitVector = 0xDA39A3EE5E6B4B0D;
	}

	//first key
	switch (m_nrOf3DESkeys) //cases represent key options
	{
	case 1:
	{
		m_3DESkey1 = Set3DESKey(m_3DESkeyBuffer, readKeyAsHex);
		Keyf = m_3DESkey1;
		break;
	}
	case 2:
	{
		m_3DESkey1 = Set3DESKey(m_3DESkeyBuffer, readKeyAsHex, false);
		Keyf = m_3DESkey1;
		break;
	}
	case 3:
	{
		m_3DESkey1 = Set3DESKey(m_3DESkeyBuffer, readKeyAsHex, false);
		Keyf = m_3DESkey1;
		break;
	}
	}
	
	long long int inputLength = m_fileInputLen;
	int mod = inputLength % 8;
	
	char* cinput = new char[inputLength + 8 - mod];
	memcpy(cinput, buffer, m_fileInputLen);
	
	//pad the input
	if (mod != 0)
	{
		for (int i = 0; i < 8 - mod; ++i)
		{
			cinput[inputLength++] = 0x00;
		}
	}
	
	char* msg = new char[8];
	char* outbuff = new char[inputLength];
	char* currentPos = outbuff;
	for (int i = 0; i < inputLength / 8; ++i)//for each block
	{
		memcpy(&Messagef, &cinput[i * 8], 8); //set block
		Messagef = swapLong(Messagef);
		if (!readKeyAsHex)
			Keyf = swapLong(Keyf);

		if (m_mode == Mode::CBC)
		{
			if (i != 0)
				Messagef ^= CBCCarryOver;
			else
				Messagef ^= InitVector;
		}

		Messagef = Encipher(); //encrypt block		

		if (m_mode == Mode::CBC)
		{
			CBCCarryOver = Messagef;
		}

		Messagef = swapLong(Messagef);
		if (!readKeyAsHex)
			Keyf = swapLong(Keyf);
	
		memcpy(currentPos, &Messagef, sizeof(Messagef));
		currentPos += sizeof(Messagef);
	}
	
	//second key
	switch (m_nrOf3DESkeys)
	{
	case 1:
	{
		m_3DESkey2 = Set3DESKey(m_3DESkeyBuffer, readKeyAsHex);
		Keyf = m_3DESkey2;
		break;
	}
	case 2:
	{
		m_3DESkey2 = Set3DESKey(m_3DESkeyBuffer + 8, readKeyAsHex);
		if(readKeyAsHex)
			m_3DESkey2 = Set3DESKey(m_3DESkeyBuffer + 16, readKeyAsHex);
		Keyf = m_3DESkey2;
		break;
	}
	case 3:
	{
		m_3DESkey2 = Set3DESKey(m_3DESkeyBuffer + 8, readKeyAsHex);
		if (readKeyAsHex)
			m_3DESkey2 = Set3DESKey(m_3DESkeyBuffer + 16, readKeyAsHex, false);
		Keyf = m_3DESkey2;
		break;
	}
	}
	
	memcpy(cinput, outbuff, inputLength);
	currentPos = outbuff;
	for (int i = 0; i < inputLength / 8; ++i)//for each block
	{
		memcpy(&Messagef, &cinput[i * 8], 8); //set block
		Messagef = swapLong(Messagef);
		if (!readKeyAsHex)
			Keyf = swapLong(Keyf);

		if (m_mode == Mode::CBC)
		{
			CBCCarryOver = Messagef;
		}

		Messagef = Decipher(); //encrypt block	

		if (m_mode == Mode::CBC)
		{
			if (i != 0)
				Messagef ^= DecryptCBCCarryOver;
			else
				Messagef ^= InitVector;

			DecryptCBCCarryOver = CBCCarryOver;
		}

		Messagef = swapLong(Messagef);
		if (!readKeyAsHex)
			Keyf = swapLong(Keyf);

		memcpy(currentPos, &Messagef, sizeof(Messagef));
		currentPos += sizeof(Messagef);
	}
	
	//third key
	switch (m_nrOf3DESkeys)
	{
	case 1:
	{
		m_3DESkey3 = Set3DESKey(m_3DESkeyBuffer, readKeyAsHex);
		Keyf = m_3DESkey3;
		break;
	}
	case 2:
	{
		m_3DESkey3 = Set3DESKey(m_3DESkeyBuffer, readKeyAsHex, false);
		Keyf = m_3DESkey3;
		break;
	}
	case 3:
	{
		m_3DESkey3 = Set3DESKey(m_3DESkeyBuffer + 16, readKeyAsHex);
		if (readKeyAsHex)
			m_3DESkey3 = Set3DESKey(m_3DESkeyBuffer + 32, readKeyAsHex);
		Keyf = m_3DESkey3;
		break;
	}
	}
	
	memcpy(cinput, outbuff, inputLength);
	currentPos = outbuff;
	for (int i = 0; i < inputLength / 8; ++i)//for each block
	{
		memcpy(&Messagef, &cinput[i * 8], 8); //set block
		Messagef = swapLong(Messagef);
		if (!readKeyAsHex)
			Keyf = swapLong(Keyf);

		if (m_mode == Mode::CBC)
		{
			if (i != 0)
				Messagef ^= CBCCarryOver;
			else
				Messagef ^= InitVector;
		}

		Messagef = Encipher(); //encrypt block

		if (m_mode == Mode::CBC)
		{
			CBCCarryOver = Messagef;
		}

		Messagef = swapLong(Messagef);
		if (!readKeyAsHex)
			Keyf = swapLong(Keyf);

		memcpy(currentPos, &Messagef, sizeof(Messagef));
		currentPos += sizeof(Messagef);
	}
	
	m_fileOutput.write(outbuff, inputLength);
	delete[] outbuff;
	delete[] msg;
	delete[] cinput;
	
	m_fileInput.close();
	m_fileOutput.close();
	delete[] buffer;
	
}

void DES::Decrypt3DES(char* fileInput, char* fileOutput, char* keyFile, int mode, bool readKeyAsHex)
{
	m_mode = (Mode)mode;
	Load3DESKey(keyFile, readKeyAsHex);
	m_fileInput.open(fileInput, ios::binary);
	m_fileOutput.open(fileOutput, ios::binary);
	
	m_fileInput.ignore(std::numeric_limits<std::streamsize>::max());
	m_fileInputLen = m_fileInput.gcount();
	m_fileInput.clear();
	m_fileInput.seekg(0, m_fileInput.beg);
	
	char* buffer = new char[m_fileInputLen];
	m_fileInput.read(buffer, m_fileInputLen);
	
	if (m_mode == Mode::CBC)
	{
		InitVector = 0xDA39A3EE5E6B4B0D;
	}

	switch (m_nrOf3DESkeys)
	{
	case 1:
	{
		m_3DESkey1 = Set3DESKey(m_3DESkeyBuffer, readKeyAsHex);
		Keyf = m_3DESkey1;
		break;
	}
	case 2:
	{
		m_3DESkey1 = Set3DESKey(m_3DESkeyBuffer, readKeyAsHex, false);
		Keyf = m_3DESkey1;
		break;
	}
	case 3:
	{
		m_3DESkey1 = Set3DESKey(m_3DESkeyBuffer + 16, readKeyAsHex);
		if (readKeyAsHex)
			m_3DESkey1 = Set3DESKey(m_3DESkeyBuffer + 32, readKeyAsHex);
		Keyf = m_3DESkey1;
		break;
	}
	}
	
	long long int inputLength = m_fileInputLen;
	int mod = inputLength % 8;
	
	char* cinput = new char[inputLength + 8 - mod];
	memcpy(cinput, buffer, m_fileInputLen);
	
	//pad the input
	if (mod != 0)
	{
		for (int i = 0; i < 8 - mod; ++i)
		{
			cinput[inputLength++] = 0x00;
		}
	}
	
	char* msg = new char[8];
	char* outbuff = new char[inputLength];
	char* currentPos = outbuff;
	for (int i = 0; i < inputLength / 8; ++i)//for each block
	{
		memcpy(&Messagef, &cinput[i * 8], 8); //set block
		Messagef = swapLong(Messagef);
		if (!readKeyAsHex)
			Keyf = swapLong(Keyf);

		if (m_mode == Mode::CBC)
		{
			CBCCarryOver = Messagef;
		}

		Messagef = Decipher(); //encrypt block		

		if (m_mode == Mode::CBC)
		{
			if (i != 0)
				Messagef ^= DecryptCBCCarryOver;
			else
				Messagef ^= InitVector;

			DecryptCBCCarryOver = CBCCarryOver;
		}

		Messagef = swapLong(Messagef);
		if (!readKeyAsHex)
			Keyf = swapLong(Keyf);
	
		memcpy(currentPos, &Messagef, sizeof(Messagef));
		currentPos += sizeof(Messagef);
	}
	
	switch (m_nrOf3DESkeys)
	{
	case 1:
	{
		m_3DESkey2 = Set3DESKey(m_3DESkeyBuffer, readKeyAsHex);
		Keyf = m_3DESkey2;
		break;
	}
	case 2:
	{
		m_3DESkey2 = Set3DESKey(m_3DESkeyBuffer + 8, readKeyAsHex);
		if (readKeyAsHex)
			m_3DESkey2 = Set3DESKey(m_3DESkeyBuffer + 16, readKeyAsHex);
		Keyf = m_3DESkey2;
		break;
	}
	case 3:
	{
		m_3DESkey2 = Set3DESKey(m_3DESkeyBuffer + 8, readKeyAsHex);
		if (readKeyAsHex)
			m_3DESkey2 = Set3DESKey(m_3DESkeyBuffer + 16, readKeyAsHex, false);
		Keyf = m_3DESkey2;
		break;
	}
	}
	
	memcpy(cinput, outbuff, inputLength);
	currentPos = outbuff;
	for (int i = 0; i < inputLength / 8; ++i)//for each block
	{
		memcpy(&Messagef, &cinput[i * 8], 8); //set block
		Messagef = swapLong(Messagef);
		if (!readKeyAsHex)
			Keyf = swapLong(Keyf);

		if (m_mode == Mode::CBC)
		{
			if (i != 0)
				Messagef ^= CBCCarryOver;
			else
				Messagef ^= InitVector;
		}

		Messagef = Encipher(); //encrypt block		

		if (m_mode == Mode::CBC)
		{
			CBCCarryOver = Messagef;
		}

		Messagef = swapLong(Messagef);
		if (!readKeyAsHex)
			Keyf = swapLong(Keyf);
	
		memcpy(currentPos, &Messagef, sizeof(Messagef));
		currentPos += sizeof(Messagef);
	}
	
	
	switch (m_nrOf3DESkeys)
	{
	case 1:
	{ 
		m_3DESkey3 = Set3DESKey(m_3DESkeyBuffer, readKeyAsHex);
		Keyf = m_3DESkey3;
		break;
	}
	case 2:
	{
		m_3DESkey3 = Set3DESKey(m_3DESkeyBuffer, readKeyAsHex, false);
		Keyf = m_3DESkey3;
		break;
	}
	case 3:
	{
		m_3DESkey3 = Set3DESKey(m_3DESkeyBuffer, readKeyAsHex, false);
		Keyf = m_3DESkey3;
		break;
	}
	}
	
	memcpy(cinput, outbuff, inputLength);
	currentPos = outbuff;
	for (int i = 0; i < inputLength / 8; ++i)//for each block
	{
		memcpy(&Messagef, &cinput[i * 8], 8); //set block
		Messagef = swapLong(Messagef);
		if (!readKeyAsHex)
			Keyf = swapLong(Keyf);

		if (m_mode == Mode::CBC)
		{
			CBCCarryOver = Messagef;
		}

		Messagef = Decipher(); //encrypt block	

		if (m_mode == Mode::CBC)
		{
			if (i != 0)
				Messagef ^= DecryptCBCCarryOver;
			else
				Messagef ^= InitVector;

			DecryptCBCCarryOver = CBCCarryOver;
		}

		Messagef = swapLong(Messagef);
		if (!readKeyAsHex)
			Keyf = swapLong(Keyf);
	
		memcpy(currentPos, &Messagef, sizeof(Messagef));
		currentPos += sizeof(Messagef);
	}
	
	m_fileOutput.write(outbuff, inputLength);
	
	delete[] outbuff;
	delete[] msg;
	delete[] cinput;
	
	m_fileInput.close();
	m_fileOutput.close();
	delete[] buffer;
}

char* DES::Encrypt3DESPixels(char* input, long long int inputLen, char* keyFile, int mode, bool readKeyAsHex)
{
	m_mode = (Mode)mode;
	m_fileInputLen = inputLen;
	char* buffer = new char[m_fileInputLen];
	memcpy(buffer, input, inputLen);
	

	if (m_mode == Mode::CBC)
	{
		InitVector = 0xDA39A3EE5E6B4B0D;
	}

	//first key
	switch (m_nrOf3DESkeys) //cases represent key options
	{
	case 1:
	{
		m_3DESkey1 = Set3DESKey(m_3DESkeyBuffer, readKeyAsHex);
		Keyf = m_3DESkey1;
		break;
	}
	case 2:
	{
		m_3DESkey1 = Set3DESKey(m_3DESkeyBuffer, readKeyAsHex, false);
		Keyf = m_3DESkey1;
		break;
	}
	case 3:
	{
		m_3DESkey1 = Set3DESKey(m_3DESkeyBuffer, readKeyAsHex, false);
		Keyf = m_3DESkey1;
		break;
	}
	}

	long long int inputLength = m_fileInputLen;
	int mod = inputLength % 8;

	char* cinput = new char[inputLength + 8 - mod];
	memcpy(cinput, buffer, m_fileInputLen);

	//pad the input
	if (mod != 0)
	{
		for (int i = 0; i < 8 - mod; ++i)
		{
			cinput[inputLength++] = 0x00;
		}
	}

	char* msg = new char[8];
	char* outbuff = new char[inputLength];
	char* currentPos = outbuff;
	for (int i = 0; i < inputLength / 8; ++i)//for each block
	{
		memcpy(&Messagef, &cinput[i * 8], 8); //set block
		Messagef = swapLong(Messagef);
		if (!readKeyAsHex)
			Keyf = swapLong(Keyf);

		if (m_mode == Mode::CBC)
		{
			if (i != 0)
				Messagef ^= CBCCarryOver;
			else
				Messagef ^= InitVector;
		}

		Messagef = Encipher(); //encrypt block		

		if (m_mode == Mode::CBC)
		{
			CBCCarryOver = Messagef;
		}

		Messagef = swapLong(Messagef);
		if (!readKeyAsHex)
			Keyf = swapLong(Keyf);

		memcpy(currentPos, &Messagef, sizeof(Messagef));
		currentPos += sizeof(Messagef);
	}

	//second key
	switch (m_nrOf3DESkeys)
	{
	case 1:
	{
		m_3DESkey2 = Set3DESKey(m_3DESkeyBuffer, readKeyAsHex);
		Keyf = m_3DESkey2;
		break;
	}
	case 2:
	{
		m_3DESkey2 = Set3DESKey(m_3DESkeyBuffer + 8, readKeyAsHex);
		if (readKeyAsHex)
			m_3DESkey2 = Set3DESKey(m_3DESkeyBuffer + 16, readKeyAsHex);
		Keyf = m_3DESkey2;
		break;
	}
	case 3:
	{
		m_3DESkey2 = Set3DESKey(m_3DESkeyBuffer + 8, readKeyAsHex);
		if (readKeyAsHex)
			m_3DESkey2 = Set3DESKey(m_3DESkeyBuffer + 16, readKeyAsHex, false);
		Keyf = m_3DESkey2;
		break;
	}
	}

	memcpy(cinput, outbuff, inputLength);
	currentPos = outbuff;
	for (int i = 0; i < inputLength / 8; ++i)//for each block
	{
		memcpy(&Messagef, &cinput[i * 8], 8); //set block
		Messagef = swapLong(Messagef);
		if (!readKeyAsHex)
			Keyf = swapLong(Keyf);

		if (m_mode == Mode::CBC)
		{
			CBCCarryOver = Messagef;
		}

		Messagef = Decipher(); //encrypt block	

		if (m_mode == Mode::CBC)
		{
			if (i != 0)
				Messagef ^= DecryptCBCCarryOver;
			else
				Messagef ^= InitVector;

			DecryptCBCCarryOver = CBCCarryOver;
		}

		Messagef = swapLong(Messagef);
		if (!readKeyAsHex)
			Keyf = swapLong(Keyf);

		memcpy(currentPos, &Messagef, sizeof(Messagef));
		currentPos += sizeof(Messagef);
	}

	//third key
	switch (m_nrOf3DESkeys)
	{
	case 1:
	{
		m_3DESkey3 = Set3DESKey(m_3DESkeyBuffer, readKeyAsHex);
		Keyf = m_3DESkey3;
		break;
	}
	case 2:
	{
		m_3DESkey3 = Set3DESKey(m_3DESkeyBuffer, readKeyAsHex, false);
		Keyf = m_3DESkey3;
		break;
	}
	case 3:
	{
		m_3DESkey3 = Set3DESKey(m_3DESkeyBuffer + 16, readKeyAsHex);
		if (readKeyAsHex)
			m_3DESkey3 = Set3DESKey(m_3DESkeyBuffer + 32, readKeyAsHex);
		Keyf = m_3DESkey3;
		break;
	}
	}

	memcpy(cinput, outbuff, inputLength);
	currentPos = outbuff;
	for (int i = 0; i < inputLength / 8; ++i)//for each block
	{
		memcpy(&Messagef, &cinput[i * 8], 8); //set block
		Messagef = swapLong(Messagef);
		if (!readKeyAsHex)
			Keyf = swapLong(Keyf);

		if (m_mode == Mode::CBC)
		{
			if (i != 0)
				Messagef ^= CBCCarryOver;
			else
				Messagef ^= InitVector;
		}

		Messagef = Encipher(); //encrypt block

		if (m_mode == Mode::CBC)
		{
			CBCCarryOver = Messagef;
		}

		Messagef = swapLong(Messagef);
		if (!readKeyAsHex)
			Keyf = swapLong(Keyf);

		memcpy(currentPos, &Messagef, sizeof(Messagef));
		currentPos += sizeof(Messagef);
	}
	
	delete[] msg;
	delete[] cinput;
	delete[] buffer;
	return outbuff;
}

char* DES::Decrypt3DESPixels(char* input, long long int inputLen, char* keyFile, int mode, bool readKeyAsHex)
{
	m_mode = (Mode)mode;
	m_fileInputLen = inputLen;
	char* buffer = new char[m_fileInputLen];
	memcpy(buffer, input, inputLen);
	

	if (m_mode == Mode::CBC)
	{
		InitVector = 0xDA39A3EE5E6B4B0D;
	}

	switch (m_nrOf3DESkeys)
	{
	case 1:
	{
		m_3DESkey1 = Set3DESKey(m_3DESkeyBuffer, readKeyAsHex);
		Keyf = m_3DESkey1;
		break;
	}
	case 2:
	{
		m_3DESkey1 = Set3DESKey(m_3DESkeyBuffer, readKeyAsHex, false);
		Keyf = m_3DESkey1;
		break;
	}
	case 3:
	{
		m_3DESkey1 = Set3DESKey(m_3DESkeyBuffer + 16, readKeyAsHex);
		if (readKeyAsHex)
			m_3DESkey1 = Set3DESKey(m_3DESkeyBuffer + 32, readKeyAsHex);
		Keyf = m_3DESkey1;
		break;
	}
	}

	long long int inputLength = m_fileInputLen;
	//cout << inputLength << endl;
	int mod = inputLength % 8;

	char* cinput = new char[inputLength + 8 - mod];
	memcpy(cinput, buffer, m_fileInputLen);

	//pad the input
	if (mod != 0)
	{
		for (int i = 0; i < 8 - mod; ++i)
		{
			cinput[inputLength++] = 0x00;
		}
	}

	char* msg = new char[8];
	char* outbuff = new char[inputLength];
	char* currentPos = outbuff;
	for (int i = 0; i < inputLength / 8; ++i)//for each block
	{
		memcpy(&Messagef, &cinput[i * 8], 8); //set block
		Messagef = swapLong(Messagef);
		if (!readKeyAsHex)
			Keyf = swapLong(Keyf);

		if (m_mode == Mode::CBC)
		{
			CBCCarryOver = Messagef;
		}

		Messagef = Decipher(); //encrypt block		

		if (m_mode == Mode::CBC)
		{
			if (i != 0)
				Messagef ^= DecryptCBCCarryOver;
			else
				Messagef ^= InitVector;

			DecryptCBCCarryOver = CBCCarryOver;
		}

		Messagef = swapLong(Messagef);
		if (!readKeyAsHex)
			Keyf = swapLong(Keyf);

		memcpy(currentPos, &Messagef, sizeof(Messagef));
		currentPos += sizeof(Messagef);
	}

	switch (m_nrOf3DESkeys)
	{
	case 1:
	{
		m_3DESkey2 = Set3DESKey(m_3DESkeyBuffer, readKeyAsHex);
		Keyf = m_3DESkey2;
		break;
	}
	case 2:
	{
		m_3DESkey2 = Set3DESKey(m_3DESkeyBuffer + 8, readKeyAsHex);
		if (readKeyAsHex)
			m_3DESkey2 = Set3DESKey(m_3DESkeyBuffer + 16, readKeyAsHex);
		Keyf = m_3DESkey2;
		break;
	}
	case 3:
	{
		m_3DESkey2 = Set3DESKey(m_3DESkeyBuffer + 8, readKeyAsHex);
		if (readKeyAsHex)
			m_3DESkey2 = Set3DESKey(m_3DESkeyBuffer + 16, readKeyAsHex, false);
		Keyf = m_3DESkey2;
		break;
	}
	}

	memcpy(cinput, outbuff, inputLength);
	currentPos = outbuff;
	for (int i = 0; i < inputLength / 8; ++i)//for each block
	{
		memcpy(&Messagef, &cinput[i * 8], 8); //set block
		Messagef = swapLong(Messagef);
		if (!readKeyAsHex)
			Keyf = swapLong(Keyf);

		if (m_mode == Mode::CBC)
		{
			if (i != 0)
				Messagef ^= CBCCarryOver;
			else
				Messagef ^= InitVector;
		}

		Messagef = Encipher(); //encrypt block		

		if (m_mode == Mode::CBC)
		{
			CBCCarryOver = Messagef;
		}

		Messagef = swapLong(Messagef);
		if (!readKeyAsHex)
			Keyf = swapLong(Keyf);

		memcpy(currentPos, &Messagef, sizeof(Messagef));
		currentPos += sizeof(Messagef);
	}


	switch (m_nrOf3DESkeys)
	{
	case 1:
	{
		m_3DESkey3 = Set3DESKey(m_3DESkeyBuffer, readKeyAsHex);
		Keyf = m_3DESkey3;
		break;
	}
	case 2:
	{
		m_3DESkey3 = Set3DESKey(m_3DESkeyBuffer, readKeyAsHex, false);
		Keyf = m_3DESkey3;
		break;
	}
	case 3:
	{
		m_3DESkey3 = Set3DESKey(m_3DESkeyBuffer, readKeyAsHex, false);
		Keyf = m_3DESkey3;
		break;
	}
	}

	memcpy(cinput, outbuff, inputLength);
	currentPos = outbuff;
	for (int i = 0; i < inputLength / 8; ++i)//for each block
	{
		memcpy(&Messagef, &cinput[i * 8], 8); //set block
		Messagef = swapLong(Messagef);
		if (!readKeyAsHex)
			Keyf = swapLong(Keyf);

		if (m_mode == Mode::CBC)
		{
			CBCCarryOver = Messagef;
		}

		Messagef = Decipher(); //encrypt block	

		if (m_mode == Mode::CBC)
		{
			if (i != 0)
				Messagef ^= DecryptCBCCarryOver;
			else
				Messagef ^= InitVector;

			DecryptCBCCarryOver = CBCCarryOver;
		}

		Messagef = swapLong(Messagef);
		if (!readKeyAsHex)
			Keyf = swapLong(Keyf);

		memcpy(currentPos, &Messagef, sizeof(Messagef));
		currentPos += sizeof(Messagef);
	}

	delete[] msg;
	delete[] cinput;
	delete[] buffer;

	return outbuff;
}